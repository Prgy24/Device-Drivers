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
static wait_queue_head_t wq;

int test_open(struct inode *inodp, struct file *filp)
{
        printk("In open call\n");
        return 0;
}

int test_close(struct inode *inodp, struct file *filp)
{
        printk("In close call\n");
        return 0;
}

ssize_t test_read(struct file *filp, char __user *ubuff,size_t cnt, loff_t *off)
{
	int i,ret,m;
        printk("In read system call\n");
	
	wait_event_interruptible(wq,CIRC_CNT(cbuf.head,cbuf.tail,SIZE));
	m=min(cnt,CIRC_CNT(cbuf.head,cbuf.tail,SIZE));

	for(i=0; i<m; i++)
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

ssize_t test_write(struct file *filp , const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i,ret,m;
        printk("In write system call\n");

	m= min(cnt,CIRC_SPACE(cbuf.head,cbuf.tail,SIZE));

	for(i=0; i<m; i++)
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

static long test_ioctl(struct file *filp, unsigned int cmd , unsigned long arg)
{
			printk("In IOCTL call\n");
        switch(cmd)
                {
                        case BUF_CNT:
                                       printk("Buffer count\n");
                                        put_user(CIRC_CNT(cbuf.head,cbuf.tail,SIZE),(int __user *)arg);
                                        break;
                        case BUF_CLR:
                                        if(!capable(CAP_SYS_ADMIN))
                                                return -EPERM;
                                          kfree(cbuf.buf);
					printk("Buffer is cleared\n");
      
                                        break;
                        case BUF_DISP:
                                        printk("\nThe data into Buffer is: %s\n",cbuf.buf);
                                        break;
                                        
                        default:
                                        printk("Not valid IOCTL\n");
                                        return -ENOTTY;
                 }
                 
return 0;
}

static struct file_operations fops={
                .owner = THIS_MODULE,
                .open = test_open,
                .release = test_close,
                .read = test_read,
                .write = test_write,
                .unlocked_ioctl = test_ioctl,
};


static __init int test_init(void)
{
        int ret;
        devno = MKDEV(MAJORNO,0);
		printk("In Init call\n");

        ret = register_chrdev_region(devno,NO_MINOR,"test_dev");

        if(ret){
                printk("Error registering device number\n");
                return ret;
                }
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


static __exit void test_exit(void){

	printk("In exit\n");
        cdev_del(&_cdev);
        unregister_chrdev_region(devno,NO_MINOR);
	kfree(cbuf.buf);
        }

module_init(test_init);
module_exit(test_exit);

