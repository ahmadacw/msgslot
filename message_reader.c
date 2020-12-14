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
#define SUCCESS 0
#define FAILURE 1
#define BUFFER_SIZE 128

void check_parameters(int argc){
    if (argc!=3){
        exit(FAILURE);
    }
}

int main(int argc, char* argv[]){
    char* file_path;
    int target_id, fd, bytes_read;
    char buffer[BUFFER_SIZE];
    check_parameters(argc);

    file_path = argv[1];
    target_id = atoi(argv[2]);
    fd = open(file_path, O_RDWR);
    if(fd < 0){
	printf("couldn't open file\n");
        exit(FAILURE);
    }
    if(ioctl(fd, MSG_SLOT_CHANNEL, target_id)!=SUCCESS){
	printf("ioctl fail\n");
        exit(FAILURE);
    }
    bytes_read = read(fd,buffer,target_id);
    if (bytes_read < 0){
	printf("NONE READ\n");
        exit(FAILURE);
    }
    if (bytes_read<BUFFER_SIZE){
        buffer[bytes_read] = '\0';
    }
    printf("%s\n",buffer);
    printf("%d bytes read from %s\n",bytes_read,file_path);
    exit(SUCCESS);
    return SUCCESS;



}