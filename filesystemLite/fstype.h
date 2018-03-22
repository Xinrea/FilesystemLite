//
//  fstype.h
//  filesystemLite
//
//  Created by 黄涵 on 2018/3/21.
//  Copyright © 2018年 黄涵. All rights reserved.
//

#ifndef fstype_h
#define fstype_h

#include <stdint.h>

#define FSLITE_DEFAULT_BLOCK_SIZE (4*1024)
#define FSLITE_DEFAULT_INODE_SPACE_SIZE (128*2048) //2048 files + dirs
#define FSLITE_DEFAULT_INODE_SIZE  (128)
#define P_INODE_MAP (4*1024)
#define P_DATA_MAP  (4*1024+256)
#define P_INODE_BASE (4*1024+256+2048)
#define BASE (4*1024+256+2048+128*2048)

#define TYPE_FILE 1
#define TYPE_DIR  2

#define LO_RDONLY 0x01
#define LO_WRONLY 0x02
#define LO_RDWR   0x03
#define LO_CREATE 0x04

struct fslite_super_block { // size: 4k
    uint32_t version;
    uint32_t block_size;
    uint32_t free_blocks;
    char padding[FSLITE_DEFAULT_BLOCK_SIZE - 3*sizeof(uint32_t)];
};

struct fslite_inode_map { //indicate the usage of inode
    unsigned char map[256];
};

struct fslite_data_map { //indicate the usage of data block（total: 2048*8）
    unsigned char map[2048];
};

struct fslite_inode { // size: 128
    uint32_t id;
    uint32_t open_flag;
    uint32_t type;
    uint32_t uid;
    uint32_t size;
    uint32_t mode;
    uint32_t block_num;
    uint32_t dblock[8];
    char padding[FSLITE_DEFAULT_INODE_SIZE - 15*sizeof(uint32_t)];
};

struct fslite_direntry{//size: 16
    uint32_t inode;
    char file_name[12];
};

struct fslite_dirblock {//must be the BLOCK_SIZE
    unsigned char entry_map[32];//a record for the usage of entry
    struct fslite_direntry entry[254]; // every block can contain 254 files/dirs
};
#endif /* fstype_h */
