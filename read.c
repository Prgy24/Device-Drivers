
#include<stdio.h>
#include<fcntl.h>

#define SIZE 20

int main()
{
        int fd,ret;
        char buf[SIZE];

        fd = open("/dev/nonblock0", O_RDWR | O_NONBLOCK);

        if(fd<0)
        {
                perror("Error opening the device\n");
                return -1;
        }
        ret = read(fd,buf,SIZE);
	
	if(ret>=0)
	{
        	printf("data read is %s\n",buf);
        }
	else
		printf("Error in reading\n");
	
	return 0;
}








