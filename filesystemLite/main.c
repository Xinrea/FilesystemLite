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
    char path[12] = "/good2";
    char data[32] = "goodtest2";
    fslite_create_file(path, (void*)data, 32);
    char  names[12][12];
    int size = fslite_get_dir(0, names);
    for(int i = 0; i < size; ++i){
        printf("%s\n",names[i]);
    }
    return 0;
}
