#include "block.h"
#include <stdio.h>

int block_device_init() {
    return 0;
}

int block_device_deinit() {
    return 0;
}

int read_sector(const struct ufat_device *dev, ufat_block_t start, ufat_block_t count, void *buffer) {
    FILE *fp = fopen("./test.fat32", "r+b");

    fseek(fp, start << dev->log2_block_size, SEEK_SET);
    fread(buffer, 1 << dev->log2_block_size, count, fp);
    fclose(fp);
    return 0;
}

int write_sector(const struct ufat_device *dev, ufat_block_t start, ufat_block_t count, const void *buffer) {
    FILE *fp = fopen("./test.fat32", "r+");

    fseek(fp, start << dev->log2_block_size, SEEK_SET);
    fwrite(buffer, 1 << dev->log2_block_size, count, fp);
    fclose(fp);
    return 0;
}
