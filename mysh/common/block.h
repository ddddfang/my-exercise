#ifndef __BLOCK_H
#define __BLOCK_H

#include "fs/ufat.h"

int block_device_init();
int block_device_deinit();
int read_sector(const struct ufat_device *dev, ufat_block_t start, ufat_block_t count, void *buffer);
int write_sector(const struct ufat_device *dev, ufat_block_t start, ufat_block_t count, const void *buffer);

#endif


