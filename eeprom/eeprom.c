
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	int fd;
	char buff[50] = {0x0};
	
	fd = open(argv[1],O_RDWR);
	if(fd < 0)
	{
		printf("errno=%d\n",errno); 
		printf("open %s error!\n",argv[1]);
		return -1;
	}
	
	write(fd,buff,50);
	
	close(fd);
	
	return 0;
}


