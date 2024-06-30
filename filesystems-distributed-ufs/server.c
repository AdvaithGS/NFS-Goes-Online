#include "udp.c"
#include "ufs.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <signal.h>

#define BUFFER_SIZE (1000)

bool SendResponse(mes_t* message, int* sd);
bool areEqual(char * a, char * b);
int rc;
int fd;
struct sockaddr_in addr;

// server code
int main(int argc, char* argv[]) {
    int sd = UDP_Open(atoi(argv[1]));
    fd = open(argv[2], O_RDWR);
    assert(fd > -1);
    struct stat sbuf;
    int rc = fstat(fd, &sbuf);
    assert(rc > -1);
    int image_size = (int)sbuf.st_size;
    void* image = mmap(NULL, image_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(image != MAP_FAILED);
    super_t* s = (super_t*)image;
    filesystem files;
    files.data_bm = *(bitmap_t*)(image + s->data_bitmap_addr * UFS_BLOCK_SIZE);
    files.inode_bm = *(bitmap_t*)(image + s->inode_bitmap_addr * UFS_BLOCK_SIZE);
    files.inodes = (inode_t*)(image + s->inode_region_addr * UFS_BLOCK_SIZE);

    printf("Opened filesystem {%s} on port {%s}\n",argv[2],argv[1]);
    assert(sd > -1);
    while (1){
        char * m1;
        printf("server:: waiting...\n");
        rc = UDP_Read(sd, &addr, m1, 30*sizeof(char));
        mes_t * message = (mes_t *)m1;
        printf("%s\n",message->name);
        printf("server:: read message [size:%d contents:(%d), name: (%s)]\n", rc, message->mes_type,message->name);
        if (rc > 0 && !SendResponse(message,&sd)){
            res_t* response;
            response->code = 0;
            response->content = "Internal Server Error :?";
            rc = UDP_Write(sd, &addr, (char *) &response, sizeof(res_t));
            printf("sent internal error\n");
        };
        // if (rc > 0){
        //     char reply[BUFFER_SIZE];
        //     sprintf(reply, "goodbye world");
        //     rc = UDP_Write(sd, &addr, reply, BUFFER_SIZE);
        //     // printf("server:: reply\n");
        // }
        }
    return 0;
    }

bool SendResponse(mes_t* message, int* sd) {
    
    
    
    res_t response;

    switch (message->mes_type){
        case __lookup: ;//weirdest workaround ever
            printf("Got to lookup\n");
            inode_t * inode = files.inodes + message->pinum;
            for(int i = 0; i < inode->size; i ++){
                dir_ent_t * obj =  image + (inode->direct[i] * UFS_BLOCK_SIZE);
                if(areEqual(obj->name,message->name)){
                    response.code = 1;
                    sprintf(response.content,"{%d}",obj->inum);
                    rc = UDP_Write(*sd, &addr, (char *) &response, sizeof(res_t));
                    return 1;
                }
            }
            return 0;
            break;
        default:
            break;
        }
    };


    bool areEqual(char * a, char * b){
        if(strlen(a) != strlen(b)){
            return 0;
        }else{
            for(int i = 0; i < strlen(a);i++){
                if(a[i] != b[i]){
                    return 0;
                }
            }
        }
        return 1;
    };