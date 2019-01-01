
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "mcu_ctr.h"
#include "test.h"



static const char *device = "/dev/spidev2.0";
static uint32_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 100000;
static uint16_t delay;

static test_pthread mcu_ctr_pthread;

uint8_t log_buff[128];


static void hex_dump(const void *src, size_t length, size_t line_size, char *prefix)
{
	int i = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" | ");  /* right close */
			while (line < address) {
				c = *line++;
				printf("%c", (c < 33 || c == 255) ? 0x2E : c);
			}
			printf("\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}


int read_nbit(int fd,unsigned char *buff,int len)
{
	int ret = 0;
	int readn = len;
	
	do{
		buff += ret;
		ret = read(fd,buff,readn);
		if(ret <= 0)
		{
			printf("read error!\n");
			return -1;
		}
		readn -= ret; 
	}while(readn > 0);
	
	return len;
	
}

int read_info(int fd,unsigned char *buff,int len)
{
	int ret = 0;
	int readn = 2;
	
	if(!buff || len <= 2)
		return -1;
	
	ret = read_nbit(fd,buff,2);
	if(ret < 0)
	{
		printf("read_info error!\n");
		return -1;
	}
	
	if(len < buff[1] + 2)
		readn = len - 2;
	else
		readn = buff[1];
	buff += 2;
	ret = read_nbit(fd,buff,readn);
	if(ret < 0)
	{
		printf("read_info error!\n");
		return -1;
	}
	
	return readn + 2;
			
}
/*
int spi_write_read(int fd,uint8_t *cmd_buf,uint8_t *rx_buff,int len)
{
	write(fd,cmd_buf,cmd_buf[1] + 2);
	usleep(1000);
	read_info(fd,rx_buff,len);
}*/

static void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	 if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	} 

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		log_write("can't send spi message");

	//hex_dump(rx, len, 32, "RX");
}


 int spi_ctr(int fd,uint8_t *cmd,uint8_t *rx_buff)
{
	int cmd_len = 0;
	uint8_t cmd_buf[64] = {0x5A};
	
	switch(cmd[0])
	{
		case 0x01:
			cmd_buf[1] = 2;
			cmd_buf[2] = OPEN_DOOR;
			cmd_buf[3] = cmd[1];
			cmd_len = 4;
		break;
		case 0x02:
			cmd_buf[1] = 1;
			cmd_buf[2] = GET_ALARM;
			cmd_len = 3;
		break;
		case 0x03:
			cmd_buf[1] = 5;
			cmd_buf[2] = SET_MODE_485;
			cmd_buf[3] = 0x03;
			cmd_buf[4] = 0x0;
			cmd_buf[5] = 0x0;
			cmd_buf[6] = 0x01;
			cmd_len = 7;
		break;
		case 0x04:

			cmd_buf[1] = 4;
			cmd_buf[2] = RS485_TX;
			cmd_buf[3] = 0xAA;
			cmd_buf[4] = cmd[1];
			cmd_buf[5] = 0xBB;
			cmd_len = 6;
		break;
		case 0x05:
			cmd_buf[1] = 1;
			cmd_buf[2] = RS485_RX;
			cmd_len = 3;
		break;
		case 0x06:
			cmd_buf[1] = 6;
			cmd_buf[2] = WG_OOUT;
			cmd_buf[3] = cmd[1];
			cmd_buf[4] = 0x1;
			cmd_buf[5] = 0x2;
			cmd_buf[6] = 0x3;
			cmd_buf[7] = 0x04;
			cmd_len = 8;
			
		break;
		case 0x07:
			cmd_buf[1] = 4;
			cmd_buf[2] = WDOG_EN;
			cmd_buf[3] = 0x01;
			cmd_buf[4] = 0x00;
			cmd_buf[5] = cmd[1];
			
			cmd_len = 6;
		break;
		case 0x08:
			cmd_buf[1] = 1;
			cmd_buf[2] = WDOG_FEED;
			cmd_len = 3;
		break;
		case 0x09:
			cmd_buf[1] = 4;
			cmd_buf[2] = WDOG_EN;
			cmd_buf[3] = 0x00;
			cmd_buf[4] = 0x00;
			cmd_buf[5] = 0x00;
			
			cmd_len = 6;
		break;
		default:
			printf("reboot cpu!\n");
			//cmd_buf[1] = 1;
			//cmd_buf[2] = REBOOT;
			//cmd_len = 3;
	}
	
	//hex_dump(cmd_buf, cmd_len, 32, "TX");
	transfer(fd, cmd_buf, NULL, cmd_len);
	usleep(100 * 1000);
	//read_info(fd,rx_buff,50);
	
	transfer(fd, NULL, rx_buff, 20);
	//ret = read(fd,rx_buff,20);
	
	if((rx_buff[0]==0xA5)&&(rx_buff[3] == 0x0 || rx_buff[3] == 0x04))
	{	
		sprintf(log_buff,"spi send cmd %x sucess status:%x! date=%x %x %x %x",rx_buff[2],rx_buff[3],rx_buff[4],rx_buff[5],rx_buff[6],rx_buff[7]);
		log_write(log_buff);
		return 0;
	}
	
	
	sprintf(log_buff,"spi send cmd %x failed!",cmd_buf[2]);
	log_write(log_buff);
	return -1;
	//hex_dump(rx_buff, ret, 32, "RX");
} 
 
 
 
static void *test_spi(void *argv)
{
	int ret = 0;
	int fd;
	int alarm;
	uint8_t cmd[2];
	uint8_t rx_buff[64];
	char *dev = (char *)argv;
	
	printf("spi dev = %s\n",dev);
	fd = open(dev, O_RDWR);
	if (fd < 0){
		log_write("can't open device");
		return (void *)-1;
	}
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1){
		log_write("can't get spi mode");
		return (void *)-1;
	}

	mode |= SPI_CPHA;
	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1){
		log_write("can't get spi mode");
		return (void *)-1;
	}
	

	
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1){
		log_write("can't set bits per word");
		return (void *)-1;
	}

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1){
		log_write("can't get bits per word");
		return (void *)-1;
	}

	
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1){
		log_write("can't set max speed hz");
		return (void *)-1;
	}

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1){
		log_write("can't get max speed hz");
		return (void *)-1;
	}

	/*ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1){
		log_write("can't get RD_MODE hz");
		return -1;
	}
	
	printf("spi RD_MODE: 0x%x\n", mode);
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		log_write("can't get RD_MODE hz");*/
	
	while(mcu_ctr_pthread.run)
	{
		//test 485
		cmd[0] = 0x04;
		cmd[1] = 0x06;
		ret = spi_ctr(fd,cmd,rx_buff);
		usleep(100000);
		cmd[0] = 0x05;
		ret = spi_ctr(fd,cmd,rx_buff);
		if(ret == 0 && rx_buff[4] == 0x06)
		{
			sprintf(log_buff,"spi test 485 sucess!");
			log_write(log_buff);
		}
		else
		{
			sprintf(log_buff,"spi test 485 failed!");
			log_write(log_buff);
		}
		
		//test open_door
		cmd[0] = 0x01;
		cmd[1] = 0x0a;
		spi_ctr(fd,cmd,rx_buff);
		usleep(100000);
		cmd[0] = 0x04;
		cmd[1] = 0x05;
		ret = spi_ctr(fd,cmd,rx_buff);
		usleep(100000);
		cmd[0] = 0x05;
		ret = spi_ctr(fd,cmd,rx_buff);
		if(ret == 0 && rx_buff[4] != 0)
		{
			sprintf(log_buff,"spi test open_door sucess!");
			log_write(log_buff);
		}
		else
		{
			sprintf(log_buff,"spi test open_door failed!");
			log_write(log_buff);
		}
		
		//test wg
		cmd[0] = 0x04;
		cmd[1] = 0x01;
		ret = spi_ctr(fd,cmd,rx_buff);
		usleep(100000);
		cmd[0] = 0x06;
		cmd[1] = 0x0;
		spi_ctr(fd,cmd,rx_buff);
		usleep(300000);
		cmd[0] = 0x05;
		ret = spi_ctr(fd,cmd,rx_buff);
		if((ret == 0) && (rx_buff[4] == 0x04)
			&&(rx_buff[5] == 0x03)
			&&(rx_buff[6] == 0x02))
		{
			sprintf(log_buff,"spi test wg sucess!");
			log_write(log_buff);
		}
		else
		{
			sprintf(log_buff,"spi test wg failed!");
			log_write(log_buff);
		}
		
		//alarm
		alarm = 0;
		cmd[0] = 0x04;
		cmd[1] = 0x03;
		ret = spi_ctr(fd,cmd,rx_buff);
		usleep(100000);
		cmd[0] = 0x02;
		ret = spi_ctr(fd,cmd,rx_buff);
		if((ret == 0) && (rx_buff[5] == 0x0) && (rx_buff[6] == 0x0))
		{
			alarm = 1;
		}
		sprintf(log_buff,"spi test alarm status %x %x !",rx_buff[5],rx_buff[6]);
		log_write(log_buff);
		cmd[0] = 0x04;
		cmd[1] = 0x04;
		ret = spi_ctr(fd,cmd,rx_buff);
		usleep(100000);
		cmd[0] = 0x02;
		ret = spi_ctr(fd,cmd,rx_buff);
		if((ret == 0) && (rx_buff[5] == 0x01) && (rx_buff[6] == 0x01) && alarm)
		{
			sprintf(log_buff,"spi test alarm sucess!");
			log_write(log_buff);
		}
		else
		{
			sprintf(log_buff,"spi test alarm failed!status %x %x",rx_buff[5],rx_buff[6]);
			log_write(log_buff);
		}
		
		
		sleep(1);
		
		
		
	}
	
	return (void *)0;
} 

void mcu_test(void *argv)
{
	int ret;
	if(mcu_ctr_pthread.run == 1)
	{
		printf("card_pthread is running!\n");
		return ;
	}
	
	mcu_ctr_pthread.run = 1;
	pthread_mutex_init(&mcu_ctr_pthread.mutex,NULL);
	ret = pthread_create(&mcu_ctr_pthread.pid,NULL,test_spi,argv);
	if(ret != 0)
	{
		mcu_ctr_pthread.run = 0;
	}
	
	return;
}
void mcu_test_close()
{
	pthread_mutex_destroy(&mcu_ctr_pthread.mutex);
	mcu_ctr_pthread.run = 0;
}
 /* unsigned char *help_string = "\n1:open door\n \
						2:get alarm\n  \
						3:set 485 mode\n \
						4:rs485_tx\n \
						5:rs485_rx\n \
						6:wg_out\n \
						7:get_machine_info\n \
						please enter cmd\n \
						"; */
/*						
int main(int argc,char *argv[])
{
	int ret = 0;
	int fd;
	int cmd;
	int wg_mode = 0;
	int mdelay = 50;
	int cmd_len = 0;
	uint8_t cmd_buf[10] = {0x5A};
	
	//uint8_t *tx;
	//uint8_t *rx;
	//int size;
	
	if(argc < 2)
	{
		return -1;
	}
	
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		log_write("can't open device");
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		log_write("can't get spi mode");

	mode |= SPI_CPHA;
	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		log_write("can't get spi mode");
	

	
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		log_write("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		log_write("can't get bits per word");

	
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		log_write("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		log_write("can't get max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		log_write("can't get RD_MODE hz");
	printf("spi RD_MODE: 0x%x\n", mode);
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		log_write("can't get RD_MODE hz");
	
	printf("spi WR_MODE: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
	
	while(1)
	{
		//printf(help_string);
		printf("enter cmd and delay time ms\n");
		scanf("%d %d",&cmd,&mdelay);
		
		
		switch(cmd)
		{
			case 1:
				printf("enter delay time\n");
				scanf("%d",&cmd);
				printf("delay = %x\n",cmd);
				cmd_buf[1] = 2;
				cmd_buf[2] = OPEN_DOOR;
				cmd_buf[3] = cmd;
				cmd_len = 4;
			break;
			case 2:
				cmd_buf[1] = 1;
				cmd_buf[2] = GET_ALARM;
				cmd_len = 3;
			break;
			case 3:
				cmd_buf[1] = 5;
				cmd_buf[2] = SET_MODE_485;
				cmd_buf[3] = 0x03;
				cmd_buf[4] = 0x0;
				cmd_buf[5] = 0x0;
				cmd_buf[6] = 0x01;
				cmd_len = 7;
			break;
			case 4:
				printf("enter 485 cmd\n");
				scanf("%d",&cmd);
				printf("cmd = %x\n",cmd);
				cmd_buf[1] = 4;
				cmd_buf[2] = RS485_TX;
				cmd_buf[3] = 0xAA;
				cmd_buf[4] = cmd;
				cmd_buf[5] = 0xBB;
				cmd_len = 6;
			break;
			case 5:
				cmd_buf[1] = 1;
				cmd_buf[2] = RS485_RX;
				cmd_len = 3;
			break;
			case 6:
			
				printf("enter wg cmd\n");
				scanf("%d %d",&wg_mode,&cmd);
				printf("wg_mode = %d,cmd = %x\n",wg_mode,cmd);
				cmd_buf[1] = 6;
				cmd_buf[2] = WG_OOUT;
				cmd_buf[3] = wg_mode;
				cmd_buf[4] = 0x0;
				cmd_buf[5] = 0x0;
				cmd_buf[6] = 0x0;
				cmd_buf[7] = cmd;
				cmd_len = 8;
				
			break;
			case 7:
				printf("enter wdog time(s)\n");
				scanf("%d",&cmd);
				printf("time = %x\n",cmd);
				cmd_buf[1] = 4;
				cmd_buf[2] = WDOG_EN;
				cmd_buf[3] = 0x01;
				cmd_buf[4] = 0x00;
				cmd_buf[5] = cmd;
				
				cmd_len = 6;
			break;
			case 8:
				cmd_buf[1] = 1;
				cmd_buf[2] = WDOG_FEED;
				cmd_len = 3;
			break;
			case 9:
				cmd_buf[1] = 4;
				cmd_buf[2] = WDOG_EN;
				cmd_buf[3] = 0x00;
				cmd_buf[4] = 0x00;
				cmd_buf[5] = 0x00;
				
				cmd_len = 6;
			default:
				printf("reboot cpu!\n");
				cmd_buf[1] = 1;
				cmd_buf[2] = REBOOT;
				cmd_len = 3;
		}
	#if 0
		write(fd,cmd_buf,cmd_len);
		usleep(mdelay * 1000);
		//sleep(1);
		read_info(fd,rx_buff,50);
		hex_dump(rx_buff, rx_buff[1] + 2, 32, "RX");
		
	#else
		hex_dump(cmd_buf, cmd_len, 32, "TX");
		transfer(fd, cmd_buf, default_rx, cmd_len);
		usleep(mdelay * 1000);
		//read_info(fd,rx_buff,50);
		ret = read(fd,rx_buff,20);
		hex_dump(rx_buff, ret, 32, "RX");
		
	#endif
		
		//close(fd);
	}
	
	return 0;
}
*/
