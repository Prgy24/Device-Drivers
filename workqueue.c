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
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/workqueue.h>


#define NO_MINOR 3
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PP");

static dev_t devno;

static struct t_dev{
	 struct cdev _cdev;
	 struct circ_buf cbuf;
	wait_queue_head_t wq;
}_tdev[NO_MINOR];

static struct work_struct w_struct;
static struct workqueue_struct *(w_q);

void work_handler(struct work_struct *w_wrk)
{
	printk("Our own workqueue\n");
}

int multi_dev_open(struct inode *inodp, struct file *filp)
{
	filp->private_data = container_of(inodp->i_cdev, struct t_dev, _cdev);
        printk("In open call\n");
        return 0;
}

int multi_dev_close(struct inode *inodp, struct file *filp)
{
        printk("In close call\n");
        return 0;
}

ssize_t multi_dev_read(struct file *filp, char __user *ubuff,size_t cnt, loff_t *off)
{
	int i,ret,m;
	
	struct t_dev *_tdev = filp->private_data;

        printk("In read system call\n");
	
	wait_event_interruptible(_tdev->wq,CIRC_CNT(_tdev->cbuf.head,_tdev->cbuf.tail,SIZE));

	m=min(cnt,CIRC_CNT(_tdev->cbuf.head,_tdev->cbuf.tail,SIZE));

	for(i=0; i<cnt; i++)
	{
		ret = copy_to_user(ubuff+i, _tdev->cbuf.buf+ _tdev->cbuf.tail,1);

		if(ret)
		{
			printk("Error copying\n");
		return -1;
		}

		_tdev->cbuf.tail = ((_tdev->cbuf.tail+1)&(SIZE-1));
	}		
        return i;
}

ssize_t multi_dev_write(struct file *filp , const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i,ret,m;

        printk("In write system call\n");
	
	struct t_dev *_tdev = filp->private_data;

	m= min(cnt,CIRC_SPACE(_tdev->cbuf.head,_tdev->cbuf.tail,SIZE));

	for(i=0; i<cnt; i++)
	{
		ret = copy_from_user(_tdev->cbuf.buf+_tdev->cbuf.head,ubuff+i,1);

		if(ret)
		{
			printk("Error copying\n");
		return -1;
		}

		_tdev->cbuf.head = (_tdev->cbuf.head+1)&(SIZE -1);
	}
	wake_up(&_tdev->wq);

	return 0;
}

static struct file_operations fops={
                .owner = THIS_MODULE,
                .open = multi_dev_open,
                .release = multi_dev_close,
                .read = multi_dev_read,
                .write = multi_dev_write,
};


static __init int multi_dev_init(void){
        int ret,i,j;
	dev_t tempdev;


	printk("In init\n");

         ret = alloc_chrdev_region(&devno,0,NO_MINOR,"test_dev");

        if(ret){
                printk("Error registering device number\n");
                return ret;
                }

	for(i=0; i<NO_MINOR; i++)
	{

		_tdev[i].cbuf.buf = kmalloc(SIZE,GFP_KERNEL);
		
		if(!_tdev[i].cbuf.buf)
		
		 goto cbuf_err;

			

	init_waitqueue_head(&_tdev[i].wq);
        cdev_init(&_tdev[i]._cdev,&fops);
        }
	
	for(j=0; j<NO_MINOR; j++)
	{
		tempdev=MKDEV(MAJOR(devno),MINOR(devno)+j);
        	ret = cdev_add(&_tdev[j]._cdev, tempdev, j);
        if(ret)

		goto cdev_err;
	}

	w_q = create_singlethread_workqueue("My Test Workqueue");
	INIT_WORK(&w_struct,work_handler);
	queue_work(w_q,&w_struct);
	return 0;
	
	cdev_err:
		for(--j;j>=0;j--)
		cdev_del(&_tdev[j]._cdev);

	cbuf_err:
		for(--i; i>=0; i--)
                
		kfree(_tdev[i].cbuf.buf);
                 unregister_chrdev_region(devno,NO_MINOR);
                 return 0;  

        
}


static __exit void multi_dev_exit(void)
{

	int i;
	printk("In exit\n");
	
	for(i=0; i<NO_MINOR; i++)
	{
        	
		kfree(_tdev[i].cbuf.buf);
                cdev_del(&_tdev[i]._cdev);

	}
		flush_workqueue(w_q);
		destroy_workqueue(w_q);
        	unregister_chrdev_region(devno,NO_MINOR);
	
}

module_init(multi_dev_init);
module_exit(multi_dev_exit);

