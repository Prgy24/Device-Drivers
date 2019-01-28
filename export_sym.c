#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("i am");

static int add(int a,int b)
{

 	return(a+b);
}
EXPORT_SYMBOL(add);

static int __init Hello_init(void)
{
	printk(KERN_ALERT "\n HELLO TO ALL \n\n");
        return 0;
}

static int __exit Hello_exit(void)
{
        printk(KERN_ALERT "\n BYE TO ALL \n\n");
        
}

module_init(Hello_init);
module_exit(Hello_exit);

