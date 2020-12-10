// Declare what kind of code we want
// from the header files. Defining __KERNEL__
// and MODULE allows us to access kernel-level
// code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE
#define BUFFSIZE 256
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
  int numChannel;
  char[BUFFSIZE] data;
  Channel * nextChannel
}
// muliple driver copies, found by looking at iminor
typedef struct minorChannel{
    int minorNum;
    Channel* myChannel;
    minorChannel * nextMinorChannel;
}
minorChannel * HEAD;

//================== DEVICE FUNCTIONS ===========================
static int device_open( struct inode* inode,
                        struct file*  file )
{
  printk("Invoking device_open(%p)\n", file);
  if(HEAD == NULL){
    HEAD =(minorChannel *) kmalloc(sizeof(minorChannel),GFP_KERNEL);
    if(HEAD==NULL){
      printk("KMALLOC HAS FAILED DEVICE WAS NOT OPENED\n");
      return -EINVAL
    }
    HEAD.minorNum=-1;
    printlk("Driver Has Been Loaded and Head Has Been Created\n");
    return SUCCESS
  }
  return SUCCESS;
}

//---------------------------------------------------------------
static int device_release( struct inode* inode,
                           struct file*  file)
{
  unsigned long flags; // for spinlock
  printk("Invoking device_release(%p,%p)\n", inode, file);

  // ready for our next caller
  spin_lock_irqsave(&device_info.lock, flags);
  --dev_open_flag;
  spin_unlock_irqrestore(&device_info.lock, flags);
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
    get_user(the_message[i], &buffer[i]);
    if( 1 == encryption_flag )
      the_message[i] += 1;
  }
 
  // return the number of input characters used
  return i;
}

//----------------------------------------------------------------
static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
  // Switch according to the ioctl called


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
  memset( &device_info, 0, sizeof(struct chardev_info) );
  spin_lock_init( &device_info.lock );

  // Register driver capabilities. Obtain major num
  rc = register_chrdev( MAJOR_NUM, DEVICE_RANGE_NAME, &Fops );

  // Negative values signify an error
  if( rc < 0 )
  {
    printk( KERN_ALERT "%s registraion failed for  %d\n",
                       DEVICE_FILE_NAME, MAJOR_NUM );
    return rc;
  }

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
