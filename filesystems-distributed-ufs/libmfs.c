#include "ufs.h"
#include "udp.h"
#include "mfs.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#define BUFFER_SIZE (1000)

bool connected = false;

struct sockaddr_in addrSnd, addrRcv;
int sd;
int rc;

int makeConnection(char *hostname, int port){
  struct sockaddr_in addrSnd, addrRcv;
  sd = UDP_Open(20000);
  rc = UDP_FillSockAddr(&addrSnd, hostname, port);
  connected = true;
  return 0;
}

int sendMessage(mes_t * message){
  printf("client:: send message [%d]\n", message->type);
  rc = UDP_Write(sd, &addrSnd, (char *)message, BUFFER_SIZE);
  if (rc < 0){
    return 0;
  }
  res_t * response;
  rc = UDP_Read(sd, &addrRcv, response, BUFFER_SIZE);
  if(response->code == 1){
    printf("Reponse received: {%s}",response->content);
  }else{
    return 0;
  }
  return 1;
}

int MFS_Init(char *hostname, int port){
  if (connected){
    return 1;
  }
  else{
    makeConnection(hostname, port);
    return 1;
  }
  return 0;
}
int MFS_Lookup(int pinum, char *name){
  
};
int MFS_Stat(int inum, MFS_Stat_t *m);
int MFS_Write(int inum, char *buffer, int offset, int nbytes);
int MFS_Read(int inum, char *buffer, int offset, int nbytes);
int MFS_Creat(int pinum, int type, char *name);
int MFS_Unlink(int pinum, char *name);
int MFS_Shutdown(){
  mes_t * mes;
  mes->mes_type = shut;

  exit(1);
};

int main(int argc, char **argv)
{
  super_t *s = (super_t *)image_data(argv[1]);
  printf("%d\n", s->inode_bitmap_addr);
}

// total blocks        36
// inodes            32 [size of each: 128]
// data blocks       32
// layout details
// inode bitmap address/len 1 [1]
// data bitmap address/len  2 [1]