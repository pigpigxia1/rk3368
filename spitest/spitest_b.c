
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



static const char *device = "/dev/spidev2.0";
static uint32_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;


uint8_t default_tx[] = {
	0x5A, 0x01, 0xF1
	
};

uint8_t default_rx[50] = {0, };

static void pabort(const char *s)
{
	perror(s);
	abort();
}


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

/* uint8_t mcu_ctr(int fd,uint8_t cmd,uint8_t dat)
{
	uint8_t buff[64];
	
	buff[0] = cmd;
	switch(cmd)
	{
		case OPEN_DOOR:
			
		break;
		case GET_ALARM:
		
		break;
		case SET_MODE_485:
		
		break;
		case RS485_TX:
		
		break;
		case RS485_RX:
		
		break;
		case WG_OOUT:
		
		break;
		case GET_MAC_INFO:
		
		break;
	}
} */
 
 
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
		pabort("can't send spi message");

	hex_dump(rx, len, 32, "RX");
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
	unsigned char rx_buff[64];
	
	if(argc < 2)
	{
		return -1;
	}
	
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		pabort("can't open device");
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	mode |= SPI_CPHA;
	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");
	

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get RD_MODE hz");
	printf("spi RD_MODE: 0x%x\n", mode);
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't get RD_MODE hz");
	
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
			
				cmd_buf[1] = 2;
				cmd_buf[2] = OPEN_DOOR;
				cmd_buf[3] = 0x64;
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
				cmd_buf[1] = 1;
				cmd_buf[2] = GET_MAC_INFO;
				cmd_len = 3;
			break;
			case 8:
				cmd_buf[1] = 1;
				cmd_buf[2] = 0x33;
				cmd_len = 3;
			break;
			default:
				printf("cmd error!\n");
				return 0;
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
		read_info(fd,rx_buff,50);
		hex_dump(rx_buff, rx_buff[1] + 2, 32, "RX");
		
	#endif
		//transfer(fd, default_tx, default_rx, sizeof(default_tx));
		/* write(fd,default_tx,1);
		write(fd,&default_tx[1],2);
		
		read_info(fd,rx_buff,50);
		hex_dump(rx_buff, rx_buff[1] + 2, 32, "RX");
		sleep(1); */
		//close(fd);
	}
	
	return 0;
}

