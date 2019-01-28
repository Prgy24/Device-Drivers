#include<stdio.h>
#include<fcntl.h>
#define SIZE 20

int main()
{

        int fd,ret;
        char buf[SIZE]="Hello DESD\n";

        fd=open("/dev/wait_queue0",O_RDWR);
        if(fd<0)
        {
                perror("Error operation\n");
        return -1;
        }

        write(fd,buf,sizeof(buf));
        return 0;
}


