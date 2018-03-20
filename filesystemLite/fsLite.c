//
//  fsLite.c
//  filesystemLite
//
//  Created by 黄涵 on 2018/3/20.
//  Copyright © 2018年 黄涵. All rights reserved.
//

#include "fsLite.h"

unsigned int fslite_init(void){
    int fd = open("/home/base.vdisk", O_WRONLY|O_CREAT, 0777);
    if(unlikely(fd == -1)){
#ifdef DEBUG
        printf("Log: Open file error\n");
#endif
        return FS_ERR_INIT;
    }
    ftruncate(fd, BASE_SIZE);
    close(fd);
    return 0;
}
