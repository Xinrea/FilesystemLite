//
//  fsLite.h
//  filesystemLite
//
//  Created by 黄涵 on 2018/3/20.
//  Copyright © 2018年 黄涵. All rights reserved.
//

#ifndef fsLite_h
#define fsLite_h

#include "fstype.h"

#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>

#define likely(x) __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)

#define DEVICE "base.vdisk"
#define BASE_SIZE (128*1024*1024)
#define FS_ERR_INIT -1
#define FS_ERR_OPDEV -2

int fslite_init(void);
int fslite_fill_super_block(struct fslite_super_block* sb);
int fslite_init_inode_map(void);
int fslite_init_data_map(void);
int fslite_init_inode_space(void);
uint32_t fslite_new_inode(void);
uint32_t fslite_new_block(void);
//uint32_t fslite_open(const char* path, uint32_t mode, uint32_t permission);
uint32_t fslite_create_file(char* path,void* data, uint32_t size);
uint32_t fslite_get_dir(uint32_t dir_id,char names[12][12]);
uint32_t fslite_open(char* path,uint32_t mode);
#endif /* fsLite_h */
