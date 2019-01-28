#include<linux/init.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>


#define MAJORNO 42
#define NO_MINOR 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PP");

static dev_t devno;

static struct cdev _cdev;


ssize_t test_write(struct file *filp , const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i,ret;
	char *buff;

        printk("In write system call\n");

	buff = kmalloc(cnt,GFP_KERNEL);
	printk("The buffer size is : %d\n",cnt);

	for(i=0; i<cnt; i++)
	{
		ret = copy_from_user(buff+i,ubuff+i,1);


		if(ret)
		{
			kfree(buff);

		}

	}
		printk("The entered message is: %s\n",buff);

	return 0;
}

static struct file_operations fops={
                .owner = THIS_MODULE,
                .write = test_write,
};


static __init int test_init(void){
        int ret;
        devno = MKDEV(MAJORNO,0);

        ret = register_chrdev_region(devno,NO_MINOR,"testdev");

        if(ret){
                printk("Error registering device number\n");
                return ret;
                }
	
        cdev_init(&_cdev,&fops);

        ret = cdev_add(&_cdev, devno, NO_MINOR);
        if(ret){
                unregister_chrdev_region(devno,NO_MINOR);  
       return ret;
                }

        return 0;
}


static __exit void test_exit(void){

	printk("In exit\n");
        cdev_del(&_cdev);
        unregister_chrdev_region(devno,NO_MINOR);
        }

module_init(test_init);
module_exit(test_exit);

