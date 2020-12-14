#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include "message_slot.h"
#define SUCCESS 0
#define FAILURE 1

void check_parameters(int argc){
    if (argc!=4){
	printf("hallo");
        exit(FAILURE);
    }

}

int main(int argc, char* argv[]){
    char* file_path;
    int target_id, fd, bytes_written;
    char* message;

    check_parameters(argc);

    file_path = argv[1];
    target_id = atoi(argv[2]);
    message = argv[3];
    fd = open(file_path, O_RDWR);
	printf("%s\n",file_path);
    if(fd < 0){
	printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
	printf("fail 1 %d\n",fd);

        exit(FAILURE);
    }
    if(ioctl(fd, MSG_SLOT_CHANNEL, target_id)!=SUCCESS){
	printf("fail ioctl");
        exit(FAILURE);
    }
    bytes_written = write(fd, message, strlen(message));
    if(bytes_written < 0 ){
	printf("ailr write");
        exit(FAILURE);
    }
    printf("message sender succeded, number of bytes written = %d\n",bytes_written);
    exit(SUCCESS);
    return 0;





    
    
}