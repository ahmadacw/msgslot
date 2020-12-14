// Declare what kind of code we want
// from the header files. Defining _KERNEL__
// and MODULE allows us to access kernel-level
// code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE
#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/init.h>
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/
#include <linux/slab.h>
#include <linux/string.h> 
#include "message_slot.h"
MODULE_LICENSE("GPL");
typedef struct Channel{
  int numChannel;
  int written;
  char data[BUFFSIZE];
  struct Channel * nextChannel;
} Channel;

// muliple driver copies, found by looking at iminor
typedef struct minorChannel{
    int minorNum;
    Channel* myNode;
    struct minorChannel * nextMinorChannel;
 } minorChannel;
static minorChannel * HEAD;
//================== DEVICE FUNCTIONS ===========================
static int device_open( struct inode* inode,
                        struct file*  file )
{
  int found;
  minorChannel* mychannel;
  minorChannel* newhead;
  mychannel=HEAD;
  found=0;
  printk("Invoking device_open(%p)\n", file);
  if(HEAD == NULL){
    printk("insmod has not been called\b");
	  return 1;
  }
  while(!found && mychannel!=NULL){
    if(mychannel->minorNum==iminor(inode)){
      found=1;
      break;
    }
    mychannel=mychannel->nextMinorChannel;
  }
  if(!found){
      newhead=(minorChannel*)kmalloc(sizeof(minorChannel),GFP_KERNEL);
      if(newhead==NULL) { // kmalloc failed  
        printk("kmalloc has failed on opening file\n");
        //errno = -EINVAL;
        return 1;
      }
    newhead->nextMinorChannel=HEAD;
    HEAD=newhead;
    newhead->minorNum=iminor(inode);
    newhead->myNode=NULL;
    file->private_data=(void*)-1;
  }
  return SUCCESS;
}

//---------------------------------------------------------------
static int device_release( struct inode* inode,
                           struct file*  file)
{
  printk("Invoking device_release(%p,%p)\n", inode, file);

  return SUCCESS;
}

//---------------------------------------------------------------
// a process which has already opened
// the device file attempts to read from it
static ssize_t device_read( struct file* file,
                            char __user* buffer,
                            size_t       length,
                            loff_t*      offset )
{
  int found,bytesRead,channelnum;
  int minorNumber;
  Channel* myChannel;
  minorChannel* myMinorChannel;
printk("=============Reading============\n");
  found=0;
  bytesRead=0;
  minorNumber=iminor(file->f_path.dentry->d_inode);
  if(HEAD == NULL){
    printk("insmod has not been called\b");
    return -1;
  }
  myMinorChannel=HEAD;
  while(!found && myMinorChannel!=NULL){
    if(myMinorChannel->minorNum==minorNumber){
      found=1;
      break;
    }
      myMinorChannel=myMinorChannel->nextMinorChannel;
  }
  if(!found){
    printk("no this file has never been opened before\n");
    return -1;
  }
  channelnum=(int)file->private_data;
  found =0;
  myChannel=myMinorChannel->myNode;
  while(myChannel!=NULL){
    if(myChannel->numChannel==channelnum){
      found=1;
      break;
    }
    myChannel=myChannel->nextChannel;
  }
  if(!found){ // trying to read from a channel which was not ioctaled.
    printk("something went wrong, the channel wasn't found\n");
    return -EINVAL;
  }
  /* 
    we found our node with the right minor num and the right channel num, we just
    need to read from it now
  */
  	if(myChannel->data[0]){
 		printk("%s\n",myChannel->data);
	}

  while((bytesRead<BUFFSIZE)&& bytesRead< strlen(myChannel->data)){
    put_user(myChannel->data[bytesRead],&buffer[bytesRead]);
    bytesRead++;
  }
  if(bytesRead==0){ 
   printk("nothing read\n");
    return -1;
  }
   printk( "Invocing device_read(%p,%ld) - "
          "operation not supported yet\n"
          "(last written - %s)\n",
          file, length, buffer );
  myChannel->data[0]='\0';
  return bytesRead;

}

//---------------------------------------------------------------
// a processs which has already opened
// the device file attempts to write to it
static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{
  int found,byteswritten,minorNumber,channelnum;
  minorChannel* myMinorChannel;
  Channel* myChannel;
printk("=============Writing %s============\n",buffer);
  found=0;
  byteswritten=0;
  minorNumber=iminor(file->f_path.dentry->d_inode);
  if(length>BUFFSIZE || length<1){
    //perror("but length for read\n");
    return -1;
  }
  if(HEAD == NULL){
    printk("insmod has not been called\b");
    return -1;
  }
  myMinorChannel=HEAD;
  while(!found && myMinorChannel!=NULL){
    if(myMinorChannel->minorNum==minorNumber){
      found=1;
      break;
    }
      myMinorChannel=myMinorChannel->nextMinorChannel;
  }
  if(!found){
    //perror("no this file has never been opened before\n");
    return -1;
  }
  channelnum=(int)file->private_data;
  found =0;
  myChannel=myMinorChannel->myNode;
  while(myChannel!=NULL){
    if(myChannel->numChannel==channelnum){
      found=1;
      break;
    }
    myChannel=myChannel->nextChannel;
  }
  if(!found){ // trying to write to a channel which was not ioctaled.
    return -EINVAL;
  }
  while(byteswritten<BUFFSIZE && byteswritten< length){
    get_user(myChannel->data[byteswritten],&buffer[byteswritten]);
    byteswritten++;
  }
  printk("%s",myChannel->data);
  myChannel->written=1;
  return byteswritten;

}


//----------------------------------------------------------------

static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
  // Switch according to the ioctl called
   int minorNum;
   minorChannel* mychannel;
  if(ioctl_param && MSG_SLOT_CHANNEL==ioctl_command_id){
    minorNum=iminor(file->f_path.dentry->d_inode);;
    mychannel=HEAD;
     if(mychannel==NULL){ // file has not been opened
        printk("no insmod opertion was ran on this driver\n");
        return 1;

    }
    while(mychannel && mychannel->minorNum!=minorNum){
      mychannel=mychannel->nextMinorChannel;
    }

    if(mychannel==NULL){ // file has not been opened
        printk("File has not been opened hence its minor node was not found\n");
        return 1;

    }

    Channel* foundChannel=mychannel->myNode;
    while(foundChannel){
      if(foundChannel->numChannel==ioctl_param) break;
      foundChannel=foundChannel->nextChannel;
    }
    if(foundChannel ==NULL){ // channel has not been created
      foundChannel=(Channel*) kmalloc(sizeof(Channel),GFP_KERNEL);
      foundChannel->nextChannel=mychannel->myNode;
      foundChannel->numChannel=ioctl_param;
      mychannel->myNode=foundChannel;
    }
    file->private_data=(void*) ioctl_param;
  }
  else{

	printk("else in ioctl\n");
    return -1;
  }
  return SUCCESS;

}


//==================== DEVICE SETUP =============================

// This structure will hold the functions to be called
// when a process does something to the device we created
struct file_operations Fops =
{
  .owner	  = THIS_MODULE, 
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
  .release        = device_release,
};

//---------------------------------------------------------------
// Initialize the module - Register the character device
static int __init simple_init(void)
{
	int retvalue;
   if(HEAD == NULL){
     HEAD =(minorChannel *) kmalloc(sizeof(minorChannel),GFP_KERNEL);
     if(HEAD==NULL){
       printk("KMALLOC HAS FAILED DEVICE WAS NOT OPENED\n");
       return -EINVAL;
	}
    HEAD->minorNum=-1;
	}
    printk("Driver Has Been Loaded and Head Has Been Created\n");

    retvalue=register_chrdev(MAJOR_NUM,DEVICE_RANGE_NAME,&Fops);
    if (retvalue<0){
        printk(KERN_ALERT "error in %d register_crdev",retvalue);
    }
    printk(KERN_INFO "message_slot registered major number %d\n",MAJOR_NUM);
    return SUCCESS;
}

//---------------------------------------------------------------
static void __exit simple_cleanup(void)
{
  // Unregister the device
  // Should always succeed
	minorChannel* currentHead = HEAD;
	while(currentHead){
		minorChannel* tmpHead=currentHead->nextMinorChannel;
		Channel* currentChannel=currentHead->myNode;
		while(currentChannel){
			Channel* tmpChannel =currentChannel->nextChannel;
			kfree(currentChannel);
			currentChannel=tmpChannel;
		}
		kfree(currentHead);	
		currentHead=tmpHead;
	}
  unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);

//========================= END OF FILE =========================
