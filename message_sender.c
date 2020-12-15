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



int main(int argc, char* argv[]){
    char* file_path;
    int target_id, fd, bytes_written;
    char* message;
    if(argc<4) exit(1);
    file_path = argv[1];
    target_id = atoi(argv[2]);
    message = argv[3];
    fd = open(file_path, O_RDWR);
    if(fd < 0){
        exit(1);
    }
    if(ioctl(fd, MSG_SLOT_CHANNEL, target_id)!=SUCCESS){
        exit(1);
    }
    bytes_written = write(fd, message, strlen(message)-1);
    if(bytes_written < 0 ){
        exit(1);
    }
    printf("message sender succeded, number of bytes written = %d\n",bytes_written);
    return 0;





    
    
}