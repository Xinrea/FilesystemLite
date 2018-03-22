//
//  fsLite.c
//  filesystemLite
//
//  Created by 黄涵 on 2018/3/20.
//  Copyright © 2018年 黄涵. All rights reserved.
//

#include "fsLite.h"

int set_map(unsigned char* m, uint32_t id);
uint32_t find_file_under_dir(uint32_t dir_inode, const char* filename);

int fslite_init(void){
    int fd = open(DEVICE, O_WRONLY|O_CREAT, 0666);
    if(unlikely(fd == -1)){
        printf("Log: Open file error\n");
        return FS_ERR_INIT;
    }
    char temp[1024] = {0};
    for(int i = 0; i < BASE_SIZE/1024; ++i){
        write(fd, (void*)temp, 1024);
    }
#ifdef DEBUG
    printf("Log: Create file success\n");
#endif
    close(fd);
    return 0;
}

int fslite_fill_super_block(struct fslite_super_block* sb){
    int fd = open(DEVICE,O_WRONLY);
    if(unlikely(fd == -1)){
        printf("Log: Open file error\n");
        return FS_ERR_OPDEV;
    }
    ssize_t err = write(fd, (void*)sb, sizeof(struct fslite_super_block));
    if(unlikely(err == -1||err != FSLITE_DEFAULT_BLOCK_SIZE)){
        printf("Log: Write block error\n");
        close(fd);
        return FS_ERR_INIT;
    }
    close(fd);
#ifdef DEBUG
    printf("Log: Fill super_block success\n");
#endif
    return 0;
}

int fslite_init_inode_map(void){
    struct fslite_inode_map inode_m;
    memset((void*)(inode_m.map), 0, 256);
    int fd = open(DEVICE,O_WRONLY);
    if(unlikely(fd == -1)){
        printf("Log: Open file error\n");
        return FS_ERR_OPDEV;
    }
    lseek(fd, P_INODE_MAP, SEEK_SET);
    set_map(inode_m.map, 1);
    ssize_t err = write(fd, (void*)&inode_m, sizeof(struct fslite_inode_map));
    if(unlikely(err == -1||err != sizeof(struct fslite_inode_map))){
        printf("Log: Write block error\n");
        close(fd);
        return FS_ERR_INIT;
    }
    close(fd);
#ifdef DEBUG
    printf("Log: Init inode_map success\n");
#endif
    return 0;
}

int fslite_init_data_map(void){
    struct fslite_data_map data_m;
    memset((void*)(data_m.map), 0, 2048);
    int fd = open(DEVICE,O_WRONLY);
    if(unlikely(fd == -1)){
        printf("Log: Open file error\n");
        return FS_ERR_OPDEV;
    }
    set_map(data_m.map, 1);
    lseek(fd, P_DATA_MAP, SEEK_SET);
    ssize_t error = write(fd, (void*)&data_m, sizeof(struct fslite_data_map));
    if(unlikely(error == -1||error != sizeof(struct fslite_data_map))){
        printf("Log: Write block error\n");
        close(fd);
        return FS_ERR_INIT;
    }
    close(fd);
#ifdef DEBUG
    printf("Log: Init data_map success\n");
#endif
    return 0;
}

const char char_tb[256] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2, // 140
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 160
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 180
    2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3, // 200
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, // 220
    3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4, // 240
    5,5,5,5,5,5,5,5,6,6,6,6,7,7,8,0
};

uint32_t find_free(unsigned char* m, unsigned int size){
    uint32_t id;
    unsigned char base = 0x1;
    for(unsigned int i = 0; i < size; ++i){
        if(char_tb[(uint32_t)(m[i])]){
            id = char_tb[(uint32_t)(m[i])];
            m[i] = m[i]|(base << (8 - id));
            return id + 8*i - 1;
        }
    }
    return 0;
}

unsigned int valid(unsigned char* m, uint32_t id){
    uint32_t row = id/8;
    return ((m[row]) << id) >> (7 - id);
}


int set_map(unsigned char* m, uint32_t id){ //to free block/inode
    uint32_t row = id/8;
    unsigned char base = 0x1;
    m[row] = m[row]^(base << (8 - id%8));
    return 0;
}

int fslite_init_inode_space(void){
    struct fslite_inode root_inode;
    root_inode.id = 1;
    root_inode.type = TYPE_DIR;
    root_inode.uid = 0;
    root_inode.mode = 0;
    root_inode.open_flag = 0;
    root_inode.block_num = 1;
    root_inode.dblock[0] = 0;
    int fd = open(DEVICE,O_RDWR);
    if(unlikely(fd == -1)){
        printf("Log: Open file error\n");
        return FS_ERR_OPDEV;
    }
    lseek(fd, P_INODE_BASE, SEEK_SET);
    write(fd, (void*)&root_inode, FSLITE_DEFAULT_INODE_SIZE);
    lseek(fd, BASE, SEEK_SET); // write block
    struct fslite_dirblock block;
    strcpy(block.entry[0].file_name, "welcome");
    set_map(block.entry_map, 1);
    write(fd, (void*)&block, FSLITE_DEFAULT_BLOCK_SIZE);
    close(fd);
    return 0;
}


uint32_t fslite_new_inode(void){
    struct fslite_inode_map inode_m;
    int fd = open(DEVICE,O_RDWR);
    if(unlikely(fd == -1)){
        printf("Log: Open file error\n");
        return FS_ERR_OPDEV;
    }
    lseek(fd, P_INODE_MAP, SEEK_SET);
    read(fd, (void*)&inode_m, sizeof(struct fslite_inode_map));
    uint32_t id = find_free(inode_m.map,256);
    lseek(fd, P_INODE_MAP, SEEK_SET);
    write(fd, (void*)&inode_m, sizeof(struct fslite_inode_map));// save the change
    close(fd);
    return id;
}

uint32_t fslite_new_block(void){
    struct fslite_data_map data_m;
    int fd = open(DEVICE,O_RDWR);
    if(unlikely(fd == -1)){
        printf("Log: Open file error\n");
        return FS_ERR_OPDEV;
    }
    lseek(fd, P_DATA_MAP, SEEK_SET);
    read(fd, (void*)&data_m, sizeof(struct fslite_data_map));
    uint32_t id = find_free(data_m.map,2048);
    lseek(fd, P_DATA_MAP, SEEK_SET);
    write(fd, (void*)&data_m, sizeof(struct fslite_data_map));// save the change
    close(fd);
    return id;
}

//int fslite_open(const char* path, uint32_t mode, uint32_t permission){
//    if()
//}

uint32_t fslite_create_file(char* path,void* data, uint32_t size){
    char* filename,*temp;
    temp = strtok(path, "/");
    if(temp == NULL)temp = strtok(NULL, "/");
    filename = temp;
    uint32_t inode,itemp;
    inode = 0;itemp = 0;
    do {
        filename = temp;
        inode = itemp;
        itemp = find_file_under_dir(inode, filename);
        temp = strtok(NULL, "/");
        if(temp != NULL && itemp == -1)return -1;// find the same name file, but it's not dir_type
        if(temp == NULL && itemp != 0)return -1;// find the same name file/dir
        if(temp == NULL && itemp == 0){         // find the last dir and no conflict, create file
            int fd = open(DEVICE,O_RDWR);
            if(unlikely(fd == -1)){
                printf("Log: Open file error\n");
                return FS_ERR_OPDEV;
            }
            struct fslite_inode ninode,finode;
            struct fslite_dirblock dir;
            lseek(fd, P_INODE_BASE + inode*FSLITE_DEFAULT_INODE_SIZE, SEEK_SET);
            read(fd, (void*)&ninode, FSLITE_DEFAULT_INODE_SIZE);
            lseek(fd, BASE+ninode.dblock[0]*FSLITE_DEFAULT_BLOCK_SIZE, SEEK_SET);
            read(fd, (void*)&dir, FSLITE_DEFAULT_BLOCK_SIZE);
            uint32_t entry_id = find_free(dir.entry_map, 32);
            uint32_t inode_id = fslite_new_inode();
            /* edit dir entry */
            dir.entry[entry_id].inode = inode_id;
            strcpy(dir.entry[entry_id].file_name, filename);
            lseek(fd, BASE+ninode.dblock[0]*FSLITE_DEFAULT_BLOCK_SIZE, SEEK_SET);
            write(fd, (void*)&dir, FSLITE_DEFAULT_BLOCK_SIZE);
            
            /* edit fnode */
            finode.id = inode_id;
            finode.block_num = (size + FSLITE_DEFAULT_BLOCK_SIZE - 1)/FSLITE_DEFAULT_BLOCK_SIZE;
            for(int i = 0; i < finode.block_num; ++i){
                finode.dblock[i] = fslite_new_block();
                if(i != finode.block_num){
                    lseek(fd, BASE+finode.dblock[i]*FSLITE_DEFAULT_BLOCK_SIZE, SEEK_SET);
                    write(fd, data, FSLITE_DEFAULT_BLOCK_SIZE);
                }
                else {
                    lseek(fd, BASE+finode.dblock[i]*FSLITE_DEFAULT_BLOCK_SIZE, SEEK_SET);
                    write(fd, data, size%FSLITE_DEFAULT_BLOCK_SIZE);
                }
                
            }
            lseek(fd, P_INODE_BASE+inode_id*FSLITE_DEFAULT_INODE_SIZE, SEEK_SET);
            write(fd, (void*)&finode, FSLITE_DEFAULT_INODE_SIZE);
            close(fd);
#ifdef DEBUG
            printf("Log: Create file [%d] %s : %d bytes\n",inode_id,filename,size);
#endif
            return inode_id;
        }
    } while (temp);
    return 0;
}

uint32_t find_file_under_dir(uint32_t dir_inode, const char* filename){
    int fd = open(DEVICE,O_RDONLY);
    if(unlikely(fd == -1)){
        printf("Log: Open file error\n");
        return FS_ERR_OPDEV;
    }
    struct fslite_inode inode;
    lseek(fd, P_INODE_BASE + dir_inode*FSLITE_DEFAULT_INODE_SIZE, SEEK_SET);
    read(fd, (void*)&inode, FSLITE_DEFAULT_INODE_SIZE);
    
    uint32_t block_id;
    struct fslite_dirblock dirb;
    for(uint32_t i = 0; i < inode.block_num; ++i){
        block_id = inode.dblock[i];
        lseek(fd, BASE + block_id*FSLITE_DEFAULT_BLOCK_SIZE, SEEK_SET);
        read(fd, (void*)&dirb, sizeof(struct fslite_dirblock));
        for(uint32_t k = 0; k < 254; ++k){
            if(valid(dirb.entry_map, k)){
                if(!strcmp(dirb.entry[k].file_name, filename)){
                    lseek(fd, P_INODE_BASE + dirb.entry[k].inode*FSLITE_DEFAULT_BLOCK_SIZE, SEEK_SET);
                    struct fslite_inode temp;
                    read(fd, (void*)&temp, FSLITE_DEFAULT_INODE_SIZE);
                    if(temp.type == TYPE_DIR)return dirb.entry[k].inode;
                    else return -1;
                }
            }
            else continue;
        }
    }
    return 0;
}

uint32_t fslite_get_dir(uint32_t dir_id, char* names){
    
}







