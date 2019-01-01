
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "test.h"

unsigned char *dev = "/sys/class/rtc/rtc0/date";
unsigned char *dev1 = "/sys/class/rtc/rtc0/time";

int test_rtc(int argc, char *argv[])
{
	int fd,fb_t;
	char buff[50] = {0x0};
	int n = 0;
	
	fd = open(dev,O_RDONLY);
	if(fd < 0)
	{
		printf("errno=%d\n",errno); 
		//printf("open %s error!\n",argv[1]);
		return -1;
	}
	
	fb_t = open(dev1,O_RDONLY);
	if(fb_t < 0)
	{
		printf("errno=%d\n",errno); 
		//printf("open %s error!\n",argv[1]);
		return -1;
	}
	while(1)
	{
		n = read(fd,buff,20);
		buff[n] = '\0',
		printf("rtc date:%s",buff);
		
		
		
		lseek(fd,0,SEEK_SET);
		usleep(500000);
		
	}
	close(fd);
	
	return 0;
}


