#include <stdio.h>


void main
{
	unsigned char buff[4] = {0x11,0x22,0x33,0x44};

	printf("%d\n",*((unsigned int *)buff));
}
