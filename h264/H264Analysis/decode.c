//
//  decode.c
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/16.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include "nalu.h"
#include "stream.h"
#include "slice.h"

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        printf("usage : ./xxx <h264file>\n");
        return -1;
    }
    // 0. 读取h264文件
    //int buff_size = readAnnexbBitStreamFile("silent_cif_baseline_5_frames.h264");
    int buff_size = readAnnexbBitStreamFile(argv[1]);
    printf("totalSize: %d\n", buff_size);

    // 1. 开辟nalu_t保存nalu_header和SODB
    nalu_t *nalu = allocNalu(MAX_NALU_SIZE);

    int nalu_i = 0;
    int curr_nal_start = 0;  // 当前找到的nalu起始位置
    int curr_find_index = 0; // 当前查找的位置索引

    // 2.找到h264码流中的各个nalu
    while (find_nal_unit(nalu, buff_size, &curr_nal_start, &curr_find_index) > 0) {
        printf("nalu: %d, start: %d, index: %d, size: %d\n", nalu_i, curr_nal_start, curr_find_index, curr_find_index - curr_nal_start);
 
        // 3.读取/解析 nalu
        read_nal_unit(nalu);

        nalu_i++;
    }

    freeNalu(nalu);
    freeFilebuffer();

    return 0;
}
