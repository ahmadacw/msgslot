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
    int target_id, fd, bytesRead;
    char buffer[BUFFSIZE];
    target_id = atoi(argv[2]);
    fd = open(argv[1], O_RDWR);
    if(fd < 0){
	    strerror(EBADR);
        exit(1);
    }
    if(ioctl(fd, MSG_SLOT_CHANNEL, target_id)!=SUCCESS){
	    strerror(ENODATA);
        exit(1);
    }
    bytesRead= read(fd,buffer,target_id);
    if (bytesRead < 0){
	    strerror(ENODATA);
        exit(1);
    }
    write(STDOUT_FILENO ,buffer,bytesRead);
    return SUCCESS;



}