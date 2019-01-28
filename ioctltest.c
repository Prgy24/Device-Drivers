#include<linux/init.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
#include<linux/circ_buf.h>
#include<linux/wait.h>
#include<linux/sched.h>
#include"ioctl_test.h"
#define MAJORNO 42
#define NO_MINOR 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PP");

static dev_t devno;

static struct cdev _cdev;
static struct circ_buf cbuf;
wait_queue_head_t wq;

int ioctltest_open(struct inode *inodp, struct file *filp)
{
        printk("In open call\n");
        return 0;
}

int ioctltest_close(struct inode *inodp, struct file *filp)
{
        printk("In close call\n");
        return 0;
}

ssize_t ioctltest_read(struct file *filp, char __user *ubuff,size_t cnt, loff_t *off)
{
	int i,ret,m;
        printk("In read system call\n");
	
	wait_event_interruptible(wq,CIRC_CNT(cbuf.head,cbuf.tail,SIZE));
	m=min(cnt,CIRC_CNT(cbuf.head,cbuf.tail,SIZE));

	for(i=0; i<cnt; i++)
	{
		ret = copy_to_user(ubuff+i,cbuf.buf+cbuf.tail,1);

		if(ret)
		{
			printk("Error copying\n");
		return -1;
		}

		cbuf.tail = (cbuf.tail+1)&(SIZE-1);
	}		
        return 0;
}

ssize_t ioctltest_write(struct file *filp , const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i,ret,m;
        printk("In write system call\n");

	m= min(cnt,CIRC_SPACE(cbuf.head,cbuf.tail,SIZE));

	for(i=0; i<cnt; i++)
	{
		ret = copy_from_user(cbuf.buf+cbuf.head,ubuff+i,1);

		if(ret)
		{
			printk("Error copying\n");
		return -1;
		}

		cbuf.head = (cbuf.head+1)&(SIZE -1);
	}
	wake_up(&wq);

	return i;
}

static long ioctltest_ioctl(struct file *filp , unsigned int cmd , unsigned long arg)
{
	switch(cmd)
	{
		case _CMD_TEST1:
				printk("CMD TEST1 IOCTL demo\n");
				break;
	
		case _CMD_TEST2:
				if(!capable(CAP_SYS_ADMIN))
					return -EPERM;
				printk("In CMD TEST2 demo\n");
				put_user(20, (int*)arg);
				break;
		default:
			printk("Not valid IOCTL command\n");
			return -ENOTTY;
	}
return 0;
}

	
static struct file_operations fops={
                .owner = THIS_MODULE,
                .open = ioctltest_open,
                .release = ioctltest_close,
                .read = ioctltest_read,
                .write = ioctltest_write,
		.unlocked_ioctl = ioctltest_ioctl
};


static __init int ioctltest_init(void){
        int ret;

	printk("In init function\n");
        devno = MKDEV(MAJORNO,0);

        ret = register_chrdev_region(devno,NO_MINOR,"test_dev");

        if(ret){
                printk("Error registering device number\n");
                return ret;
                }
	printk("successfully registered\n");

	cbuf.buf = kmalloc(SIZE,GFP_KERNEL);

	if(!cbuf.buf)
	{
		printk("Error allocating memory\n");
		unregister_chrdev_region(devno,1);
	return -1;
	}

	init_waitqueue_head(&wq);
        cdev_init(&_cdev,&fops);

        ret = cdev_add(&_cdev, devno, NO_MINOR);
        if(ret){
                unregister_chrdev_region(devno,NO_MINOR);  
		kfree(cbuf.buf);
        return ret;
                }

        return 0;
}


static __exit void ioctltest_exit(void){

	printk("In exit\n");
        cdev_del(&_cdev);
        unregister_chrdev_region(devno,NO_MINOR);
	kfree(cbuf.buf);
        }

module_init(ioctltest_init);
module_exit(ioctltest_exit);

