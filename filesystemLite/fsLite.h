//
//  fsLite.h
//  filesystemLite
//
//  Created by 黄涵 on 2018/3/20.
//  Copyright © 2018年 黄涵. All rights reserved.
//

#ifndef fsLite_h
#define fsLite_h

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define likely(x) __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)

#define BASE_SIZE (100*1024*1024)
#define FS_ERR_INIT -1

unsigned int fslite_init(void);
#endif /* fsLite_h */
