#include "gpio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define GPIO_EXPORT   "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
//#define GPIO_DIR       

int gpio_export(int gpio)
{
	//int ret;
	char buffer[64];
	int len;
	
	
	int fd = open(GPIO_EXPORT,O_WRONLY);
	if(fd < 0)
	{
		printf("open %s error\n",GPIO_EXPORT);
		return -1;
	}
	
	len = snprintf(buffer, sizeof(buffer), "%d", gpio);
	write(fd,buffer,len);
	
	close(fd);
	return 0;
	
}

int gpio_unexport(int gpio)
{
	char buffer[64];
	int len;
	
	int fd = open(GPIO_UNEXPORT,O_WRONLY);
	if(fd < 0)
	{
		printf("open %s error\n",GPIO_UNEXPORT);
		return -1;
	}
	
	len = snprintf(buffer, sizeof(buffer), "%d", gpio);
	write(fd,buffer,len);
	
	close(fd);
	return 0;
}

void gpio_setDir(int gpio,int dir)
{
	unsigned char buff[64];
	int fd;
	int ret;
	
	ret = snprintf(buff,sizeof(buff),"/sys/class/gpio/gpio%d/direction",gpio);
	fd = open(buff,O_WRONLY);
	if(fd < 0)
	{
		printf("open failed\n");
	}
	
	if(dir == 0)
	{
		write(fd,"in",2);
	}
	else
	{
		write(fd,"out",3);
	}
	
	close(fd);
	
}

void gpio_setValue(int gpio,int value)
{
	unsigned char buff[64];
	int fd;
	int ret;
	
	ret = snprintf(buff,sizeof(buff),"/sys/class/gpio/gpio%d/value",gpio);
	fd = open(buff,O_WRONLY);
	if(fd < 0)
	{
		printf("open failed\n");
	}
	
	if(value == 0)
	{
		write(fd,"0",1);
	}
	else
	{
		write(fd,"1",1);
	}
	
	close(fd);
	
}