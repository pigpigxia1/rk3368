
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "test.h"

char *test = "1234567890123012";


int test_eeprom(int argc, char *argv)
{
	int fd;
	int i;
	char buff[64] = {0x0};
	char buff1[64] = {0x0};
	int num = 0;
	int cur_position = 0;
	
	fd = open(argv,O_RDWR);
	if(fd < 0)
	{
		printf("errno=%d\n",errno); 
		printf("open %s error!\n",argv[1]);
		//log_write(unsigned char *str)
		return -1;
	}
	lseek(fd,16,SEEK_SET);
	for(i = 16;i < 256;i++)
	{
		write(fd,"0",1);
		
		usleep(100);
	}
	lseek(fd,16,SEEK_SET);
	for(i = 16;i < 256;i++)
	{
		write(fd,&i,1);
		
		usleep(100);
		
		/* lseek(fd,cur_position,SEEK_SET);
		//sprintf(buff,"%s%2d",test,i);
		write(fd,test,16);
		usleep(10000);
		cur_position += lseek(fd,0,SEEK_CUR);
		lseek(fd,cur_position-16,SEEK_SET);
		read(fd,buff1,16);
		if(!strncmp(test,buff1,16))
			log_write("eeprom test sucess\r\n");
		else
			log_write("eeprom test error\r\n"); */
		
	}
	lseek(fd,16,SEEK_SET);
	for(i = 16;i < 256;i++)
	{
		usleep(100);
		read(fd,buff1,1);
		if(buff1[0] == i){
			//log_write("eeprom test sucess\r\n");
		}
		else{
			sprintf(buff,"%s%2d","eeprom test error\r\n",i);
			log_write(buff);
			goto error_end;
		}
	}
	log_write("eeprom test sucess\r\n");
	
error_end:
	close(fd);
	
	return 0;
	
}


