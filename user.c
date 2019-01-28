
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<sys/ioctl.h>
#include<errno.h>
#include"ioctl_test.h"



int main()
{
        int fd,ret,count;
        

        fd = open("/dev/test0", O_RDWR);

        if(fd<0)
        {
                perror("Error opening the device\n");
                return -1;
        }
        write(fd,"Hello Desd",10);
        
        ioctl(fd,BUF_CNT, &count);
        printf("The count of the buffer is: %d\n",count);
        
        ioctl(fd, BUF_CLR);
        ioctl(fd, BUF_DISP);
        
        return 0;
}








