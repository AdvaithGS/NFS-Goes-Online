#include "udp.c"
#include "ufs.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <signal.h>

int main(){
  int fd = open("database", O_RDWR);
  assert(fd > -1);
  struct stat sbuf;
  int rc = fstat(fd, &sbuf);
  assert(rc > -1);
  int image_size = (int)sbuf.st_size;
  void *image = mmap(NULL, image_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  assert(image != MAP_FAILED);
  super_t *s = (super_t *)image;
  
  filesystem files;
  files.data_bm = *(bitmap_t *)(image + s->data_bitmap_addr * UFS_BLOCK_SIZE);
  files.inode_bm = *(bitmap_t *)(image + s->inode_bitmap_addr * UFS_BLOCK_SIZE);
  files.inodes = (inode_t *)(image + s->inode_region_addr * UFS_BLOCK_SIZE);
  dir_ent_t *data = (dir_ent_t * )(image + files.inodes[0].direct[1] * UFS_BLOCK_SIZE);
  printf("%s\n", data->name);
}