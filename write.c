#include<stdio.h>
#include<fcntl.h>


int main()
{

        int fd,ret;
        char buf[]="Hello DESD\n";

        fd=open("/dev/nonblock0",O_RDWR);
        if(fd<0)
        {
                perror("Error operation\n");
        return -1;
        }

        ret = write(fd,buf,sizeof(buf));

	if(ret != sizeof(buf))
	{
		printf("Error writing to device\n");
	}
	else
		printf("Successfully written\n");

        return 0;
}


