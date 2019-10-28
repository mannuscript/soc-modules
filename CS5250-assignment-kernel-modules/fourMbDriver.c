#include <linux/slab.h> //For kmalloc
#include <linux/fs.h> 	//For struct file-operations
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */ 
#define SCULL_IOC_MAGIC  'k' 
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC,   1)
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, char*)
#define SCULL_READ _IOR(SCULL_IOC_MAGIC, 3, char*)
#define SCULL_READ_WRITE _IOWR(SCULL_IOC_MAGIC, 4, char*)
#define SCULL_IOC_MAXNR 4

#define NAME "fourMbDriver"
#define DRIVER_SIZE 4*1000000
#define MSG_SIZE 1000

//Function declarations 
int d_open(struct inode*, struct file*);
int d_release(struct inode *, struct file*);
ssize_t d_read(struct file*, char *, size_t, loff_t *);
ssize_t d_write(struct file *, const char *, size_t, loff_t *);
static void exit_func(void);
static loff_t d_llseek(struct file*, loff_t, int);
long ioctl_example(struct file *, unsigned int, unsigned long);


char *data;
char *dev_msg;
char *tempBuffer;

int majorNumber = 62;   //Major number of kernel will be stored in this
size_t sizeOfMessage = 0; //Size of the message which has been stored till now

//Instance of file_operations struct is required to pass register_chardev function while registering a driver
//To define the functions for various operations, meaning of every operations is explained with the function
struct file_operations fOps = {
	read: d_read,
	write: d_write,
	open: d_open,
	release: d_release,
	llseek : d_llseek,
	unlocked_ioctl : ioctl_example,
};

long ioctl_example(struct file *filep, unsigned int cmd, unsigned long arg)
{
	int err = 0, tmp; 
	int retval = 0; 
	int dataToCopy = 0;
 	/* 
     	* extract the type and number bitfields, and don't decode 
     	* wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok() 
     	*/ 
	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC)
		return -ENOTTY;
    	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) 
		return -ENOTTY; 
     /* 
     * the direction is a bitmask, and VERIFY_WRITE catches R/W 
     * transfers. `Type' is user‐oriented, while 
     * access_ok is kernel‐oriented, so the concept of "read" and 
     * "write" is reversed 
     */ 
    	if (_IOC_DIR(cmd) & _IOC_READ) 
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd)); 
	else if (_IOC_DIR(cmd) & _IOC_WRITE) 
    		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd)); 
    
	if (err) return -EFAULT; 

	if(arg)
	{
		//type conversion to string
		arg = (char*)arg;
		//Find out the length of input string
		int argLength = strlen(arg);
		dataToCopy = (argLength < MSG_SIZE ? argLength : MSG_SIZE-1);
	}	

	switch(cmd) { 
        	case SCULL_HELLO: 
			printk(KERN_WARNING "hello\n"); 
        		break;
		case SCULL_READ:		
			printk(KERN_ALERT "SCULL_READ is being called when dev_msg is: %s\n", dev_msg);
			copy_to_user(arg, dev_msg, strlen(dev_msg));
			printk(KERN_ALERT "SCULL_READ returning the value: %s", arg);
			break;
		case SCULL_WRITE:	
			printk(KERN_ALERT "SCULL_WRITE is being called with argument: %s \n", arg);
			copy_from_user(dev_msg, arg, dataToCopy);
			dev_msg[dataToCopy] = '\0'; 
			printk(KERN_ALERT "DEBUG_POINT::IOCTL_EXAMPLE::SCULL_WRITE:: dev_msg: %s\n", dev_msg);
			break;
		case SCULL_READ_WRITE:
			printk(KERN_ALERT "SCULL_READ_WRITE called with argument: %s \n", arg);
			int devMsgLen = strlen(dev_msg);
			strcpy(tempBuffer, dev_msg);
			tempBuffer[devMsgLen+1] = '\0';
			copy_from_user(dev_msg, arg, dataToCopy);
			dev_msg[dataToCopy] = '\0';
			copy_to_user(arg, tempBuffer, devMsgLen+1);
			printk(KERN_ALERT "SCULL_READ_WRITE:: new value for dev_msg is: %s\n", dev_msg);
			break;
		default:  /* redundant, as cmd was checked against MAXNR */ 
    			return -ENOTTY; 
    	} 
    	return retval; 
}


int d_open(struct inode *inode, struct file *filep) 
{
  	filep->f_pos = kmalloc(sizeof(loff_t), GFP_KERNEL);

	if(filep->f_pos = NULL) {
		printk(KERN_ALERT "DEBUG::fourMbDriver::D_OPEN:: Failed t allocate memory to f_pos");
		return -EFAULT;
	}
	
	return 0; // always successful
}


int d_release(struct inode *inode, struct file *filep)
{
	kfree(filep->f_pos);
	filep->f_pos = NULL;
	return 0; // always successful
}

static loff_t d_llseek(struct file *filep, loff_t offset, int whence)
{

	printk(KERN_ALERT "DEBUG_POINT::fourMbDriver::llseek:: function called with params: %d %d", (int32_t)offset, whence);
    loff_t newPos = 0;
    switch(whence) {
    
        case SEEK_SET://Seek set 
            newPos = (int32_t)offset;
            break;
        case SEEK_CUR://Seek current
            newPos = (int32_t)offset + filep->f_pos;
            break;
        case SEEK_END://Seek end
            newPos = (int32_t)offset + sizeOfMessage;
            break;
        default:
            return -EINVAL;//operation not supported, return error
            break;
    
    }
    if(sizeOfMessage < newPos)
    {
	if(newPos > DRIVER_SIZE)
	{
            return -EINVAL;
	}
        memset(&data[sizeOfMessage], 0, newPos - sizeOfMessage);
	sizeOfMessage = newPos;
	printk(KERN_ALERT "DEBUGGING::d_ll_seek:: string after padding: %.*s, size: %d", newPos, data, sizeOfMessage); 
    }

    filep->f_pos = newPos;
    return newPos;
}

ssize_t d_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
	printk(KERN_ALERT "DEBUGGING_POINT:: f_pos: %d,  count: %d, sizeOfMessage: %d", *f_pos, count, sizeOfMessage);

	int 
		dataSizeToCopy = (count + *f_pos) < sizeOfMessage ? count : (sizeOfMessage - *f_pos),
		eCode;
	//Nothing to read if count is zero or everything has already been read
	if(!dataSizeToCopy || *f_pos >= sizeOfMessage)
		return 0;
	eCode = copy_to_user(buf, data + *f_pos, dataSizeToCopy);
	
//	printk(KERN_ALERT "DEBUGGING POINT: copied string %s", buf);
	
	//Error if ecode is not zero
	if(eCode)
	{
		printk(KERN_ALERT "fourMbDriver::d_read:: copy_to_user returned an error: %d", eCode);
		return -EFAULT;
	}

	//Successfully read count bytes ? cool let the context know
	*f_pos += dataSizeToCopy;

	//Return the number of bytes read
	return dataSizeToCopy;
}

ssize_t d_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
	//Get the size of the content already written to the file
	int 
		sizeToCopy = 0, 
		isOverFlow = 0,
		fromPos = 0;

	if(count > DRIVER_SIZE)
	{
		printk(KERN_ALERT "fourMbDriver::d_read:: trying to sopy count > DRIVER_SIZE, size: %d driver: %d",count, DRIVER_SIZE);
		//prepare to through error
		isOverFlow = 1;
		//but copy the data as much as possible
		sizeToCopy = DRIVER_SIZE;
		sizeOfMessage = DRIVER_SIZE; //File is full
	}
	//Now check If the new data will overflow the file
	else if(count + filep->f_pos > DRIVER_SIZE)
	{
		//If yes, flush the older content and set an error
		isOverFlow = 1;
		sizeToCopy = count;
		sizeOfMessage = count; //Deleting older content
		printk(KERN_ALERT "fourMbDriver::d_read:: Trying to copy when count + existing size > driver size");
	}
	else
	{
		fromPos = filep->f_pos;
		sizeOfMessage = max(sizeOfMessage, fromPos + count);
		sizeToCopy = count;
	}
	int eCode = copy_from_user(data + fromPos, buf, sizeToCopy);
	printk(KERN_ALERT "Going to return: isOverFLow: %d and sizeToCopy: %d  current file size: %d eCode: %d, written@: %d \n", isOverFlow, sizeToCopy,sizeOfMessage , eCode, fromPos);
	if(isOverFlow) 
	{
		printk(KERN_ALERT "fourMbDriver::d_read:: No space left on the device!");
	}
	//printk(KERN_ALERT "SSSSSSSSSSSSSSSSSSSSS: %.*s", sizeOfMessage, data);
	return sizeToCopy;
}


//This function will be called while creating a module initializer
//Hence 1.Register the driver (char driver)
static int init_func(void)
{
	
	//register_chardev resides in linux/cdev
	//dynamically allocate the major number
	int eCode = register_chrdev(majorNumber, NAME, &fOps);

	if(eCode < 0)
	{
		printk(KERN_ALERT "fourMbDriver::init_func:: Error in registering the driver!\n");
		//Returns a -ve errno on failure.
		return eCode;
	}

	/*
		For understanding kmalloc and GFP_KERNERL:
		http://www.makelinux.net/books/lkd2/ch11lev1sec4
	*/
	//Allocate memory
	data = kmalloc(DRIVER_SIZE, GFP_KERNEL);
	dev_msg = kmalloc(MSG_SIZE, GFP_KERNEL);
	tempBuffer = kmalloc(MSG_SIZE, GFP_KERNEL);	
	if (!data || !dev_msg || !tempBuffer) {
		exit_func();
		// cannot allocate memory
		// return no memory error, negative signify a failure
		return -ENOMEM;
	}

	printk(KERN_ALERT "Cheers Successfully Initialized a %d bytes char device driver\n", DRIVER_SIZE);
	return 0;
}

static void exit_func(void)
{
	if(data)
	{
		//Free up the memory
		kfree(data);
		data = NULL;
	}
	if(dev_msg)
	{
		kfree(dev_msg);
		dev_msg = NULL;
	}
	if(tempBuffer)
	{
		kfree(tempBuffer);
		tempBuffer = NULL;
	}
	//un-register the driver
	unregister_chrdev(majorNumber, NAME);
	printk(KERN_ALERT "%s driver has been successfully removed from your machine!\n", NAME);
}


module_init(init_func);
module_exit(exit_func);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mannu Malhotra");
MODULE_DESCRIPTION("4 MB device driver");
MODULE_VERSION("1");
