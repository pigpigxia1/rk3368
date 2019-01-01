
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <unistd.h>
#include <stdlib.h>


int main ()  
{  
  //int keys_fd;  
  int led_fd;
  char ret[2];  
  //struct input_event t;  
  
  /* keys_fd = open ("/dev/input/event0", O_RDONLY);  
	if (keys_fd <= 0)  
	{  
	  printf ("open /dev/input/event2 device error!\n");  
	  return 0;  
	}   */
	led_fd = open ("/sys/class/led_gpio/light_led", O_WRONLY);  
	if (led_fd <= 0)  
	{  
	  printf ("open /sys/class/led_gpio/light_led device error!\n");  
	  return 0;  
	}  
  
	while (1)  
	{  
		write(led_fd,"g_off",strlen("g_off"));
		write(led_fd,"wh_on",strlen("wh_on"));
		sleep(10);
		write(led_fd,"g_on",strlen("g_on"));
		write(led_fd,"wh_off",strlen("wh_off"));
		sleep(10);
		/* if (read (keys_fd, &t, sizeof (t)) == sizeof (t))  
		{  
			if (t.type == EV_KEY)  
			{
				printf ("key %d %s\n", t.code,(t.value) ? "Pressed" : "Released");  
				if (t.value == 0 )  
				{  
					write(led_fd,"g_off",strlen("g_off"));
				  
				} 
				else
				{
					write(led_fd,"g_on",strlen("g_on"));
				}
			}
		}   */
	}  
  //close (keys_fd);  
  close(led_fd);
  
  return 0;  
}  
