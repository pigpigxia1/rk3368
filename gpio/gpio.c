
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>


#define GPIO_MAGIC    'O'
#define GPIO_REQUEST_NR    0x01
#define GPIO_SET_NR    0x02
#define GPIO_GET_NR    0x03


#define GPIO_IN_CMD   _IOR(GPIO_MAGIC, GPIO_REQUEST_NR, char *)
#define GPIO_OUT_CMD   _IOW(GPIO_MAGIC, GPIO_REQUEST_NR, char *)
#define GPIO_SET_CMD   _IOW(GPIO_MAGIC, GPIO_SET_NR, char *)
#define GPIO_GET_CMD   _IOR(GPIO_MAGIC, GPIO_GET_NR, char *)

#define GPIO7_A5  229
#define GPIO7_B0  232
#define GPIO0_D5  1029

int main(int argc, char *argv[])
{
	int fd;
	int cmd;
	int pin = 229;
	
	if(argc < 3)
	{
		return -1;
	}
	
	pin = atoi(argv[2]);
	
	fd = open(argv[1],O_RDWR);
	if(fd < 0)
	{
		printf("errno=%d\n",errno); 
		printf("open %s error!\n",argv[1]);
		return -1;
	}
	
	ioctl(fd,GPIO_OUT_CMD,pin);
	
	while(1)
	{
		scanf("%d",&cmd);
		if(cmd)
			ioctl(fd,GPIO_SET_CMD,1);
			//printf("GPIO0_D5 = %d\n",ioctl(fd,GPIO_SET_CMD,1));
		else
			ioctl(fd,GPIO_SET_CMD,0);
		
		usleep(500000);
		/*printf("enter 1 or 0:\n");
		scanf("%d",&cmd);
		
		if(cmd)
		{
			ioctl(fd,GPIO_SET_CMD,1);
		}
		else
		{
			ioctl(fd,GPIO_SET_CMD,0);
		}*/
	}
	
	return 0;
}


