
#include <string.h>
#include <stdio.h>
#include "thinfat32.h"

TFInfo tf_info;
TFFile tf_file_handles[TF_FILE_HANDLES];

int read_sector(char *data, uint32_t sector) {
    FILE *fp;
    fp = fopen("./test.fat32", "r+b");

    fseek(fp, sector * 512, 0);
    fread(data, 1, 512, fp);
    fclose(fp);
    return 0;
}

int write_sector(char *data, uint32_t blocknum) {
    FILE *fp;
    fp = fopen("./test.fat32", "r+");

    fseek(fp, blocknum * 512, 0);
    fwrite(data, 1, 512, fp);
    fclose(fp);
    return 0;
}

int tf_fetch(uint32_t sector) {
    int rc = 0;
    if(sector == tf_info.currentSector) {
        return 0;
    }

    if(tf_info.sectorFlags & TF_FLAG_DIRTY) {
        rc |= tf_store();
    }

    rc |= read_sector( tf_info.buffer, sector );
    if(!rc) 
        tf_info.currentSector = sector;
    return rc;
}

int tf_store() {
    tf_info.sectorFlags &= ~TF_FLAG_DIRTY;
    return write_sector(tf_info.buffer, tf_info.currentSector);
}

int tf_init() {
    BPB_struct *bpb;
    uint32_t fat_size, root_dir_sectors, data_sectors, cluster_count, temp;
    TFFile *fp;
    FatFileEntry e;

    tf_info.currentSector = -1;
    tf_info.sectorFlags = 0;
    tf_fetch(0);
    //    if (((unsigned char)tf_info.buffer[510]) == 0x55 &&
    //            ((unsigned char)tf_info.buffer[511]) == 0xaa) {
    //        start_sector = tf_info.buffer[0x1c6] +
    //                (tf_info.buffer[0x1c7] << 8) +
    //                (tf_info.buffer[0x1c8] << 16) +
    //                (tf_info.buffer[0x1c9] << 23);
    //        printf("start_sector %d \r\n", start_sector);
    //        tf_fetch(0);    //once again, fetch 0, but with offset "start_sector"
    //    }

    // Cast to a BPB, so we can extract relevant data
    bpb = (BPB_struct *) tf_info.buffer;

    // BS_jmpBoot needs to contain specific instructions
    if (!(bpb->BS_JumpBoot[0] == 0xEB && bpb->BS_JumpBoot[2] == 0x90) && !(bpb->BS_JumpBoot[0] == 0xE9)) {
        return TF_ERR_BAD_FS_TYPE;
    }

    // Only 512 bytes are supported by thinfat at the moment
    if (bpb->BytesPerSector != 512) {
        return TF_ERR_BAD_FS_TYPE;
    }
    if (bpb->ReservedSectorCount == 0) {
        return TF_ERR_BAD_FS_TYPE;
    }

    // Valid media types
    if ((bpb->Media != 0xF0) && (bpb->Media < 0xF8)) {
        return TF_ERR_BAD_FS_TYPE;
    }

    // fang: for FAT32, <RootEntryCount> field is 0
    fat_size = (bpb->FATSize16 != 0) ? bpb->FATSize16 : bpb->FSTypeSpecificData.fat32.FATSize;
    root_dir_sectors = ((bpb->RootEntryCount * 32) + (bpb->BytesPerSector-1))/(512);
    tf_info.totalSectors = (bpb->TotalSectors16 != 0) ? bpb->TotalSectors16 : bpb->TotalSectors32;
    data_sectors = tf_info.totalSectors - (bpb->ReservedSectorCount + (bpb->NumFATs * fat_size) + root_dir_sectors);
    tf_info.sectorsPerCluster = bpb->SectorsPerCluster;
    cluster_count = data_sectors/tf_info.sectorsPerCluster;
    tf_info.reservedSectors = bpb->ReservedSectorCount;
    tf_info.firstDataSector = bpb->ReservedSectorCount + (bpb->NumFATs * fat_size) + root_dir_sectors;

    // Now that we know the total count of clusters, we can compute the FAT type
    if(cluster_count < 65525) {
        return TF_ERR_BAD_FS_TYPE;
    } else {
        tf_info.type = TF_TYPE_FAT32;
    }

    // TODO ADD SANITY CHECKING HERE (CHECK THE BOOT SIGNATURE, ETC... ETC...)
    tf_info.rootDirectorySize = 0xffffffff;
    temp = 0;

    // Like recording the root directory size!
    fp = tf_fopen("/", "r");
    do {
        temp += sizeof(FatFileEntry);
        tf_fread((char *)&e, sizeof(FatFileEntry), fp);
    } while(e.msdos.filename[0] != '\x00' && (unsigned char)e.msdos.filename[0] != 0xe5);

    tf_fclose(fp);
    tf_info.rootDirectorySize = temp;

    tf_fetch(0);
    printBPB((BPB_struct*)tf_info.buffer);
    return 0;    
}

//获取fat表中某个cluster对应的值
uint32_t tf_get_fat_entry(uint32_t cluster) {
    uint32_t offset = cluster * 4;
    tf_fetch(tf_info.reservedSectors + (offset / 512)); // 512 is hardcoded bpb->bytesPerSector
    return *((uint32_t *) &(tf_info.buffer[offset % 512]));
}

int tf_set_fat_entry(uint32_t cluster, uint32_t value) {
    uint32_t offset;
    int rc;
    offset = cluster * 4; // FAT32
    rc = tf_fetch(tf_info.reservedSectors + (offset/512)); // 512 is hardcoded bpb->bytesPerSector
    if (*((uint32_t *) &(tf_info.buffer[offset % 512])) != value) {
        tf_info.sectorFlags |= TF_FLAG_DIRTY; // Mark this sector as dirty
        *((uint32_t *) &(tf_info.buffer[offset % 512])) = value;
    }
    return rc;
}

// Walk the FAT from the very first data sector and find a cluster that's available
// Return the cluster index
uint32_t tf_find_free_cluster() {
    uint32_t i, entry, totalClusters;

    totalClusters = tf_info.totalSectors / tf_info.sectorsPerCluster;
    for(i = 0; i < totalClusters; i++) {
        entry = tf_get_fat_entry(i);
        if((entry & 0x0fffffff) == 0)
            break;
    }
    return i;
}

uint32_t tf_find_free_cluster_from(uint32_t c) {
    uint32_t i, entry, totalClusters;
    totalClusters = tf_info.totalSectors / tf_info.sectorsPerCluster;
    for(i = c; i < totalClusters; i++) {
        entry = tf_get_fat_entry(i);
        if((entry & 0x0fffffff) == 0)
            break;
    }
    // We couldn't find anything here so search from the beginning
    if (i == totalClusters) {
        return tf_find_free_cluster();
    }
    return i;
}

int tf_free_clusterchain(uint32_t cluster) {
    uint32_t fat_entry;
    fat_entry = tf_get_fat_entry(cluster);
    while(fat_entry < TF_MARK_EOC32) {  //fang: this chain should end with TF_MARK_EOC32
        if (fat_entry <= 2) {       // catch-all to save root directory from corrupted stuff
            break;
        }
        tf_set_fat_entry(cluster, 0x00000000);

        fat_entry = tf_get_fat_entry(fat_entry);
        cluster = fat_entry;    //fang: 覆盖之前先记录下一个cluster的位置
    }
    return 0;
}

// Return the index of the first sector for the provided cluster
uint32_t tf_first_sector(uint32_t cluster) {
    return ((cluster - 2) * tf_info.sectorsPerCluster) + tf_info.firstDataSector;
}

//将entry中的文件名和 name(遇到第一个'/'前的部分) 匹配
//匹配成功返回1,否则返回0
int tf_compare_filename_segment(FatFileEntry *entry, char *name) {
    int i, j, res = 0;
    char reformatted_file[16];
    char *entryname = entry->msdos.filename;

    if(entry->msdos.attributes != 0x0f) {   //8.3 Segment
        j = 0;
        for(i = 0; i < 8; i++) {    //Filename
            if(entryname[i] != ' ') {
                reformatted_file[j++] = entryname[i];
            }
        }
        reformatted_file[j++] = '.';
        for(i = 8; i < 11; i++) {   //Extension
            if(entryname[i] != ' ') {
                reformatted_file[j++] = entryname[i];
            }
        }
    } else {                                //LFN Segment
        j = 0;
        for(i = 0; i < 5; i++) {
            reformatted_file[j++] = (char) entry->lfn.name1[i];
        }
        for(i = 0; i < 6; i++) {
            reformatted_file[j++] = (char) entry->lfn.name2[i];
        }
        for(i = 0; i < 2; i++) {
            reformatted_file[j++] = (char) entry->lfn.name3[i];
        }
    }
    reformatted_file[j] = '\0';     // entry中的名字长度(包含了填充字节0xff)就是j

    i = 0;
    while((name[i] != '/') && (name[i] != '\x00'))  //计算要匹配的目标的长度
        i++;

    if (i > 13) {   //lfn最多也就是13个字符长度
        if (strncasecmp(name, reformatted_file, 13)) {
            res = 0;
        } else {
            res = 1;
        }
    } else {
        if (reformatted_file[i] != 0 || strncasecmp(name, reformatted_file, i)) {
            res = 0;    //reformatted_file[i] != 0 说明目标长度比entry中的名字短
        } else {
            res = 1;
        }
    }
    return res;
}

//   1 for entry matches filename.  Side effect: fp seeks to that entry
//   0 for entry doesn't match filename.  Side effect: fp seeks to next entry
//   -1 for couldn't read an entry, due to EOF or other fread error
//match返回的时候 fp 指向的是与name 匹配的 文件的 dos entry
//not match, 则 fp 指向的是下一个文件对应的 entry(可能是lfn entry)
int tf_compare_filename(TFFile *fp, char *name) {
    uint32_t i = 0;
    FatFileEntry entry;
    char *compare_name = name;
    uint32_t lfn_entries;
    
    tf_fread((char *)&entry, sizeof(FatFileEntry), fp);
    
    // Fail if its bogus
    if(entry.msdos.filename[0] == 0x00)
        return -1;

    if(entry.msdos.attributes != 0x0f) {    // it's a DOS entry
        if(1 == tf_compare_filename_segment(&entry, name)) {
            tf_fseek(fp, (int32_t)-sizeof(FatFileEntry), fp->pos);
            return 1;   //match
        } else {
            return 0;   //doesn't match
        }
    } else if ((entry.lfn.sequence_number & 0xc0) == 0x40) {    //这里是一个lfn的entry
        lfn_entries = entry.lfn.sequence_number & ~0x40;

        tf_fseek(fp, (int32_t) sizeof(FatFileEntry) * (lfn_entries - 1), fp->pos);

        //现在指向 dos entry,下面开始反向拿取 lfn entry
        for(i = 0; i < lfn_entries; i++) {
            tf_fseek(fp, (int32_t)-sizeof(FatFileEntry), fp->pos);
            tf_fread((char *)&entry, sizeof(FatFileEntry), fp);

            if(!tf_compare_filename_segment(&entry, compare_name)) {
                tf_fseek(fp, (int32_t)(i+1)*sizeof(FatFileEntry), fp->pos);
                return 0;
            }
            tf_fseek(fp, (int32_t)-sizeof(FatFileEntry), fp->pos);

            compare_name += 13;
        }
        tf_fseek(fp, (int32_t)sizeof(FatFileEntry) * lfn_entries, fp->pos);
        return 1;
    }
    return -1;
}

//在current_directory的内容中寻找与 name("/"前)匹配的entry,
//若匹配上, current_directory->pos 会指向匹配文件的dos entry, 返回0
//匹配不上则 pos 会跳到下一个entry(可能是 lfn/dos entry), 返回 -1
int tf_find_file(TFFile *current_directory, char *name) {
    int rc;
    tf_fseek(current_directory, 0, 0);

    while(1) {
        rc = tf_compare_filename(current_directory, name);
        if(rc < 0) {
            break;
        } else if(rc == 1) {   // found!
            return 0;
        }
    }
    return -1;
}

/*
 * Walks the path provided, returning a valid file pointer for each successive level in the path.
 *
 * example:  tf_walk("/home/ryan/myfile.txt", fp to "/")
 *           Call once: returns pointer to string: home/ryan/myfile.txt, fp now points to directory for /
 *          Call again: returns pointer to string: ryan/myfile.txt, fp now points to directory for /home
 *          Call again: returns pointer to string: myfile.txt, fp now points to directory for /home/ryan
 *          Call again: returns pointer to the end of the string, fp now points to /home/ryan/myfile.txt
 *          Call again: returns NULL pointer. fp is unchanged
 * ARGS
 *   filename - a string containing the full path
 *
 * SIDE EFFECTS
 *   The filesystem is traversed, so files are opened and closed, sectors are read, etc...
 * RETURN
 *   A string pointer to the next level in the path, or NULL if this is the end of the path
 */

char *tf_walk(char *filename, TFFile *fp) {
    FatFileEntry entry;

    // fang: don't deal with "/", because it's fixed, cluster 2,
    // fang: the first you call tf_walk(), fp must be handle of "/"
    if(*filename == '/') {
        filename++;
    }

    if(*filename != '\x00') {
        // fp is the handle for the current directory
        // filename is the name of the current file in that directory
        // Go fetch the FatFileEntry that corresponds to the current file
        // Remember that tf_find_file is only going to search from the beginning of the filename
        // up until the first path separation character,(fang: strncasecmp in tf_compare_filename_segment)
        if(tf_find_file(fp, filename)) {
            fp->flags = 0xff;   //fang: 告诉调用 tf_walk 的人,这个文件找不到,entry没有修改就ok,fp在内存中无所谓
            return NULL;
        }
        //当在目录(fp)下找到filename的时候,fp->pos已经在filename对应的那个entry处了
        tf_fread((char *)&entry, sizeof(FatFileEntry), fp);

        while((*filename != '/') && (*filename != '\x00'))
            filename += 1;
        if(*filename == '/')
            filename += 1;

        fp->parentStartCluster = fp->startCluster;
        fp->startCluster = ((uint32_t)(entry.msdos.eaIndex & 0xffff) << 16) | (entry.msdos.firstCluster & 0xffff);
        fp->attributes = entry.msdos.attributes;
        fp->currentCluster = fp->startCluster;
        fp->currentClusterIdx = 0;
        fp->currentSector = 0;
        fp->currentByte = 0;
        fp->pos = 0;
        fp->flags = TF_FLAG_OPEN;   //啥时候关闭呢??
        fp->size = (entry.msdos.attributes & TF_ATTR_DIRECTORY) ? 0xffffffff :entry.msdos.fileSize;
        if(*filename == '\x00')
            return NULL;    //fang : instead of return "", we return a NULL
        return filename;
    }
    return NULL;
}

//有一些接口是直接打开文件,返回一个指针,事先没有malloc的,所以这个指针指向的内容只能是
//内部的内存,所以这里准备了一些静态的 TFFiles
TFFile *tf_get_free_handle() {
    int i;
    TFFile *fp;
    for(i = 0; i < TF_FILE_HANDLES; i++) {
        fp = &tf_file_handles[i];
        if(fp->flags & TF_FLAG_OPEN)
            continue;
        fp->flags = TF_FLAG_OPEN;
        return fp;
    }
    return NULL;
}

void tf_release_handle(TFFile *fp) {
    fp->flags &= ~TF_FLAG_OPEN;
}

// Convert a character to uppercase
char upper(char c) {
    if(c >= 'a' && c <= 'z') {
        return c + ('A'-'a');
    } else {
        return c;
    }
}

/*
 * Take the long filename (filename only, not full path) specified by src,
 * and convert it to an 8.3 compatible filename, storing the result at dst
 * TODO: This should return something, an error code for conversion failure.
 * TODO: This should handle special chars etc.
 * TODO: Test for short filenames, (<7 characters)
 * TODO: Modify this to use the basis name generation algorithm described in the FAT32 whitepaper.
 */
int tf_shorten_filename(char *dest, char *src, uint8_t num) {
    int i;
    //int lossy_flag = 0;
    char *tmp;

    tmp = strrchr(src, '.');
    // copy the extension
    for (i = 0; i < 3; i++) {
        while (tmp != 0 && *tmp != 0 && !(*tmp < 0x7f && *tmp > 20
               && *tmp != 0x22
               && *tmp != 0x2a
               && *tmp != 0x2e
               && *tmp != 0x2f
               && *tmp != 0x3a
               && *tmp != 0x3c
               && *tmp != 0x3e
               && *tmp != 0x3f
               && *tmp != 0x5b
               && *tmp != 0x5c
               && *tmp != 0x5d
               && *tmp != 0x7c))
            tmp++;
        if (tmp == 0 || *tmp == 0)
            *(dest + 8 + i) = ' ';
        else
            *(dest + 8 + i) = upper(*(tmp++));
    }

    // Copy the basename
    i = 0;
    tmp = strrchr(src, '.');
    while(1) {
        if (i == 8)
            break;
        if (src == tmp) {
            dest[i++] = ' ';
            continue;
        }

        if((*dest == ' ')) {
            //lossy_flag = 1;
        } else {
            while (*src != 0 && !(*src < 0x7f && *src > 20
                   && *src != 0x22
                   && *src != 0x2a
                   && *src != 0x2e
                   && *src != 0x2f
                   && *src != 0x3a
                   && *src != 0x3c
                   && *src != 0x3e
                   && *src != 0x3f
                   && *src != 0x5b
                   && *src != 0x5c
                   && *src != 0x5d
                   && *src != 0x7c))
                src++;
            if (*src == 0)
                dest[i] = ' ';
            else if ( *src==',' || *src == '[' || *src == ']')
                dest[i] = '_';
            else
                dest[i] = upper(*(src++));
        }
        i += 1;
    }
    // now that they are populated, do analysis.
    // if num>4, do 2 letters
    if (num > 4) {
        snprintf(dest + 2, 6, "%.4X~", num);
        dest[7] = '1';
    } else {
        tmp = strchr(dest, ' ');
        if (tmp == 0 || tmp - dest > 6) {
            dest[6] = '~';
            dest[7] = num + 0x30;
        } else {
            *tmp++ = '~';
            *tmp++ = num + 0x30;
        }
    }
    return 0;
}

void tf_choose_sfn(char *dest, char *src, TFFile *fp) {
    int results, num = 1;
    TFFile xfile;
    // throwaway fp that doesn't muck with the original
    memcpy( &xfile, fp, sizeof(TFFile) );
    char temp[13];  //<8>.<3>\0, total=8+1+3+1

    while (1) {
        results = tf_shorten_filename( dest, src, num );
        switch (results) {
            case 0: // ok
                // does the file collide with the current directory?
                memcpy(temp, dest, 8);
                memcpy(temp + 9, dest + 8, 3);
                temp[8] = '.';
                temp[12] = 0;

                if (0 > tf_find_file( &xfile, temp )) {
                    return; //found found non-conflicting filename
                }
                //trying again with index: num
                num++;
                break;
            case -1: // error
                return;
        }
    }
}

//根据 filename 填充一个 lfn entry,若此 lfn entry 已经足够表达 filename,则返回 NULL
//否则会只填充一个 lfn entry, 对应 filename 的前13个字符,并将剩余的 filename 返回
//关于 lfn 中的第一个字节 SEQUENCE number,需要外部自己填充
char *tf_create_lfn_entry(char *filename, FatFileEntry *entry) {
    int i, done = 0;
    for(i = 0; i < 5; i++) {
        if (!done)
            entry->lfn.name1[i] = (unsigned short) *(filename);
        else
            entry->lfn.name1[i] = 0xffff;
        if(*filename++ == '\x00')
            done = 1;
    }
    for(i = 0; i < 6; i++) {
        if (!done)
            entry->lfn.name2[i] = (unsigned short) *(filename);
        else
            entry->lfn.name2[i] = 0xffff;
        if(*filename++ == '\x00')
            done = 1;
    }
    for(i = 0; i < 2; i++) {
        if (!done)
            entry->lfn.name3[i] = (unsigned short) *(filename);
        else
            entry->lfn.name3[i] = 0xffff;
        if(*filename++ == '\x00')
            done = 1;
    }
    entry->lfn.attributes = 0x0f;
    entry->lfn.reserved = 0;
    entry->lfn.firstCluster = 0;
    if(done)
        return NULL;
    if(*filename)
        return filename;
    else
        return NULL;
}

// Taken from http://en.wikipedia.org/wiki/File_Allocation_Table
uint8_t tf_lfn_checksum(const char *pFcbName)
{
    int i;
    uint8_t sum = 0;
    for (i = 11; i; i--)
        sum = ((sum & 1) << 7) + (sum >> 1) + *pFcbName++;
    return sum;
}

// fang: 生成filename对应的 entries, 并把他们放在当前fp所指向的位置
// (这些 entry 都在文件的dos entry之前,且是倒序放置的)
// and don't forget to modify FAT32 table to chain them.
int tf_place_lfn_chain(TFFile *fp, char *filename, char *sfn) {
    char *strptr = filename;
    int entries = 1;
    int i;
    char *last_strptr = filename;
    FatFileEntry entry;
    uint8_t seq;

    // 计算有多少 entry,同时将 last_strptr 指向 filename 的最后一部分
    while((strptr = tf_create_lfn_entry(strptr, &entry)) != NULL) {
        last_strptr = strptr;
        entries += 1;
    }

    // LFN sequence number (first byte of LFN)
    seq = entries | 0x40;
    for(i = 0; i < entries; i++) {
        tf_create_lfn_entry(last_strptr, &entry);
        entry.lfn.sequence_number = seq;
        entry.lfn.checksum = tf_lfn_checksum(sfn);

        //tf_fwrite 会做调整的吧,万一写入导致cluster不够,要有 "去fat表扩展cluster chain" 的动作
        tf_fwrite((char *)&entry, sizeof(FatFileEntry), 1, fp);
        seq = ((seq & ~0x40) - 1);
        last_strptr -= 13;
    }
    return 0;
}

//以可写方式打开父目录,并在其中seek到可以容纳 filename 的 dos entry + lfn entry 的地方
//记得释放 release fp
TFFile *find_free_for_entry_in_parent(char *filename, int mkParents) {
    FatFileEntry entry;
    int found_num = 0;
    char *temp = strrchr(filename, '/') + 1;
    int entries = (strlen(temp) + (5+6+2) - 1) / (5+6+2);
    TFFile *fp = tf_parent(filename, "r+", mkParents);
    if (!fp) {
        printf("%s 's parent not exist ?\r\n", filename);
        return NULL;
    }

    while (1) {
        tf_fread((char *)&entry, sizeof(FatFileEntry), fp);
        if (entry.msdos.filename[0] == '\0') {
            found_num++;
            tf_fseek(fp, (int32_t)(-found_num) * sizeof(FatFileEntry), fp->pos);
            return fp;
        } else if ((unsigned char)entry.msdos.filename[0] == 0xe5) {
            found_num++;
            if (found_num == entries) {
                tf_fseek(fp, (int32_t)(-found_num) * sizeof(FatFileEntry), fp->pos);
                return fp;
            }
        } else {
            found_num = 0;
        }
    }
    printf("not find a valid free space for entry\r\n");
    return NULL;
}

// fang: open file "filename[0:n)", filename is full path
TFFile *tf_fnopen(char *filename, const char *mode, int n) {
    // Request a new file handle from the system
    TFFile *fp = tf_get_free_handle();
    char myfile[256];
    char *temp_filename = myfile;
    uint32_t cluster;

    if (fp == NULL) {
        printf("tf_fnopen: out of handle?\r\n");
        return (TFFile*) -1;
    }

    strncpy(myfile, filename, n);
    myfile[n] = 0;

    // fang: fill in the fp for "/", so we search and walk from "/"
    fp->currentCluster = 2;
    fp->startCluster = 2;
    fp->parentStartCluster = 0xffffffff;
    fp->currentClusterIdx = 0;
    fp->currentSector = 0;
    fp->currentByte = 0;
    fp->attributes = TF_ATTR_DIRECTORY;
    fp->pos = 0;
    fp->flags |= TF_FLAG_ROOT;
    fp->size = 0xffffffff;
    //fp->size=tf_info.rootDirectorySize;
    fp->mode = TF_MODE_READ | TF_MODE_WRITE | TF_MODE_OVERWRITE;

    while(temp_filename != NULL) {
        temp_filename = tf_walk(temp_filename, fp);
        if(fp->flags == 0xff) {
            tf_release_handle(fp);
            return NULL;
        }
    }

    if(strchr(mode, 'r')) {
        fp->mode |= TF_MODE_READ;
    }
    if(strchr(mode, 'a')) {
        tf_unsafe_fseek(fp, fp->size, 0);
        fp->mode |= TF_MODE_WRITE | TF_MODE_OVERWRITE;
    }
    if(strchr(mode, '+'))
        fp->mode |= TF_MODE_OVERWRITE | TF_MODE_WRITE;
    if(strchr(mode, 'w')) {
        // Opened for writing. Truncate file only if it's not a directory
        if (!(fp->attributes & TF_ATTR_DIRECTORY)) {
            fp->size = 0;
            tf_unsafe_fseek(fp, 0, 0);
            if ((cluster = tf_get_fat_entry(fp->startCluster)) != TF_MARK_EOC32) {
                tf_free_clusterchain(cluster);
                tf_set_fat_entry(fp->startCluster, TF_MARK_EOC32);
            }
        }
        fp->mode |= TF_MODE_WRITE;
    }
    //填充 fp->filename
    strncpy(fp->filename, myfile, n);
    fp->filename[n] = 0;
    return fp;
}

/* fang: change fp->pos, fp->currentCluster, fp->currentClusterIdx, fp->currentByte...,
 * even fp->size(append mode)
 * TODO: Make it so seek fails aren't destructive to the file handle
 */
int tf_unsafe_fseek(TFFile *fp, int32_t base, long offset) {
    uint32_t cluster_idx;
    long pos = base + offset;
    uint32_t mark = tf_info.type ? TF_MARK_EOC32 : TF_MARK_EOC16;
    uint32_t temp;

    if(pos > fp->size) {    //fang:最多 seek 到 fp->size ?
        return TF_ERR_INVALID_SEEK;
    }
    if(pos == fp->size) {
        fp->size += 1;
        fp->flags |= TF_FLAG_SIZECHANGED;
    }

    //这个 cluster_idx 是 逻辑上在 file 中位于第几个 cluster
    cluster_idx = pos / (tf_info.sectorsPerCluster * 512); // The cluster we want in the file
    if(cluster_idx != fp->currentClusterIdx) {
        temp = cluster_idx;
        if (cluster_idx > fp->currentClusterIdx) {
            cluster_idx -= fp->currentClusterIdx;   //要是seek位置在当前在文件中的cluster之后的话,理论上向后稍微找几个就到了
        } else {
            fp->currentCluster = fp->startCluster;
        }
        fp->currentClusterIdx = temp;   //

        //下面以 fp->currentCluster 为起始,向后 seek cluster_idx 个 cluster
        while(cluster_idx > 0) {
            // TODO Check file mode here for r/w/a/etc...
            temp = tf_get_fat_entry(fp->currentCluster); // next, next, next
            if((temp & 0x0fffffff) != mark) {
                fp->currentCluster = temp;
            } else {
                temp = tf_find_free_cluster_from(fp->currentCluster);
                tf_set_fat_entry(fp->currentCluster, temp); // Allocates new space
                tf_set_fat_entry(temp, mark); // Marks the new cluster as the last one
                fp->currentCluster = temp;
            }
            cluster_idx--;
            if(fp->currentCluster >= mark) {
                if(cluster_idx > 0) {
                    return TF_ERR_INVALID_SEEK;
                }
            }
        }
        // We now have the correct cluster number (whether we had to fetch it from the fat, or realized we already had it)
        // Now we need just compute the correct sector and byte index into the cluster
    }
    fp->currentByte = pos % (tf_info.sectorsPerCluster * 512); // The offset into the cluster
    fp->pos = pos;
    return 0;
}


// fang: create a file, should give a full name, like "/home/fang/abc.txt"
// 根目录下创建文件应该修改 TFInfo.rootDirectorySize
// 普通文件的话也应该修改 size 吧,毕竟 entry 又多了
int tf_create(char *filename) {
    FatFileEntry entry;
    uint32_t cluster;
    char *temp = strrchr(filename, '/') + 1;

    TFFile *fp = tf_fnopen(filename, "r", strlen(filename));   //检查是否已经存在了
    if (fp) {
        tf_fclose(fp);
        return 0;
    }
    fp = tf_parent(filename, "r", false);   //检查一下父目录是否存在
    if(!fp) {
        return 1;
    }
    tf_fclose(fp);

    //fp = tf_parent(filename, "r+", false);          //开始向父目录写
    //do {    //先在父目录中找一个空闲的 entry
    //    tf_fread((char *)&entry, sizeof(FatFileEntry), fp);
    //} while(entry.msdos.filename[0] != '\x00');
    //tf_fseek(fp, (int32_t)-sizeof(FatFileEntry), fp->pos);
    fp = find_free_for_entry_in_parent(filename, false);

    cluster = tf_find_free_cluster();   // fang: this cluster will be the start cluster of the created file
    tf_set_fat_entry(cluster, TF_MARK_EOC32); // Marks the new cluster as the last one (but no longer free)

    entry.msdos.attributes = 0;
    entry.msdos.creationTimeMs = 0x25;//
    entry.msdos.creationTime = 0x7e3c;
    entry.msdos.creationDate = 0x4262;
    entry.msdos.lastAccessTime = 0x4262;
    entry.msdos.eaIndex = (cluster >> 16) & 0xffff;
    entry.msdos.modifiedTime = 0x7e3c;
    entry.msdos.modifiedDate = 0x4262;
    entry.msdos.firstCluster = cluster & 0xffff;
    entry.msdos.fileSize = 0;
    tf_choose_sfn(entry.msdos.filename, temp, fp);  //生成 sfn 并填充到 entry.msdos.filename

    //tf_fwrite 会做调整的吧,万一写入导致cluster不够,要有 "去fat表扩展cluster chain" 的动作
    tf_place_lfn_chain(fp, temp, entry.msdos.filename);         //生成lfn(以sfn作为校验)对应的entries,并写入到fp
    tf_fwrite((char *)&entry, sizeof(FatFileEntry), 1, fp);     //将填充好的dos entry写入到fp

    memset(&entry, 0, sizeof(FatFileEntry));        //继续写入一个空闲的 entry
    tf_fwrite((char *)&entry, sizeof(FatFileEntry), 1, fp);

    tf_fclose(fp);
    return 0;
}

/*
 * tf_mkdir attempts to create a new directory in the filesystem. 
 * duplicates are *not* allowed!
 * returns 1 on failure
 * returns 0 on success
 * fang: if mkParents > 0, this is like <mkdir -p xxx/xxx/xxx> and cannot have the final "/"
 */
int tf_mkdir(char *filename, int mkParents) {
    char orig_fn[TF_MAX_PATH];
    TFFile *fp;
    FatFileEntry entry, blank;

    uint32_t cluster;
    char *temp;

    strncpy(orig_fn, filename, TF_MAX_PATH - 1);
    orig_fn[TF_MAX_PATH - 1] = 0;

    memset(&blank, 0, sizeof(FatFileEntry));

    fp = tf_fopen(filename, "r");
    if (fp) { // if not NULL, the filename already exists.
        tf_fclose(fp);
        if (mkParents) {
            return 0;
        }
        return 1;   //不允许重名文件夹
    }

    fp = find_free_for_entry_in_parent(filename, mkParents);

    cluster = tf_find_free_cluster();
    tf_set_fat_entry(cluster, TF_MARK_EOC32); // Marks the new cluster as the last one (but no longer free)

    entry.msdos.attributes = TF_ATTR_DIRECTORY ;
    entry.msdos.creationTimeMs = 0x25;
    entry.msdos.creationTime = 0x7e3c;
    entry.msdos.creationDate = 0x4262;
    entry.msdos.lastAccessTime = 0x4262;
    entry.msdos.eaIndex = (cluster >> 16) & 0xffff;
    entry.msdos.modifiedTime = 0x7e3c;
    entry.msdos.modifiedDate = 0x4262;
    entry.msdos.firstCluster = cluster & 0xffff;
    entry.msdos.fileSize = 0;
    temp = strrchr(filename, '/') + 1;
    tf_choose_sfn(entry.msdos.filename, temp, fp);

    tf_place_lfn_chain(fp, temp, entry.msdos.filename);
    tf_fwrite((char *)&entry, sizeof(FatFileEntry), 1, fp);

    tf_fwrite((char *)&blank, sizeof(FatFileEntry), 1, fp);
    tf_fclose(fp);

    fp = tf_fopen(orig_fn, "w");

    memcpy( entry.msdos.filename, ".          ", 11 );  // set up .
    //entry.msdos.attributes = TF_ATTR_DIRECTORY;
    //entry.msdos.firstCluster = cluster & 0xffff
    tf_fwrite((char *)&entry, sizeof(FatFileEntry), 1, fp);

    memcpy( entry.msdos.filename, "..         ", 11 );  // set up ..
    //entry.msdos.attributes = TF_ATTR_DIRECTORY;
    //entry.msdos.firstCluster = cluster & 0xffff
    tf_fwrite((char *)&entry, sizeof(FatFileEntry), 1, fp);

    tf_fwrite((char *)&blank, sizeof(FatFileEntry), 1, fp);
    tf_fclose(fp);
    return 0;
}

//dir 结尾不要带/
//return -1: you should stop
//return 1: a ready file info
//return 0: you can continue
int tf_listdir(char *dir, TFFileInfo *finfo) {
    int i, j, t;
    FatFileEntry entry;
    if (finfo->fp == NULL) {
        finfo->fp = tf_fnopen(dir, "r", strlen(dir));
        if (finfo->fp == NULL) {
            return -1;
        }
    }

    tf_fread((char *)&entry, sizeof(FatFileEntry), finfo->fp);
    //print_FatFileEntry(&entry);

    switch (((uint8_t *)&entry)[0]){
        case 0x05:
            // pending delete files under some implementations.  ignore it.
            return 0;
        case 0xe5:
            // freespace (deleted file)
            return 0;
        case 0x2e:
            // '.' or '..'
            return 0;
        case 0x00:
            // no further entries exist, and this one is available
            tf_fclose(finfo->fp);
            finfo->fp = NULL;
            return -1;
        default:
            if(entry.msdos.attributes != 0x0f) {    // it's a DOS entry
                finfo->size = entry.msdos.fileSize;
                finfo->fdate = entry.msdos.modifiedDate;
                finfo->ftime = entry.msdos.modifiedTime;
                finfo->attributes = entry.msdos.attributes;
                //access lfn
                i = 0;  //往回access了多少个entry
                j = 0;  //
                memset(finfo->filename, 0, TF_MAX_PATH);
                while (1) {
                    //
                    tf_fseek(finfo->fp, (int32_t)-(2 * sizeof(FatFileEntry)), finfo->fp->pos);
                    tf_fread((char *)&entry, sizeof(FatFileEntry), finfo->fp);
                    i++;
                    if(entry.msdos.attributes != 0x0f) {
                        break;
                    } else {
                        for(t = 0; t < 5; t++) {
                            if (entry.lfn.name1[t] == 0xffff) {
                                break;
                            }
                            finfo->filename[j++] = entry.lfn.name1[t];
                        }
                        for(t = 0; t < 6; t++) {
                            if (entry.lfn.name2[t] == 0xffff) {
                                break;
                            }
                            finfo->filename[j++] = entry.lfn.name2[t];
                        }
                        for(t = 0; t < 2; t++) {
                            if (entry.lfn.name3[t] == 0xffff) {
                                break;
                            }
                            finfo->filename[j++] = entry.lfn.name3[t];
                        }
                        if ((entry.lfn.sequence_number & 0xc0) == 0x40) {
                            break;
                        }
                    }
                }
                tf_fseek(finfo->fp, (int32_t)i * sizeof(FatFileEntry), finfo->fp->pos);
                return 1;
            }
            return 0;
    }
    return -1;
}

TFFile *tf_fopen(char *filename, const char *mode) {
    TFFile *fp;

    fp = tf_fnopen(filename, mode, strlen(filename));
    if(fp == NULL) {
        if(strchr(mode, '+') || strchr(mode, 'w') || strchr(mode, 'a')) {
            tf_create(filename); 
        }
        return tf_fnopen(filename, mode, strlen(filename));
    }
    return fp;
}

//fang: return 0 if seek is success
int tf_fseek(TFFile *fp, int32_t base, long offset) {
    long pos = base + offset;
    if (pos >= fp->size)
        return TF_ERR_INVALID_SEEK;
    return tf_unsafe_fseek(fp, base, offset); 
}

// fang: here should return bytes num we actually read.
// and here can refine to be more effective.(512 bytes once)
int tf_fread(char *dest, int size, TFFile *fp) {
    uint32_t sector;
    int i = 0;
    for (i = 0; i < size && fp->pos < fp->size; i++) {
        sector = tf_first_sector(fp->currentCluster) + (fp->currentByte / 512);
        tf_fetch(sector);

        dest[i] = tf_info.buffer[fp->currentByte % 512];    // fang: read a byte
        if(fp->attributes & TF_ATTR_DIRECTORY) {    //warning: reading a directory
            if(tf_fseek(fp, 0, fp->pos + 1)) {
                break;
            }
        } else {
            if(tf_fseek(fp, 0, fp->pos + 1)) {  // fang: then seek to next byte
                break;
            }
        }
    }
    return i;
}

//每次写入 size 字节, 重复 count 次
//每次读写的最小单位就是扇区
int tf_fwrite(char *src, int size, int count, TFFile *fp) {
    int res, tracking, segsize;
    fp->flags |= TF_FLAG_DIRTY;
    while(count > 0) {
        res = size;
        fp->flags |= TF_FLAG_SIZECHANGED;   //指示这个文件对应的entry需要修改
        while(res > 0) {
            //fetch 当前扇区
            tf_fetch(tf_first_sector(fp->currentCluster) + (fp->currentByte / 512));
            //当前 pos 不足一扇区的残余, currentByte 应该是当前待写入的位置
            tracking = fp->currentByte % 512;
            //当前要处理的数据量
            segsize = (res > (512 - tracking)) ? (512 - tracking) : res;
            
            memcpy(&tf_info.buffer[tracking], src, segsize);
            tf_info.sectorFlags |= TF_FLAG_DIRTY; // Mark this sector as dirty, fang: next time we fetch or read, or flush, write_sector will be called

            if (fp->pos + segsize > fp->size) {
                fp->size += segsize;  //这里不修改 size 下面 tf_unsafe_fseek 就会失败
            }
            if(tf_unsafe_fseek(fp, 0, fp->pos + segsize)) {
                return -1;
            }
            res -= segsize;
            src += segsize;
        }
        count--;
    }
    return size - res;
}

int tf_fputs(char *src, TFFile *fp) {
    return tf_fwrite(src, strlen(src), 1, fp);
}

int tf_fclose(TFFile *fp) {
    int rc;
    
    rc =  tf_fflush(fp);
    fp->flags &= ~TF_FLAG_OPEN; // Mark the file as available for the system to use
    // FIXME: is there any reason not to release the handle here?
    tf_release_handle(fp);
    return rc;
}

/* tf_parent attempts to open the parent directory of whatever file you request
 * returns basically a fp the tf_fnopen returns
 * fang: try to find the directory who contains the file's entry ?
 * give "/home/fang/abc.txt", return a TFFile pointer, to (/home/fang/)
*/
TFFile *tf_parent(char *filename, const char *mode, int mkParents) {
    TFFile *retval;
    char *f2;
    f2 = (char *)strrchr((char const*)filename, '/');
    retval = tf_fnopen(filename, "rw", (int)(f2 - filename));
    if (retval == NULL && mkParents) {
        char tmpbuf[260];
        if (f2 - filename > 260) {
            return NULL;
        }
        strncpy(tmpbuf, filename, f2 - filename);
        tmpbuf[f2 - filename] = 0;
        tf_mkdir(tmpbuf, mkParents);
        retval = tf_parent(filename, mode, mkParents);
    } else if (retval == (void*) -1) {
        return NULL;
    }
    return retval;
}

int tf_fflush(TFFile *fp) {
    int rc = 0;
    TFFile *dir;
    FatFileEntry entry;
    char *filename = entry.msdos.filename;

    if(!(fp->flags & TF_FLAG_DIRTY))
        return 0;

    // First write any pending data to disk
    if(tf_info.sectorFlags & TF_FLAG_DIRTY) {
        rc = tf_store();
    }
    // Now go modify the directory entry for this file to reflect changes in the file's size
    // (If they occurred)
    if(fp->flags & TF_FLAG_SIZECHANGED) {

        if(fp->attributes & 0x10) {
            // TODO Deal with changes in the root directory size here
        } else {
            // Open the parent directory
            dir = tf_parent(fp->filename, "r+", false);
            if (dir == (void *)-1) {
                return -1;
            }
            filename = (char *)strrchr((char const*)fp->filename, '/');

            // Seek to the entry we want to modify and pull it from disk
            tf_find_file(dir, filename + 1);
            tf_fread((char *)&entry, sizeof(FatFileEntry), dir);
            tf_fseek(dir, (int32_t)-sizeof(FatFileEntry), dir->pos);

            // Modify the entry in place to reflect the new file size
            entry.msdos.fileSize = fp->size - 1;
            tf_fwrite((char *)&entry, sizeof(FatFileEntry), 1, dir); // Write fatfile entry back to disk
            tf_fclose(dir);
        }
        fp->flags &= ~TF_FLAG_SIZECHANGED;
    }
    fp->flags &= ~TF_FLAG_DIRTY;
    return rc;
}

//param filename - The full path of the file to be removed
//memmove,从其父目录中将这个文件对应的 entrys 删除
//从fat表中将其占用的  cluster chain 释放
int tf_remove(char *filename) {
    TFFile *fp;
    FatFileEntry entry;
    FatFileEntry blank;
    int rc;
    uint32_t startCluster;
    int del_entry_num = 0;
    int i = 0;

    memset(&blank, 0, sizeof(FatFileEntry));

    //fp = tf_fopen(filename, "r");
    fp = tf_fnopen(filename, "r", strlen(filename));
    if(fp == NULL) {
        printf("you are removing a file not exist:%s\r\n", filename);
        return -1;
    }
    startCluster = fp->startCluster; // Remember first cluster of the file so we can remove the clusterchain
    tf_fclose(fp);  // flush

    fp = tf_parent(filename, "r+", false);
    rc = tf_find_file(fp, (strrchr((char *)filename, '/') + 1));    //匹配上了后,pos会指向 dos entry
    if(rc == 0) {
        del_entry_num = 1;
        while (1) {
            tf_fseek(fp, (int32_t)-sizeof(FatFileEntry), fp->pos);
            tf_fread((char *)&entry, sizeof(FatFileEntry), fp);
            tf_fseek(fp, (int32_t)-sizeof(FatFileEntry), fp->pos);
            if (entry.lfn.attributes == 0x0f) {
                del_entry_num++;
                if (entry.lfn.sequence_number & 0x40) {
                    break;
                }
            } else {
                break;
            }
        }
        while (1) {
            tf_fseek(fp, (int32_t) del_entry_num * sizeof(FatFileEntry), fp->pos);
            tf_fread((char *)&entry, sizeof(FatFileEntry), fp);
            tf_fseek(fp, (int32_t) -(del_entry_num + 1) * sizeof(FatFileEntry), fp->pos);
            tf_fwrite((char *)&entry, sizeof(FatFileEntry), 1, fp);
            if (entry.msdos.filename[0] == '\0') {
                for (i = 0; i < del_entry_num - 1; i++) {
                    tf_fwrite((char *)&blank, sizeof(FatFileEntry), 1, fp);
                }
                break;
            }
        }
        fp->size -= del_entry_num * sizeof(FatFileEntry);
        fp->flags |= TF_FLAG_SIZECHANGED; 
    }
    tf_fclose(fp);
    tf_free_clusterchain(startCluster); // Free the data associated with the file
    return 0;
}

/* Initialize the FileSystem metadata on the media (yes, the "FORMAT" command 
    that Windows doesn't allow for large volumes */
// this should take in some lun number to make this all good...
//but we'll do that when we get read/write_sector lun-aware.
// also, hardcoded sector configuration
uint32_t tf_initializeMedia(uint32_t totalSectors)
{
    char sectorBuf0[512];
    char sectorBuf[512];
    BPB_struct bpb; // = (BPB_struct*)sectorBuf0;
    uint32_t scl, ssa, fat;

    memset(sectorBuf0, 0x00, 0x200);
    memset(&bpb, 0, sizeof(bpb));

    //--- jump instruction
    bpb.BS_JumpBoot[0] = 0xEB;
    bpb.BS_JumpBoot[1] = 0x58;
    bpb.BS_JumpBoot[2] = 0x90;

    //--- OEM name
    memcpy( bpb.BS_OEMName, " mkdosfs", 8);

    //--- BPB
    bpb.BytesPerSector = 0x200;        // hard coded, must be a define somewhere
    bpb.SectorsPerCluster = 32;        // this may change based on drive size
    bpb.ReservedSectorCount = 32;
    bpb.NumFATs = 2;
    //bpb.RootEntryCount = 0;
    //bpb.TotalSectors16 = 0;
    bpb.Media = 0xf8;
    //bpb.FATSize16 = 0;
    bpb.SectorsPerTrack = 32;          // unknown here
    bpb.NumberOfHeads = 64;            // ?
    //bpb.HiddenSectors = 0;
    bpb.TotalSectors32 = totalSectors;
    //--- BPB-FAT32 Extension
    bpb.FSTypeSpecificData.fat32.FATSize = totalSectors / 4095;
    //bpb.FSTypeSpecificData.fat32.ExtFlags = 0;
    //bpb.FSTypeSpecificData.fat32.FSVersion = 0;
    bpb.FSTypeSpecificData.fat32.RootCluster = 2;
    bpb.FSTypeSpecificData.fat32.FSInfo = 1;
    bpb.FSTypeSpecificData.fat32.BkBootSec = 6;
    //memset( bpb.FSTypeSpecificData.fat32.Reserved, 0x00, 12 );
    //bpb.FSTypeSpecificData.fat32.BS_DriveNumber = 0;
    //bpb.FSTypeSpecificData.fat32.BS_Reserved1 = 0;
    bpb.FSTypeSpecificData.fat32.BS_BootSig = 0x29;
    bpb.FSTypeSpecificData.fat32.BS_VolumeID = 0xf358ddc1;      // hardcoded volume id.  this is weird.  should be generated each time.
    memset( bpb.FSTypeSpecificData.fat32.BS_VolumeLabel, 0x20, 11);
    memcpy( bpb.FSTypeSpecificData.fat32.BS_FileSystemType, "FAT32   ", 8); 
    memcpy( sectorBuf0, &bpb, sizeof(bpb) );
    
    memcpy( sectorBuf0+0x5a, "\x0e\x1f\xbe\x77\x7c\xac\x22\xc0\x74\x0b\x56\xb4\x0e\xbb\x07\x00\xcd\x10\x5e\xeb\xf0\x32\xe4\xcd\x17\xcd\x19\xeb\xfe\
        This is not a bootable disk.  Please insert a bootable floppy and\r\npress any key to try again ... \r\n", 129 );

    fat = (bpb.ReservedSectorCount);
    
    //--- ending signatures
    sectorBuf0[0x1fe] = 0x55;
    sectorBuf0[0x1ff] = 0xAA;
    write_sector( sectorBuf0, 0 );

    //sector number: start of root
    ssa = (bpb.NumFATs * bpb.FSTypeSpecificData.fat32.FATSize) + fat;

    //--- FSInfo sector
    memset(sectorBuf, 0x00, 0x200);
    *((uint32_t*)sectorBuf)         = 0x41615252;
    *((uint32_t*)(sectorBuf+0x1e4))   = 0x61417272;
    *((uint32_t*)(sectorBuf+0x1e8))   = 0xffffffff; // last known number of free data clusters on volume
    *((uint32_t*)(sectorBuf+0x1ec))   = 0xffffffff; // number of most recently known to be allocated cluster
    *((uint32_t*)(sectorBuf+0x1f0))   = 0;  // reserved
    *((uint32_t*)(sectorBuf+0x1f4))   = 0;  // reserved
    *((uint32_t*)(sectorBuf+0x1f8))   = 0;  // reserved
    *((uint32_t*)(sectorBuf+0x1fc))   = 0xaa550000;
    write_sector( sectorBuf, 1 );

    memset(sectorBuf, 0x00, 0x200);
    for (scl = 2 ; scl < bpb.SectorsPerCluster; scl++) {
        memset(sectorBuf, 0x00, 0x200);
        write_sector( sectorBuf, scl );
    }
    //--- write backup copy of metadata
    write_sector( sectorBuf0, 6 );

    //--- make Root Directory
    memset(sectorBuf, 0x00, 0x200);     // 0x00000000 is the unallocated marker
    for (scl = ssa; scl < ssa + bpb.SectorsPerCluster; scl++) {
        write_sector( sectorBuf, scl );
    }

    //fang:?
    memset(sectorBuf, 0x00, 0x200);     // 0x00000000 is the unallocated marker
    for (scl = fat; scl < ssa / 2; scl++) {
        write_sector( sectorBuf, scl );
        write_sector( sectorBuf, scl + (ssa / 2));
    }

    //SSA = RSC + FN x SF + ceil((32 x RDE)/SS)  and LSN = SSA + (CN-2) x SC

    *((uint32_t*)(sectorBuf+0x000))   = 0x0ffffff8;   // special - EOF marker
    *((uint32_t*)(sectorBuf+0x004))   = 0x0fffffff;   // special and clean
    *((uint32_t*)(sectorBuf+0x008))   = 0x0ffffff8;   // root directory (one cluster)
    write_sector( sectorBuf, bpb.SectorsPerCluster );

    return 0;
}

uint32_t tf_initializeMediaNoBlock(uint32_t totalSectors, int start)
{
    char sectorBuf0[512];
    char sectorBuf[512];
    BPB_struct bpb; // = (BPB_struct*)sectorBuf0;
    uint32_t fat;
    static uint32_t scl, ssa, sectors_per_cluster;

    if( start )
    {
        memset(sectorBuf0, 0x00, 0x200);
        memset(&bpb, 0, sizeof(bpb));

            // jump instruction
        bpb.BS_JumpBoot[0] = 0xEB;
        bpb.BS_JumpBoot[1] = 0x58;
        bpb.BS_JumpBoot[2] = 0x90;

            // OEM name
        memcpy( bpb.BS_OEMName, " mkdosfs", 8);

            // BPB
        bpb.BytesPerSector = 0x200;        // hard coded, must be a define somewhere
        bpb.SectorsPerCluster = 32;        // this may change based on drive size
        sectors_per_cluster = 32;
        bpb.ReservedSectorCount = 32;
        bpb.NumFATs = 2;
        //bpb.RootEntryCount = 0;
        //bpb.TotalSectors16 = 0;
        bpb.Media = 0xf8;
        //bpb.FATSize16 = 0;
        bpb.SectorsPerTrack = 32;          // unknown here
        bpb.NumberOfHeads = 64;            // ?
        //bpb.HiddenSectors = 0;
        bpb.TotalSectors32 = totalSectors;
            // BPB-FAT32 Extension
        bpb.FSTypeSpecificData.fat32.FATSize = totalSectors / 4095;
        //bpb.FSTypeSpecificData.fat32.ExtFlags = 0;
        //bpb.FSTypeSpecificData.fat32.FSVersion = 0;
        bpb.FSTypeSpecificData.fat32.RootCluster = 2;
        bpb.FSTypeSpecificData.fat32.FSInfo = 1;
        bpb.FSTypeSpecificData.fat32.BkBootSec = 6;
        //memset( bpb.FSTypeSpecificData.fat32.Reserved, 0x00, 12 );
        //bpb.FSTypeSpecificData.fat32.BS_DriveNumber = 0;
        //bpb.FSTypeSpecificData.fat32.BS_Reserved1 = 0;
        bpb.FSTypeSpecificData.fat32.BS_BootSig = 0x29;
        bpb.FSTypeSpecificData.fat32.BS_VolumeID = 0xf358ddc1;      // hardcoded volume id.  this is weird.  should be generated each time.
        memset( bpb.FSTypeSpecificData.fat32.BS_VolumeLabel, 0x20, 11);
        memcpy( bpb.FSTypeSpecificData.fat32.BS_FileSystemType, "FAT32   ", 8); 
        memcpy( sectorBuf0, &bpb, sizeof(bpb) );

        memcpy( sectorBuf0+0x5a, "\x0e\x1f\xbe\x77\x7c\xac\x22\xc0\x74\x0b\x56\xb4\x0e\xbb\x07\x00\xcd\x10\x5e\xeb\xf0\x32\xe4\xcd\x17\xcd\x19\xeb\xfeThis is not a bootable disk.  Please insert a bootable floppy and\r\npress any key to try again ... \r\n", 129 );

        fat = (bpb.ReservedSectorCount);

            // ending signatures
        sectorBuf0[0x1fe] = 0x55;
        sectorBuf0[0x1ff] = 0xAA;
        write_sector( sectorBuf0, 0 );

        ssa = (bpb.NumFATs * bpb.FSTypeSpecificData.fat32.FATSize) + fat;

            // FSInfo sector
        memset(sectorBuf, 0x00, 0x200);
        *((uint32_t*)sectorBuf)         = 0x41615252;
        *((uint32_t*)(sectorBuf+0x1e4))   = 0x61417272;
        *((uint32_t*)(sectorBuf+0x1e8))   = 0xffffffff; // last known number of free data clusters on volume
        *((uint32_t*)(sectorBuf+0x1ec))   = 0xffffffff; // number of most recently known to be allocated cluster
        *((uint32_t*)(sectorBuf+0x1f0))   = 0;  // reserved
        *((uint32_t*)(sectorBuf+0x1f4))   = 0;  // reserved
        *((uint32_t*)(sectorBuf+0x1f8))   = 0;  // reserved
        *((uint32_t*)(sectorBuf+0x1fc))   = 0xaa550000;
        write_sector( sectorBuf, 1 );
        fat = (bpb.ReservedSectorCount);

        memset(sectorBuf, 0x00, 0x200);
        for (scl=2 ; scl<sectors_per_cluster ; scl++) {
            memset(sectorBuf, 0x00, 0x200);
            write_sector( sectorBuf, scl );
        }
            // write backup copy of metadata
        write_sector( sectorBuf0, 6 );



            // make Root Directory 

        // whack ROOT directory file: SSA = RSC + FN x SF + ceil((32 x RDE)/SS)  and LSN = SSA + (CN-2) x SC
        // this clears the first cluster of the root directory
        memset(sectorBuf, 0x00, 0x200);     // 0x00000000 is the unallocated marker
        for (scl=ssa+bpb.SectorsPerCluster; scl>=ssa; scl--) {
            write_sector( sectorBuf, scl );
        }

        scl = fat;
        return true;
    } else {
        uint32_t stop = scl+100;
        if( stop >= (ssa/2) ) stop = ssa/2;
        memset(sectorBuf, 0x00, 0x200);     // 0x00000000 is the unallocated marker
        for (; scl<stop; scl++) {
            write_sector( sectorBuf, scl );
            write_sector( sectorBuf, scl+(ssa/2) );
        }
        if( scl < ssa/2 )
            return false; 

        //SSA = RSC + FN x SF + ceil((32 x RDE)/SS)  and LSN = SSA + (CN-2) x SC
        
        *((uint32_t*)(sectorBuf+0x000))   = 0x0ffffff8;   // special - EOF marker
        *((uint32_t*)(sectorBuf+0x004))   = 0x0fffffff;   // special and clean
        *((uint32_t*)(sectorBuf+0x008))   = 0x0ffffff8;   // root directory (one cluster)
        write_sector( sectorBuf, sectors_per_cluster );
    }
    return true;
}


//-------------------------------------------------------------------------------------------------
int fat_type(BPB_struct *s) {
    return TYPE_FAT32;
}

void print_media(unsigned char mediatype) {
    switch(mediatype) {
        case 0xf0:
            printf("Generic media type (0xf0)");
            break;
        case 0xf8:
            printf("Hard disk (0xf8)");
            break;
        case 0xf9:
            printf("3.5\" double sided 720k or 5.25 double sided 1.2MB (0xf9)");
            break;
        case 0xfa:
            printf("5.25\" single sided 320k (0xfa)");
            break;
        case 0xfb:
            printf("3.5\" double sided 640k (0xfb)");
            break;
        case 0xfc:
            printf("5.25\" single sided 180k (0xfc)");
            break;
        case 0xfd:
            printf("5.25\" double sided 360k (0xfd)");
            break;
        case 0xfe:
            printf("5.25\" single sided 180k (0xfe)");
            break;
        case 0xff:
            printf("5.25\" double sided 320k (0xff)");
            break;
        default:
            printf("Unknown (0x%02x)",mediatype);
            break;
    }
}

void printBPB(BPB_struct *s) {
    int i,j;

    printf("==== BIOS PARAMETER BLOCK ====\n");
    printf("   Boot Instruction: 0x%02x%02x%02x\n", s->BS_JumpBoot[0], s->BS_JumpBoot[1], s->BS_JumpBoot[2]);
    printf("           OEM Name: '%8s'\n", s->BS_OEMName);
    printf("   Bytes per Sector: %d\n", s->BytesPerSector);
    printf("Sectors per Cluster: %d\n", s->SectorsPerCluster);
    printf("   Reserved Sectors: %d\n", s->ReservedSectorCount);
    printf("               FATS: %d\n", s->NumFATs);
    printf("       Root Entries: %d\n", s->RootEntryCount);
    printf("              Media: "); print_media(s->Media); printf("\n");
    printf("     Fat Size (16b): %d\n", s->FATSize16);
    printf("  Sectors per Track: %d\n", s->SectorsPerTrack);
    printf("    Number of Heads: %d\n", s->NumberOfHeads);
    printf("     Hidden Sectors: %d\n", s->HiddenSectors);
    printf("Total Sectors (16b): %d\n", s->TotalSectors16);
    printf("Total Sectors (32b): %d\n", s->TotalSectors32);
    switch(fat_type(s)) {
        case TYPE_FAT12:
            printf("           FAT Type: FAT12\n"); break;
        case TYPE_FAT16:
            printf("           FAT Type: FAT16\n"); break;
        case TYPE_FAT32:
            printf("           FAT Type: FAT32\n"); break;
        default:
            printf("FAT TYPE UNRECOGNIZED!\n"); break;
    }
    if((fat_type(s) == TYPE_FAT12) | (fat_type(s) == TYPE_FAT16)) {
        printf("       Drive Number: %d\n", s->FSTypeSpecificData.fat16.BS_DriveNumber);
        printf("     Boot Signature: 0x%02x\n", s->FSTypeSpecificData.fat16.BS_BootSig);
        printf("          Volume ID: 0x%04x\n", s->FSTypeSpecificData.fat16.BS_VolumeID);
        printf("       Volume Label: '%.*s'\n", 11, s->FSTypeSpecificData.fat16.BS_VolumeLabel);
        printf("     FileSystemType: '%.*s'\n", 8, s->FSTypeSpecificData.fat16.BS_FileSystemType);
    } else {
        printf("     FAT Size (32b): %d\n", s->FSTypeSpecificData.fat32.FATSize);
        printf("          FAT Flags: 0x%04x\n", s->FSTypeSpecificData.fat32.ExtFlags);
        printf("         FS Version: 0x%04x\n", s->FSTypeSpecificData.fat32.FSVersion);
        printf("       Root Cluster: %d\n", s->FSTypeSpecificData.fat32.RootCluster);
        printf("      FSINFO Sector: %d\n", s->FSTypeSpecificData.fat32.FSInfo);
        printf("Bkup BootRec Sector: %d\n", s->FSTypeSpecificData.fat32.BkBootSec);
        j = 0;
        for(i=0; i<12; i++) {
            if(s->FSTypeSpecificData.fat32.Reserved[i] != 0x00) j=1;
        }
        if(j)
            printf("      Reserved Area: NONZERO!\n");
        else
            printf("      Reserved Area: Ok\n");
        printf("       Drive Number: %d\n", s->FSTypeSpecificData.fat32.BS_DriveNumber);
        printf("     Boot Signature: 0x%02x\n", s->FSTypeSpecificData.fat32.BS_BootSig);
        printf("          Volume ID: 0x%04x\n", s->FSTypeSpecificData.fat32.BS_VolumeID);
        printf("       Volume Label: '%.*s'\n", 11, s->FSTypeSpecificData.fat32.BS_VolumeLabel);
        printf("     FileSystemType: '%.*s'\n", 8, s->FSTypeSpecificData.fat32.BS_FileSystemType);
    }

    printf("\n");
}

void print_sector(unsigned char *sector) {
    int i;

    for(i = 0; i < (512-8); i += 8) {
        printf(" %02x%02x%02x%02x %02x%02x%02x%02x        %c%c%c%c %c%c%c%c\n",
        sector[i], sector[i+1], sector[i+2], sector[i+3], sector[i+4], sector[i+5], sector[i+6], sector[i+7],
        sector[i], sector[i+1], sector[i+2], sector[i+3], sector[i+4], sector[i+5], sector[i+6], sector[i+7]);
    }   
}

void print_tf_info(TFInfo *t) {
    printf("    TFInfo Structure\n    ----------------\n");
    switch(t->type) {
        case TF_TYPE_FAT16:
            printf("               Type: FAT16\n");
            break;
        case TF_TYPE_FAT32:
            printf("               Type: FAT32\n");
            break;
        default:
            printf("               Type: UNRECOGNIZED! (0x%02x)\n", t->type);
            break;
    }
    printf("Sectors Per Cluster: %d\n", t->sectorsPerCluster);
    printf("      Total Sectors: %d\n", t->totalSectors);
    printf("   Reserved Sectors: %d\n", t->reservedSectors);
    printf("  First Data Sector: %d\n", t->firstDataSector);
}

void print_TFFile(TFFile *fp) {
    printf("     TFFile Structure\n    ----------------\n");
    printf("    currentCluster: %d\n", fp->currentCluster);
    printf(" currentClusterIdx: %d\n", fp->currentClusterIdx);
    printf("parentStartCluster: %d\n", fp->parentStartCluster);
    printf("      startCluster: %d\n", fp->startCluster);
    printf("     currentSector: %d\n", fp->currentSector);
    printf("       currentByte: %d\n", fp->currentByte);
    printf("               pos: %d\n", fp->pos);
    printf("             flags: 0x%02x\n", fp->flags);
    printf("              size: %d bytes\n", fp->size);
}

void print_FatFile83(FatFile83 *entry) {
    printf("         Type: 8.3 Filename\n");
    printf("     Filename: %.*s\n", 8, entry->filename);
    printf("    Extension: %.*s\n", 3,  entry->extension);
    printf("   Attributes: 0x%02x\n", entry->attributes);
    printf("First Cluster: %d\n", ((entry->eaIndex & 0xffff) << 16) | ((entry->firstCluster & 0xffff)));
    printf("Creation Time: %d/%d/%d\n", ((entry->creationDate & 0xfe00) >> 9) + 1980, ((entry->creationDate & 0x1e0) >> 5), (entry->creationDate & 0xf)*2);
}

void print_FatFileLFN(FatFileLFN *entry) {
    printf("         Type: Long Filename (LFN)\n");
}


void print_FatFileEntry(FatFileEntry *entry) {
    printf("    FatFile Structure\n    ---------------\n");
    if(entry->msdos.attributes == 0x0f) {
        print_FatFileLFN(&(entry->lfn));
    } else {
        print_FatFile83(&(entry->msdos));
    }
}
