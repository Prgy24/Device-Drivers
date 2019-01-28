#include<linux/init.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/device.h>


#define NO_MINOR 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PP");

static dev_t devno;

static struct cdev _cdev;

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
        printk("In read system call\n");
        return 0;
}

ssize_t test_write(struct file *filp , const char __user *ubuff, size_t cnt, loff_t *off)
{
        printk("In write system call\n");
	return 0;
}

static struct file_operations fops={
                .owner = THIS_MODULE,
                .open = test_open,
                .release = test_close,
                .read = test_read,
                .write = test_write,
};


static __init int test_init(void){
        int ret;

        ret = alloc_chrdev_region(&devno,NO_MINOR,1,"test_dev");

        if(ret){
                printk("Error registering device number\n");
                return ret;
                }
	printk("We are in init function\n");

        cdev_init(&_cdev,&fops);

        ret = cdev_add(&_cdev, devno, NO_MINOR);
        if(ret){
                unregister_chrdev_region(devno,NO_MINOR);  
        return ret;
                }
        return 0;
}


static __exit void test_exit(void){
        cdev_del(&_cdev);
        unregister_chrdev_region(devno,NO_MINOR);
        }

module_init(test_init);
module_exit(test_exit);

