
#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include"ioctl_test.h"

#define SIZE 20

int main()
{
        int fd,ret,data;
        char buf[SIZE];

        fd = open("/dev/test0", O_RDWR);

        if(fd<0)
        {
                perror("Error opening the device\n");
                return -1;
        }
        read(fd,buf,SIZE);

        printf("data read is %s\n",buf);

	ioctl(fd, _CMD_TEST1);
	ioctl(fd, _CMD_TEST2, &data);
	printf("Data is: %d\n",data);
        return 0;
}








