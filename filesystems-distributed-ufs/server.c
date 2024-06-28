#include <stdio.h>
#include "udp.h"
#include "libmfs.c"

#define BUFFER_SIZE (1000)

// server code
int main(int argc, char *argv[])
{
    int sd = UDP_Open(atoi(argv[1]));
    
    assert(sd > -1);
    while (1){
        char message[BUFFER_SIZE];
        printf("server:: waiting...\n");
        int rc = UDP_Read(sd, &addr, message, BUFFER_SIZE);
        connected  = true;
        printf("server:: read message [size:%d contents:(%s)]\n", rc, message);
        if (rc > 0){
            char reply[BUFFER_SIZE];
            sprintf(reply, "goodbye world");
            rc = UDP_Write(sd, &addr, reply, BUFFER_SIZE);
            // printf("server:: reply\n");
        }
    }
    return 0;
}
