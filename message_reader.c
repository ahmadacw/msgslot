#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdlib.h>

#include "message_slot.h"


int main(int argc, char* argv[]){
    if (argc!=3){
        exit(1);
    }
    char* file_path;
    int target_id, fd, bytes_read;
    char buffer[BUFFSIZE];
    file_path = argv[1];
    target_id = atoi(argv[2]);
    fd = open(file_path, O_RDWR);
    if(fd < 0){
	printf("couldn't open file\n");
        exit(1);
    }
    if(ioctl(fd, MSG_SLOT_CHANNEL, target_id)!=SUCCESS){
	printf("ioctl fail\n");
        exit(1);
    }
    bytes_read = read(fd,buffer,target_id);
    if (bytes_read < 0){
	printf("NONE READ\n");
        exit(1);
    }
    if (bytes_read<BUFFSIZE){
        buffer[bytes_read] = '\0';
    }
    printf("%d bytes read from %s\n",bytes_read,file_path);
    printf("%s\n",buffer);
    return SUCCESS;



}