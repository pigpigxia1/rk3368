#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include  <pthread.h>
#include <sys/select.h>
#include <time.h>
#include "rili/rili.h"
#include <string.h>

const char *Serial_Dev = "/dev/ttyS0";

typedef struct {
    char R_flag;
    char W_flag;
    int  len;
    char Data[255];
}Serial;

typedef struct {
    int Forward;
    int left;
    int rotate;
    unsigned char Check;
    char Enter[3];
}Vehicle;


Vehicle Serial_Tx = {0,0,0,0,{"\r\n"}};
Serial Serial_D = {0,0,0,{0}};
int S_fd;


int wait_flag = 0;
int one_pack_flag = 0;
int one_pack_num = 0;
int pack_num = 0;
int rev_num = 2;

int serial_send( int fd, char *Data );
int serial_sendx(int fd, unsigned char data[], int num);
int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);

void * Pthread_Serial_Rx( void *arg )
{
	unsigned char buff[512];
	int ret;
	int n;
	
	printf("start RX\n");
	while(1)
	{
		usleep(200000);
		ret = read_pack_rili(S_fd,buff,512);
		if(ret > 0)
		{
			printf("\nread len=%d\n",ret);
			for(n = 0; n < ret; n++)
			{
				printf("%x\t",buff[n]);
			}
		}
	}
}
void * Pthread_Serial_Tx( void *arg )
{
    int n=0;
    //int ret;
	fd_set readfs;
    struct termios oldstdio;
	struct timeval Timeout;
    unsigned char Rx_Data[100];
    unsigned char Tx_Data[100]={0x55,0x03,0x90,0xB6,0x01,0x00,0xAA};
    
    
	
	FD_ZERO(&readfs);
	FD_SET(S_fd, &readfs);
	Timeout.tv_usec = 0;
	Timeout.tv_sec  = 1;
	
	for(n = 2;n < 97;n++)
	{
		Tx_Data[n] = n;
	}
	Tx_Data[97] = 0x11;
	Tx_Data[1] = 96;
	Tx_Data[98] = xor_check(&Tx_Data[2],Tx_Data[1]);
	Tx_Data[99] = 0xAA;
	//printf("data xor = %x\n",Tx_Data[5]);
	/*printf("stop RF\n");
	Tx_Data[4] = 0x03;  //stop 
	serial_sendx(S_fd, Tx_Data, 7);
	for(n = 0; n < 7; n++)
	{
		printf("%x\t",Tx_Data[n]);
	}
	printf("\n");
	usleep(100000);
	
	printf("start RF\n");
	Tx_Data[4] = 0x01;  //start
	serial_sendx(S_fd, Tx_Data, 7);
	for(n = 0; n < 7; n++)
	{
		printf("%x\t",Tx_Data[n]);
	}
	printf("\n");*/
	usleep(100000);
    printf("start TX\n");
    while(1)
    {   
		serial_sendx(S_fd, Tx_Data, 100);
		usleep(200000);
		
		/*ret = read( S_fd, Rx_Data, 100);
		if(ret > 0)
		{
			printf("\nread len=%d\n",ret);
			for(n = 0; n < ret; n++)
			{
				printf("%x\t",Rx_Data[n]);
			}
		}*/
				
		 /*ret = select(S_fd+1, &readfs, NULL, NULL, &Timeout);
		if(ret > 0)
		{
			if (FD_ISSET(S_fd,&readfs))
			{
				
				ret = read( S_fd, Rx_Data, 100);
				if(ret > 0)
				{
					printf("\nread len=%d\n",ret);
					for(n = 0; n < ret; n++)
					{
						printf("%x\t",Rx_Data[n]);
					}
				}
				
			}
		} */
		//printf("end\n");
		
		//tcflush(S_fd,TCIOFLUSH);
        //ret = read( S_fd, Rx_Data, 100);
		//printf("read ret=%d\n",ret);
        /*if( ret >0 )
        {
			//printf("read data ret=%d\n",ret);
            Serial_D.len = ret;
            memset( Serial_D.Data, 0, Serial_D.len+3 );
            memcpy( Serial_D.Data, Rx_Data, Serial_D.len );        
            printf("\nread ret = %d %s ",ret,Serial_D.Data);
        }
        else
        {
            
            sprintf( Tx_Data,"send %d\r\n", n++ );
            serial_send( S_fd, Tx_Data );   
			usleep(100000);			
            //printf("send ok%d\r\n",n++);    
        }*/
		/*if( ret >0 )
        {
			printf("\nread data ret=%d\n",ret);
            //Serial_D.len = ret;
            //memset( Serial_D.Data, 0, Serial_D.len+3 );
            ///memcpy( Serial_D.Data, Rx_Data, Serial_D.len ); 
			for(n = 0; n < ret; n++)
			{
				printf("%x\t",Rx_Data[n]);
			}
				
        }
        else
        {
            //serial_sendx(S_fd, Tx_Data, 7);
            //sprintf( Tx_Data,"send %d\r\n", n++ );
            //serial_send( S_fd, Tx_Data );   
			//usleep(1000000);			
            //printf("send ok%d\r\n",n++);    
        }*/
    }
    pthread_exit(NULL);
}

int main()
{
#if 1
	int ret = 0;
    pthread_t pthread_id_rx;
	pthread_t pthread_id_tx;
	unsigned char Tx_Data[20]={0x80,0x05,0x90,0xB6,0x01,0x00,0x00};
	char *ptr;
	char c = 0x64;
    
	unsigned char test_buff[10] = {0x01,0x02,0x64,0x66,0x0};
	
	ptr = strchr(test_buff,0x64);
	//if(ptr)
		printf("test test_buff[] = %x\n",*ptr);
	//printf("test c = %c\n",(char)c);
	
	
	S_fd = open( Serial_Dev, O_RDWR);
    if( -1==S_fd ) 
        pthread_exit(NULL);
    printf("tty_fd = %d\n",S_fd);
    ret = set_opt(S_fd,38400,8,'N',1);
    if(ret == -1)
    {
         return -1;
    }
	
    //Create a thread
    pthread_create( &pthread_id_rx, NULL, &Pthread_Serial_Rx, NULL );
	pthread_create( &pthread_id_tx, NULL, &Pthread_Serial_Tx, NULL );
    usleep(1000);
	
	while(1)
	{
		
		sleep(1);
		//usleep(10000);
	}
	
#else
	
	int n=0;
    int ret;
	
	fd_set readfs;
	struct timeval Timeout;
	time_t now;
	
    unsigned char Rx_Data[30];
	unsigned char buff[30];
	unsigned char *pbuf = Rx_Data;
    unsigned char Tx_Data[20]={0x80,0x05,0x90,0xB6,0x01,0x00,0x00};
    
    S_fd = open( Serial_Dev, O_RDWR | O_NOCTTY |O_NDELAY);
    if( -1==S_fd ) 
	{
		printf("open device error!\n");
	}
        
    
    ret = set_opt(S_fd,115200,8,'N',1);
    if(ret == -1)
    {
         printf("set_opt error!\n");
    }
	
	Timeout.tv_usec = 0;
	Timeout.tv_sec  = 10;
	
	FD_ZERO(&readfs);
	FD_SET(S_fd, &readfs);
	
	printf("stop RF\n");
	Tx_Data[4] = 0x03;  //stop 
	serial_sendx(S_fd, Tx_Data, 7);
	usleep(100000);
	
	printf("start RF\n");
	Tx_Data[4] = 0x01;  //start
	serial_sendx(S_fd, Tx_Data, 7);
	
    printf("start\n");
    
    while(1)
    {	
		printf("select\n");
		ret = select(S_fd+1, &readfs, NULL, NULL, NULL);
		if(ret == 0)
		{
			printf("time out!\n");
			continue;
		}
		else if(ret < 0)
		{
			printf("select error!\n");
			
		}
		else
		{
			if (FD_ISSET(S_fd,&readfs))
			{
				
				ret = read( S_fd, buff, rev_num);
				printf("\nread data ret=%d\n",ret);
				/*if(ret == 0)
					continue;*/
			#if 1
				if(one_pack_flag == 0)
				{
					printf("one pack start\n");
					one_pack_flag = 1;
					memcpy( pbuf, buff,ret);
					if((ret >= 2)&&((buff[0] == 0xB0)|| (buff[0] == 0x90)))
					{
						rev_num = buff[1] + 2;
						one_pack_num = rev_num;
					}
					
					/*for(n = 0; n < ret; n++)
					{
						printf("%x\t",buff[n]);
					}*/
						
				}
				else
				{
					printf("read pack\n");
					/*for(n = 0; n < ret; n++)
					{
						printf("%x\t",buff[n]);
					}*/
					memcpy( pbuf, buff,ret);
				}
				pbuf += ret;
				rev_num -= ret;
				if(rev_num == 0)
				{
					time(&now);
					one_pack_flag = 0;
					rev_num = 2;
					pbuf = Rx_Data;
					pack_num ++;
					printf("\nread one pack end!send data pack_num = %d time=%d\n",pack_num,now);
					
					for(n = 0; n < one_pack_num; n++)
					{
						printf("%x\t",Rx_Data[n]);
					}
					if(Rx_Data[0] == 0xB0)
					{
						printf("stop RF\n");
						Tx_Data[4] = 0x03;  //stop 
						serial_sendx(S_fd, Tx_Data, 7);
					
						sleep(2);
						printf("start RF\n");
						Tx_Data[4] = 0x01;  //start
						serial_sendx(S_fd, Tx_Data, 7);
						
						
					}
				}
			#else
					
				if( ret >1 )
				{
					printf("\nread data ret=%d\n",ret);
					
					for(n = 0; n < ret; n++)
					{
						printf("%x\t",buff[n]);
					}
						
				}
				
			#endif
			}
		}
        usleep(10000);
        
    }
#endif
    return 0;
}

int serial_send( int fd, char *Data )
{
    int string_num;
    string_num = strlen(Data);
    return  write( fd,Data, string_num );
}

int serial_sendx(int fd, unsigned char data[], int num)
{
	return  write( fd,data, num );
	
}
int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop)
{
    struct termios newtio,oldtio;
    if(tcgetattr(fd,&oldtio)!=0)
    {
        perror("error:SetupSerial 3\n");
        return -1;
    }
    bzero(&newtio,sizeof(newtio));
    //使能串口接收
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    newtio.c_lflag &=~ICANON;//原始模式

    //newtio.c_lflag |=ICANON; //标准模式

    //设置串口数据位
    switch(nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |=CS8;
            break;
    }
    //设置奇偶校验位
    switch(nEvent)

    {
        case 'O':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N':
            newtio.c_cflag &=~PARENB;
            break;
    }
    //设置串口波特率
    switch(nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio,B2400);
            cfsetospeed(&newtio,B2400);
            break;
        case 4800:
            cfsetispeed(&newtio,B4800);
            cfsetospeed(&newtio,B4800);
            break;
        case 9600:
            cfsetispeed(&newtio,B9600);
            cfsetospeed(&newtio,B9600);
            break;
		case 19200:
            cfsetispeed(&newtio,B19200);
            cfsetospeed(&newtio,B19200);
            break;
		case 38400:
            cfsetispeed(&newtio,B38400);
            cfsetospeed(&newtio,B38400);
            break;
		case 57600:
            cfsetispeed(&newtio,B57600);
            cfsetospeed(&newtio,B57600);
            break;
        case 115200:
            cfsetispeed(&newtio,B115200);
            cfsetospeed(&newtio,B115200);
            break;
		case 230400:
            cfsetispeed(&newtio,B230400);
            cfsetospeed(&newtio,B230400);
            break;
		/* case 256000:
            cfsetispeed(&newtio,B256000);
            cfsetospeed(&newtio,B256000);
            break; */
        case 460800:
            cfsetispeed(&newtio,B460800);
            cfsetospeed(&newtio,B460800);
            break;
        default:
            cfsetispeed(&newtio,B9600);
            cfsetospeed(&newtio,B9600);
            break;
    }
    //设置停止位
    if(nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if(nStop == 2)
        newtio.c_cflag |= CSTOPB;
    newtio.c_cc[VTIME] = 1;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);

    if(tcsetattr(fd,TCSANOW,&newtio)!=0)
    {
        perror("com set error\n");
        return -1;
    }
    return 0;
}