//
//  main.c
//  filesystemLite
//
//  Created by 黄涵 on 2018/3/20.
//  Copyright © 2018年 黄涵. All rights reserved.
//

#include <stdio.h>
#include "fsLite.h"

int main(int argc, const char * argv[]) {
    struct fslite_super_block sb;
    sb.version = 1;
    sb.block_size = 4096;
    sb.free_blocks = 2048*8;
//    fslite_init();
    fslite_fill_super_block(&sb);
    fslite_init_data_map();
    fslite_init_inode_map();
    fslite_init_inode_space();
    char path[12] = "/good";
    char data[32] = "goodtest";
    fslite_create_file(path, (void*)data, 32);
    return 0;
}
