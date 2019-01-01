/****************************************Copyright (c)****************************************************
**                            Guangzhou ZLGMCU Technology Co., LTD
**
**                                 http://www.zlgmcu.com
**
**      广州周立功单片机科技有限公司所提供的所有服务内容旨在协助客户加速产品的研发进度，在服务过程中所提供
**  的任何程序、文档、测试结果、方案、支持等资料和信息，都仅供参考，客户有权不使用或自行参考修改，本公司不
**  提供任何的完整性、可靠性等保证，若在客户使用过程中因任何原因造成的特别的、偶然的或间接的损失，本公司不
**  承担任何责任。
**                                                                        ——广州周立功单片机科技有限公司
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           main.c
** Last modified Date:  2016-3-4
** Last Version:        V1.00
** Descriptions:        用于演示读卡功能
**
**--------------------------------------------------------------------------------------------------------
*/


#include "fm175xx.h"
#include "type_a.h"
#include "type_b.h"
#include "cpu_card.h"
#include "Utility.h"
#include "mifare_card.h"
#include "des.h"
#include "card.h"
#include <time.h>
#include <sys/time.h>

#include "string.h"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>


const char *Serial_Dev = "/dev/ttyS1";

unsigned char new_key[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//unsigned char default_key[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char M1_buff[500] = {0x00,0x20,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xfe,0xff};
unsigned char CPU_buff[1024] = {0x00,0x64,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x00};


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
        case 115200:
            cfsetispeed(&newtio,B115200);
            cfsetospeed(&newtio,B115200);
            break;
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



int main(void)
{
#if 1
	int fd = -1;
	int i,n;
	int ret;
	int cmd;
	//clock_t t1,t2;
	struct timeval start,end;
	int len;
	unsigned char machine_type;
	unsigned char reg;
	
	unsigned char dat_buf[2048] = {0x0};
	unsigned char *Pbuf = NULL;
	

	fd = open( Serial_Dev, O_RDWR | O_NOCTTY |O_NDELAY);
    if( fd < 0 ) 
	{
		
		printf("open uart error!\n");
		return -1;
	}
	
	ret = set_opt(fd,9600,8,'N',1);  //设置串口
	if(ret < 0)
	{
		printf("set_opt failed!\n");
		return -1;
	}
	
    Delay100us(1000);
                   
    pcd_Init();                                                         // 读卡芯片初始化               
    
	
	//fd = uart_init(9600,8,'N',1);
	
	
	//cpu_test(fd);
	//machine_type = Check_Machine(fd);
	//printf("machine_type = %d\n",machine_type);
	
	//card_pthread_init(fd,new_key);
	//SetSpeed(fd,115200);
	
	/*printf("reg=%x\n",Read_Reg(fd,SerialSpeedReg));
	
	printf("enter card type:(1:M1 2:cpu)\n");
	scanf("%d",&cmd);
	
	if(cmd == 1)
	{
		n = M1_buff[0]*256 + M1_buff[1] - 1;
		for(i = 0;i < n;i++)
			M1_buff[i+2] = i;
		M1_buff[M1_buff[0]*256 + M1_buff[1] +1] = GetByteBCC(&M1_buff[2],M1_buff[0]*256 + M1_buff[1]-1);
		Pbuf = M1_buff;
	}
		
	else if(cmd == 2)
	{
		n = CPU_buff[0]*256 + CPU_buff[1] - 1;
		for(i = 0;i < n;i++)
			CPU_buff[i+2] = i;
		CPU_buff[CPU_buff[0]*256 + CPU_buff[1] +1] = GetByteBCC(&CPU_buff[2],CPU_buff[0]*256 + CPU_buff[1]-1);
		Pbuf = CPU_buff;
	}
	
	Write_Card_Info(fd,default_key,new_key,Pbuf);*/
	//fd = uart_init(9600,8,'N',1);
	while(1)
	{
		//TyteA_Test(fd);
		//TyteB_Test(fd);
		//t1 = clock();
		
		
		//fd = uart_init(9600,8,'N',1);
		gettimeofday(&start, NULL);
		len = Read_Card(fd,new_key,dat_buf,2048);
		//len = Get_Uid(fd,dat_buf,2048);
		gettimeofday(&end, NULL);
		//t2 = clock();
		//printf("total time = %dms\n",(end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);
		if(len  > 0)
		{
			printf("total time = %dms\n",(end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);
			Print(dat_buf,len);
		}
		//close(fd);
		//sleep(1);
			
	}
	
#else
		
	int ret,len,i;
	unsigned char buff[1024] = {0};
	unsigned char dat[16] = {0x87,0x87,0x87,0x87,0x87,0xf87,0x87,0x87};
	unsigned char key[16] = {0x0E,0x32,0x92,0x32,0xEA,0x6D,0x0D,0x73};
	
	
	len = strlen((char*)dat);
        for(i = 0;i < len;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        
        //DES ECB 加密
        printf("DES ECB ENC::\r\n");
        ret = des_ecb_encrypt(buff,dat,len,key);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");
        //DES ECB 解密
        printf("DES ECB DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des_ecb_decrypt(dat,buff,ret,key);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        //DES CBC 加密
        printf("DES CBC ENC::\r\n");
        memset(buff,0,sizeof(buff));
        des_cbc_encrypt(buff,dat,ret,key,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");

        //DES CBC 解密
        printf("DES CBC DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des_cbc_decrypt(dat,buff,ret,key,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");
        printf("\r\n");


        //3DES ECB 加密
        printf("3DES ECB ENC::\r\n");
        ret = des3_ecb_encrypt(buff,dat,len,key,16);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");
        //3DES ECB 解密
        printf("3DES ECB DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des3_ecb_decrypt(dat,buff,ret,key,16);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        //3DES CBC 加密
        printf("3DES CBC ENC::\r\n");
        memset(buff,0,sizeof(buff));
        des3_cbc_encrypt(buff,dat,ret,key,16,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");

        //3DES CBC 解密
        printf("3DES CBC DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des3_cbc_decrypt(dat,buff,ret,key,16,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");
		
#endif
	
                
    
	
	return 0;
}






