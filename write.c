#include<stdio.h>
#include<fcntl.h>
#define SIZE 20

int main()
{

        int fd,ret;
        char buf[SIZE]="Hello DESD\n";

        fd=open("/dev/test0",O_RDWR);
        if(fd<0)
        {
                perror("Error operation\n");
        return -1;
        }

        ret = write(fd,buf,sizeof(buf));

	if(ret > 0)
		printf("Error writing to device\n");

	else
		printf("Successfully written\n");

        return 0;
}


