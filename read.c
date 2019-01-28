
#include<stdio.h>
#include<fcntl.h>

#define SIZE 20

int main()
{
        int fd,ret;
        char buf[SIZE];

        fd = open("/dev/circ_buf0", O_RDWR);

        if(fd<0)
        {
                perror("Error opening the device\n");
                return -1;
        }
        read(fd,buf,SIZE);

        printf("data read is %s\n",buf);
        return 0;
}








