#include<linux/init.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
#include<linux/circ_buf.h>

#define MAJORNO 42
#define NO_MINOR 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PP");

static dev_t devno;

static struct cdev _cdev;
static struct circ_buf cbuf;

int circ_buf_open(struct inode *inodp, struct file *filp)
{
        printk("In open call\n");
        return 0;
}

int circ_buf_close(struct inode *inodp, struct file *filp)
{
        printk("In close call\n");
        return 0;
}

ssize_t circ_buf_read(struct file *filp, char __user *ubuff,size_t cnt, loff_t *off)
{
	int i,ret;
        printk("In read system call\n");
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

ssize_t circ_buf_write(struct file *filp , const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i,ret;
        printk("In write system call\n");
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

	return 0;
}

static struct file_operations fops={
                .owner = THIS_MODULE,
                .open = circ_buf_open,
                .release = circ_buf_close,
                .read = circ_buf_read,
                .write = circ_buf_write,
};


static __init int circ_buf_init(void){
        int ret;
        devno = MKDEV(MAJORNO,0);

        ret = register_chrdev_region(devno,NO_MINOR,"testdev");

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
        cdev_init(&_cdev,&fops);

        ret = cdev_add(&_cdev, devno, NO_MINOR);
        if(ret){
                unregister_chrdev_region(devno,NO_MINOR);  
		kfree(cbuf.buf);
        return ret;
                }

        return 0;
}


static __exit void circ_buf_exit(void){

	printk("In exit\n");
        cdev_del(&_cdev);
        unregister_chrdev_region(devno,NO_MINOR);
	kfree(cbuf.buf);
        }

module_init(circ_buf_init);
module_exit(circ_buf_exit);

