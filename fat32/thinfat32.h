#ifndef __THIN_FAT32_H
#define __THIN_FAT32_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#define false 0
#define true 1

#pragma pack(push, 1)

// Starting at offset 36 into the BPB, this is the structure for a FAT12/16 FS
typedef struct struct_BPBFAT1216_struct {
    uint8_t     BS_DriveNumber;           // 1
    uint8_t     BS_Reserved1;             // 1
    uint8_t     BS_BootSig;               // 1
    uint32_t          BS_VolumeID;              // 4
    uint8_t     BS_VolumeLabel[11];       // 11
    uint8_t     BS_FileSystemType[8];     // 8
} BPB1216_struct;

// Starting at offset 36 into the BPB, this is the structure for a FAT32 FS
typedef struct struct_BPBFAT32_struct {
    uint32_t          FATSize;             // 4
    uint16_t    ExtFlags;              // 2
    uint16_t    FSVersion;             // 2
    uint32_t          RootCluster;           // 4
    uint16_t    FSInfo;                // 2
    uint16_t    BkBootSec;             // 2
    uint8_t     Reserved[12];          // 12
    uint8_t     BS_DriveNumber;            // 1
    uint8_t     BS_Reserved1;              // 1
    uint8_t     BS_BootSig;                // 1
    uint32_t          BS_VolumeID;               // 4
    uint8_t     BS_VolumeLabel[11];        // 11
    uint8_t     BS_FileSystemType[8];      // 8
} BPB32_struct;

typedef struct struct_BPB_struct {
    uint8_t     BS_JumpBoot[3];            // 3
    uint8_t     BS_OEMName[8];             // 8
    uint16_t    BytesPerSector;        // 2
    uint8_t     SectorsPerCluster;     // 1
    uint16_t    ReservedSectorCount;   // 2
    uint8_t     NumFATs;               // 1
    uint16_t    RootEntryCount;        // 2
    uint16_t    TotalSectors16;        // 2
    uint8_t     Media;                 // 1
    uint16_t    FATSize16;             // 2
    uint16_t    SectorsPerTrack;       // 2
    uint16_t    NumberOfHeads;         // 2
    uint32_t          HiddenSectors;         // 4
    uint32_t          TotalSectors32;        // 4
    union {
        BPB1216_struct fat16;
        BPB32_struct fat32;
    } FSTypeSpecificData;
} BPB_struct;

typedef struct struct_FatFile83 {
    char filename[8];
    char extension[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creationTimeMs;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t lastAccessTime;
    uint16_t eaIndex;
    uint16_t modifiedTime;
    uint16_t modifiedDate;
    uint16_t firstCluster;
    uint32_t fileSize;
} FatFile83;

typedef struct struct_FatFileLFN {
    uint8_t sequence_number;
    uint16_t name1[5];      // 5 Chars of name (UTF 16???)
    uint8_t attributes;     // Always 0x0f
    uint8_t reserved;       // Always 0x00
    uint8_t checksum;       // Checksum of DOS Filename.  See Docs.
    uint16_t name2[6];      // 6 More chars of name (UTF-16)
        uint16_t firstCluster;  // Always 0x0000
    uint16_t name3[2];
} FatFileLFN;

typedef union struct_FatFileEntry {
    FatFile83 msdos;
    FatFileLFN lfn; //fang: lfn should be short cut for "long file name"
} FatFileEntry;

#pragma pack(pop)



// 最长文件名
#define TF_MAX_PATH 256
#define TF_FILE_HANDLES 5

#define TF_FLAG_DIRTY 0x01
#define TF_FLAG_OPEN 0x02
#define TF_FLAG_SIZECHANGED 0x04
#define TF_FLAG_ROOT 0x08

#define TYPE_FAT12 0
#define TYPE_FAT16 1
#define TYPE_FAT32 2

#define TF_MARK_BAD_CLUSTER32 0x0ffffff7
#define TF_MARK_BAD_CLUSTER16 0xfff7
#define TF_MARK_EOC32 0x0ffffff8
#define TF_MARK_EOC16 0xfff8

#define LFN_ENTRY_CAPACITY 13       // bytes per LFN entry

#define TF_ATTR_DIRECTORY 0x10


//#define DEBUG 1


#ifdef DEBUG

    #define dbg_printf(...) printf(__VA_ARGS__)
    #define dbg_printHex(x,y) printHex(x,y)

    #define tf_printf(...) printf(__VA_ARGS__)
    #define tf_printHex(x,y) printHex(x,y)

#else   // DEBUG
    #define dbg_printf(...)
    #define dbg_printHex(x,y)
    #define tf_printf(...) 
    #define tf_printHex(x,y)
#endif  // DEBUG

#define LSN(CN, bpb) SSA + ((CN-2) * bpb->SectorsPerCluster)

#ifndef min
#define min(x,y)  (x<y)? x:y  
#define max(x,y)  (x>y)? x:y  
#endif

    
// Ultimately, once the filesystem is checked for consistency, you only need a few
// things to keep it up and running.  These are:
// 1) The type (fat16 or fat32, no fat12 support)
// 2) The number of sectors per cluster
// 3) Everything needed to compute indices into the FATs, which includes:
//    * Bytes per sector, which is fixed at 512
//    * The number of reserved sectors (pulled directly from the BPB)
// 4) The current sector in memory.  No sense reading it if it's already in memory!

typedef struct struct_tfinfo {
    // FILESYSTEM INFO PROPER
    uint8_t type; // 0 for FAT16, 1 for FAT32.  FAT12 NOT SUPPORTED
    uint8_t sectorsPerCluster;
    uint32_t firstDataSector;
    uint32_t totalSectors;
    uint16_t reservedSectors;
    // "LIVE" DATA
    uint32_t currentSector;
    uint8_t sectorFlags;
    uint32_t rootDirectorySize;
    char buffer[512];
} TFInfo;

/////////////////////////////////////////////////////////////////////////////////

typedef struct struct_TFFILE {
    uint32_t parentStartCluster;
    uint32_t startCluster;
    uint32_t currentClusterIdx; // cur rw pos is No.x cluster in this file
    uint32_t currentCluster;    // cur rw pos is No.x cluster in FAT
    short currentSector;
    short currentByte;  //在当前cluster内的偏移(byte为单位)也是下一个待写入的位置
    uint32_t pos;
    uint8_t flags;
    uint8_t attributes;
    uint8_t mode;
    uint32_t size;
    char filename[TF_MAX_PATH];
} TFFile;


#define TF_MODE_READ 0x01
#define TF_MODE_WRITE 0x02
#define TF_MODE_APPEND 0x04
#define TF_MODE_OVERWRITE 0x08

#define TF_ATTR_READ_ONLY 0x01
#define TF_ATTR_HIDDEN 0x02
#define TF_ATTR_SYSTEM 0x04
#define TF_ATTR_VOLUME_LABEL 0x08
#define TF_ATTR_DIRECTORY 0x10
#define TF_ATTR_ARCHIVE 0x20
#define TF_ATTR_DEVICE 0x40 // Should never happen!
#define TF_ATTR_UNUSED 0x80


int read_sector(char *data, uint32_t sector);
int write_sector(char *data, uint32_t blocknum);
// New error codes
#define TF_ERR_NO_ERROR 0
#define TF_ERR_BAD_BOOT_SIGNATURE 1
#define TF_ERR_BAD_FS_TYPE 2

#define TF_ERR_INVALID_SEEK 1

// FS Types
#define TF_TYPE_FAT16 0
#define TF_TYPE_FAT32 1

// New backend functions
int tf_init(void);
int tf_fetch(uint32_t sector);
int tf_store(void);
uint32_t tf_get_fat_entry(uint32_t cluster);
int tf_set_fat_entry(uint32_t cluster, uint32_t value);
int tf_unsafe_fseek(TFFile *fp, int32_t base, long offset);
TFFile *tf_fnopen(char *filename, const char *mode, int n);
int tf_free_clusterchain(uint32_t cluster);
int tf_create(char *filename);
void tf_release_handle(TFFile *fp);
TFFile *tf_parent(char *filename, const char *mode, int mkParents);
int tf_shorten_filename(char *dest, char *src, uint8_t num);

// New frontend functions
int tf_init();
int tf_fflush(TFFile *fp);
int tf_fseek(TFFile *fp, int32_t base, long offset);
int tf_fclose(TFFile *fp);
int tf_fread(char *dest, int size, TFFile *fp);
int tf_find_file(TFFile *current_directory, char *name);
int tf_compare_filename(TFFile *fp, char *name);
uint32_t tf_first_sector(uint32_t cluster);
char *tf_walk(char *filename, TFFile *fp);
TFFile *tf_fopen(char *filename, const char *mode);
int tf_fwrite(char *src, int size, int count, TFFile *fp);
int tf_fputs(char *src, TFFile *fp);
int tf_mkdir(char *filename, int mkParents);
int tf_remove(char *filename);
void tf_print_open_handles(void);

uint32_t tf_find_free_cluster();
uint32_t tf_find_free_cluster_from(uint32_t c);

uint32_t tf_initializeMedia(uint32_t totalSectors);
uint32_t tf_initializeMediaNoBlock(uint32_t totalSectors, int start);

// hidden functions... IAR requires that all functions be declared
TFFile *tf_get_free_handle();
char upper(char c);

// New Datas
extern TFInfo tf_info;
extern TFFile tf_file;



void printBPB(BPB_struct *s);
void print_sector(unsigned char *sector);
void print_tf_info(TFInfo *t);
void print_TFFile(TFFile *fp);
void print_FatFileEntry(FatFileEntry *entry);
void print_FatFile83(FatFile83 *entry);

#endif
