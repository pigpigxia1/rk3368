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


#define OK 0
#define ERR -1

const char *Serial_Dev = "/dev/ttyS0";


static unsigned char Face_Sum_Check(unsigned char *pData,int len)
{
	int i;
	unsigned char temp=0;
	for ( i=0;i<len;i++ )
	{
		temp+=(*(pData+i));
	}

	if(temp==0)
	{
		return 1;
	}

	return 0;
}

static unsigned char CheckByteGet(unsigned char *inbuf,unsigned char len)
{
	unsigned char tmp,i;
	
	tmp = 0;

	for(i=0;i<len;i++)
	tmp += inbuf[i];

	tmp += 0xcc;
	tmp += 0xdd;
	
	i = (unsigned char)(256 - tmp);
	
	return i;
}


static int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop)
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
        case 115200:
            cfsetispeed(&newtio,B115200);
            cfsetospeed(&newtio,B115200);
            break;
        case 256000:
            cfsetispeed(&newtio,B256000);
            cfsetospeed(&newtio,B256000);
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
        return ERR;
    }
    return OK;
}

static int serial_sendx(int fd,unsigned char data[], int num)
{
	int send_num = num;
	unsigned char *pdat = data;
	int ret;
	
	do
	{
		ret = write( fd,pdat, send_num);
		send_num -= ret;
		pdat += ret;
		
	}while(send_num > 0);
	return OK ;
	
}

static int serial_read(int fd,unsigned char data[],int num,struct timeval *Timeout)
{
	int read_num = num;
	unsigned char *pdat = data;
	int ret;
	fd_set readfs;
	//struct timeval Timeout;
	
	//Timeout.tv_usec = 0;
	//Timeout.tv_sec  = 2;
	FD_ZERO(&readfs);
	FD_SET(fd, &readfs);
	
	do
	{
		ret = select(fd+1, &readfs, NULL, NULL, Timeout);
		if(ret == 0)
		{
			printf("time out!\n");
			//continue;
			return ERR;
		}
		else if(ret < 0)
		{
			printf("select error!\n");
			
		}
		else
		{
			if (FD_ISSET(fd,&readfs))
			{
				
				ret = read( fd, pdat, read_num);
				read_num -= ret;
				pdat += ret;
				//printf("serial_read ret=%d\n",ret);
			}
		}
	}while((read_num > 0)/* && (ret > 0)*/);
	//printf("read_num=%d\n",num);
	
	return num;
}


int write_pack(int fd,unsigned char *data,int len)
{
	//int pack_len = 0;
	unsigned char buff[512] = {0xaa,0xbb,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char tail[3] = {0x00,0xcc,0xdd};
	
	buff[2] = (unsigned char)((len + 11) / 256);
	buff[3] = (unsigned char)((len + 11) % 256);
	
	
	memcpy(buff + 8,data,len);
	
	//pack_len = len + 8;
	
	tail[0] = CheckByteGet(buff,len + 8);
	
	serial_sendx(fd,buff, len + 8);
	serial_sendx(fd,tail, 3);
	
	return 0;
}

static int read_pack(int fd,unsigned char *data,struct timeval *Timeout)
{
	int ret;
	//int one_pack_flag = 0;
	int one_pack_num = 0;
	int rev_num = 4;
	
	unsigned char buff[512];
	unsigned char *pbuf = buff;
	
	ret = serial_read(fd,pbuf, rev_num,Timeout);
	if((ret >= 4)&&((buff[0] == 0xaa)&& (buff[1] == 0xbb)))
	{
		//memcpy( pbuf, buff,ret);
		one_pack_num = buff[2] * 256 + buff[3];
		//one_pack_num = rev_num + 2;
	}
	else
	{
		printf("\nread pack failed!\n");
		return ERR;
	}
		
	pbuf += ret;
	rev_num = one_pack_num - ret;
	ret = serial_read(fd,pbuf, rev_num,Timeout);
	if(ret < 0)
	{
		return ERR;
	}
	
	
	if(Face_Sum_Check(buff,one_pack_num)
		&& (buff[one_pack_num-2] == 0xcc) 
		&& (buff[one_pack_num-1] == 0xdd))
	{
		memcpy( data, &buff[8],one_pack_num-11);
		return one_pack_num - 11;
	}
	
	return ERR;
}


int main()
{
	int enter;
	int i;
	int fd;
	unsigned char data[20];
	unsigned char cmd[3] = {0x02,0x01,0x01};
	int len;
	struct timeval Timeout;
	
	fd = open( Serial_Dev, O_RDWR | O_NOCTTY |O_NDELAY);
    if( -1==fd ) 
	{
		printf("open device error!\n");
	}
	
	set_opt(fd,256000,8,'N',1);
	
	Timeout.tv_usec = 0;
	Timeout.tv_sec  = 5;
	while(1)
	{
		printf("read data111\n");
		
		scanf("%d",&enter);
		sleep(1);
		len = read_pack(fd,data,&Timeout);
		if( len > 0)
		{
			printf("read sucess len=%d\n",len);
			cmd[2] = 0x00;
			write_pack(fd,cmd,3);
			for(i = 0; i < len;i++)
			{
				printf("%x\t",data[i]);
			}
			printf("\n");
		}
		else
		{
			printf("read failed\n");
			cmd[2] = 0x01;
			write_pack(fd,cmd,3);
		}
	}
}