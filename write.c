#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#define SIZE 20

int main()
{

        int fd,i,length;
        char buff[SIZE]; 
  
        fd=open("/dev/test0",O_RDWR);

	printf("Enter the message:");
	scanf("%[^\n]s",buff);

	printf("The message is:%s\n",buff);
	length = strlen(buff);

	printf("The size of the buffer is: %d\n",length);

        if(fd<0)
        {
                perror("Error operation\n");
        return -1;
        }

        write(fd,buff,length);
        return 0;
}


