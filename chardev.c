// Declare what kind of code we want
// from the header files. Defining __KERNEL__
// and MODULE allows us to access kernel-level
// code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE
#define BUFFSIZE 128
#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/

MODULE_LICENSE("GPL");

//Our custom definitions of IOCTL operations
#include "chardev.h"

//multiple channels in each driver copy
typedef struct Channel{
  int minorNum; // useful for effeciancy
  int numChannel;
  char data[BUFFSIZE];
  Channel * nextChannel
} Channel;
// muliple driver copies, found by looking at iminor
typedef struct minorChannel{
    int currentChannel; // need to save the where we were before we moved
    int minorNum;
    Channel* myNode;
    minorChannel * nextMinorChannel;
 } minorChannel;
minorChannel * HEAD;
minorChannel * mychannel; // either HEAD or right of head
//================== DEVICE FUNCTIONS ===========================
static int device_open( struct inode* inode,
                        struct file*  file )
{
  printk("Invoking device_open(%p)\n", file);
  minorChannel* newhead=(minorChannel *)kmalloc(sizeof(minorChannel),GFP_KERNEL);
    if(newhead==NULL) { // kmalloc failed
      printk("kmalloc has failed on opening file\n");
      errno = -EINVAL;
      return 0;
    }
  newhead->nextMinorChannel=HEAD;
  HEAD=newhead;
  newhead->minorNum=iminor();
  newhead->myNode=NULL;
  newhead->currentChannel=-1;
  
  return SUCCESS;
}
int freeALL(){
  return SUCCESS;
}
//---------------------------------------------------------------
static int device_release( struct inode* inode,
                           struct file*  file)
{
  printk("Invoking device_release(%p,%p)\n", inode, file);

  freeALL();
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
  // read doesnt really do anything (for now)
  printk( "Invocing device_read(%p,%ld) - "
          "operation not supported yet\n"
          "(last written - %s)\n",
          file, length, the_message );

  //invalid argument error
  return -EINVAL;
}

//---------------------------------------------------------------
// a processs which has already opened
// the device file attempts to write to it
static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{
  int i;
  printk("Invoking device_write(%p,%ld)\n", file, length);
  for( i = 0; i < length && i < BUF_LEN; ++i )
  {

 
  // return the number of input characters used
  return i;
}

//----------------------------------------------------------------
static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
  // Switch according to the ioctl called
  if(ioctl_param && MSG_SLOT_CHANNEL==ioctl_command_id){
    minorChannel* mychannel=Head;
    while(mychannel->minorNum!=iminor()){
      mychannel=mychannel->nextMinorChannel;
    }
    if(mychannel==NULL){ // file has not been opened
        printk("File has not been opened hence its minor node was not found\n");
        return 0;
    }
    Channel* foundChannel=minorChannel->myNode;
    if(foundChannel ==NULL){
      foundChannel=(Channel*) kmalloc(sizeof(channel),GFP_KERNEL);
    }
  }
  else{
    errno=-EINVAL;
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
  int rc = -1;
  // init dev struct
  // if(HEAD == NULL){
  //   HEAD =(minorChannel *) kmalloc(sizeof(minorChannel),GFP_KERNEL);
  //   if(HEAD==NULL){
  //     printk("KMALLOC HAS FAILED DEVICE WAS NOT OPENED\n");
  //     return -EINVAL
  //   }
  //   HEAD.minorNum=-1;
  //   mychannel=HEAD;
  Head=NULL;
    printk("Driver Has Been Loaded and Head Has Been Created\n");
    return SUCCESS
  }
  // Register driver capabilities. Obtain major num

  printk( "Registeration is successful. ");
  printk( "If you want to talk to the device driver,\n" );
  printk( "you have to create a device file:\n" );
  printk( "mknod /dev/%s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM );
  printk( "You can echo/cat to/from the device file.\n" );
  printk( "Dont forget to rm the device file and "
          "rmmod when you're done\n" );

  return 0;
}

//---------------------------------------------------------------
static void __exit simple_cleanup(void)
{
  // Unregister the device
  // Should always succeed
  unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);

//========================= END OF FILE =========================
