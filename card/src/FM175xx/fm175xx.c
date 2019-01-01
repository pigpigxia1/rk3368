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
** File name:           type_a.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        ISO/IEC144443A卡相关操作
**
**--------------------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdint.h>
#include "fm175xx.h"
#include "Utility.h"
#include <string.h>    
#include <termios.h>
#include <strings.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#define GPIO_MAGIC    'O'
#define GPIO_REQUEST_NR    0x01
#define GPIO_SET_NR    0x02


#define GPIO_IN_CMD   _IOR(GPIO_MAGIC, GPIO_REQUEST_NR, char *)
#define GPIO_OUT_CMD   _IOW(GPIO_MAGIC, GPIO_REQUEST_NR, char *)
#define GPIO_SET_CMD   _IOW(GPIO_MAGIC, GPIO_SET_NR, char *)


#define MI_NOTAGERR 0xEE
#define MAXRLEN 64

#define REST_PIN   1019
#define PIN_CTR_DEV "/dev/gpio_ctr"

/*
void Delay100us(int time)
{
	usleep(100*time);
}*/


static void uart_send(int fd,unsigned char regdata)
{
	write( fd,&regdata, 1);
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


static unsigned char uart_read(int fd)
{
	unsigned char dat;
	int ret = -1;
	
	fd_set readfs;
	struct timeval Timeout;
	
	Timeout.tv_usec = 500000;
	Timeout.tv_sec  = 0;
	FD_ZERO(&readfs);
	FD_SET(fd, &readfs);
	
	ret = select(fd+1, &readfs, NULL, NULL, &Timeout);
	if(ret == 0)
	{
		printf("time out!\n");
			
	}
	else if(ret < 0)
	{
		printf("select error!\n");
		
	}
	else
	{
		if (FD_ISSET(fd,&readfs))
		{
			
			read( fd, &dat, 1);
			
		}
	}
	
	//serial_read(fd,&dat,1);
	
	return dat;
}


/*********************************************************************************************************
** Function name:       pcd_Init
** Descriptions:        MCU初始化函数、包括SPI和UART的初始化
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      
*********************************************************************************************************/
void pcd_Init(void)
{
    //SPI_Init();
    //CD_CfgTPD();                                                        /* 配置复位管脚                 */
	
	//rest_pin_init();
	//fd = uart_init(9600,8,'N',1);
	//return fd;
}

void pcd_DeInit()
{
	//gpio_unexport(REST_PIN);
	//close(fd);
}
static unsigned char GetReg(int fd,unsigned char Reg)
{
	unsigned char addr;
	
	addr = Reg | 0x80;
	uart_send(fd,addr);
	
	return uart_read(fd);
	
}

static unsigned char SetReg(int fd,unsigned char Reg, unsigned char value)
{
	unsigned char addr;
	
	addr = Reg & 0x7F;	
	uart_send(fd,addr);
	uart_read(fd);
	uart_send(fd,value);
	return 0;
}

void SetSpeed(int fd,int speed)
{
	switch(speed)
	{
		case 9600:
		
			break;
		case 115200:
			Write_Reg(fd,0x1F, 0x7a);     //0xcb 19200 0xAB 38400 0x7A 115200
			usleep(1000);
			set_opt(fd,115200,8,'N',1);
			break;
		default:
		break;
	}
}
/*********************************************************************************************************
** Function name:       MCU_TO_PCD_TEST
** Descriptions:        MCU与读卡芯片通信测试函数
**                      不同的读卡芯片的选取用于测试的寄存器不一样，需要更具具体芯片而确定目标寄存器
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      TRUE---测试通过    FALSE----测试失败
*********************************************************************************************************/
uint8_t MCU_TO_PCD_TEST(int fd)
{
    volatile uint8_t ucRegVal;
    pcd_RST(fd);                                                      /* 硬件复位                         */
    ucRegVal = GetReg(fd,ControlReg);
    SetReg(fd,ControlReg, 0x10);                                   /* 启动读写器模式                   */
    ucRegVal = GetReg(fd,ControlReg);
    SetReg(fd,GsNReg, 0xF0 | 0x04);                                /* CWGsN = 0xF; ModGsN = 0x4        */
    ucRegVal = GetReg(fd,GsNReg);
    if(ucRegVal != 0xF4)                                            /* 验证接口正确                     */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       pcd_RST
** Descriptions:        PCD控制PICC硬件复位函数
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      寄存器数值
*********************************************************************************************************/
void pcd_RST(int fd)
{
	
	/*gpio_setValue(REST_PIN,0);
	usleep(1000);
	gpio_setValue(REST_PIN,1);
	usleep(1000);*/
	
	int gpio_fd;
	
	set_opt(fd,9600,8,'N',1);
	
	gpio_fd = open(PIN_CTR_DEV,O_RDWR);
	if(gpio_fd < 0)
	{
		printf("%s:%d error!\n",__FUNCTION__,__LINE__);
		return;
	}
	if(ioctl(gpio_fd,GPIO_OUT_CMD,REST_PIN) < 0)
	{
		printf("REQUEST_GPIO_OUT error!\n");
		return ;
	}
	if(ioctl(gpio_fd,GPIO_SET_CMD,0) < 0)
	{
		printf("SET_GPIO error!\n");
		return ;
	}
	usleep(1000);
	if(ioctl(gpio_fd,GPIO_SET_CMD,1) < 0)
	{
		printf("SET_GPIO error!\n");
		return ;
	}
	usleep(1000);

	close(gpio_fd);
	
	return ;
}


/*********************************************************************************************************
** Function name:       Read_Reg
** Descriptions:        读取寄存器                
** input parameters:    reg_add:寄存器数值
** output parameters:   N/A
** Returned value:      寄存器数值
*********************************************************************************************************/
unsigned char Read_Reg(int fd,unsigned char reg_add)
{
    unsigned char  reg_value;       
    reg_value=GetReg(fd,reg_add);
    return reg_value;
}

/*********************************************************************************************************
** Function name:       Read_Reg_All
** Descriptions:        读取全部寄存器                
** input parameters:    reg_value:寄存器数值
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Read_Reg_All(int fd,unsigned char *reg_value)
{
    unsigned char data i;
    for (i=0;i<64;i++)       
        *(reg_value+i) = GetReg(fd,i);
    return TRUE;
}


/*********************************************************************************************************
** Function name:       Write_Reg
** Descriptions:        写寄存器操作                
** input parameters:    reg_add:寄存器地址
**                      reg_value:寄存器数值
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Write_Reg(int fd,unsigned char reg_add,unsigned char reg_value)
{
    SetReg(fd,reg_add,reg_value);
    return TRUE;
}


static void SPIRead_Sequence(int fd,unsigned char sequence_length,unsigned char ucRegAddr,unsigned char *reg_value)    
{
    uint8_t i;
    if (sequence_length==0)
		return;
    
    for(i=0;i<sequence_length;i++) {
        *(reg_value+i) = GetReg(fd,ucRegAddr);
    }
   
    return;

}

static void SPIWrite_Sequence(int fd,unsigned char sequence_length,unsigned char ucRegAddr,unsigned char *reg_value)
{
    
    uint8_t i;
    if(sequence_length==0)
        return;
    for(i=0;i<sequence_length;i++) {
        SetReg(fd,ucRegAddr, *(reg_value+i));
    }
 
    return ;    
}

/*********************************************************************************************************
** Function name:       Read_FIFO
** Descriptions:        读出FIFO的数据         
** input parameters:    length:读取数据长度
**                      *fifo_data:数据存放指针
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
void Read_FIFO(int fd,unsigned char length,unsigned char *fifo_data)
{     
    SPIRead_Sequence(fd,length,FIFODataReg,fifo_data);
    return;
}


/*********************************************************************************************************
** Function name:       Write_FIFO
** Descriptions:        写入FIFO         
** input parameters:    length:读取数据长度
**                      *fifo_data:数据存放指针
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
void Write_FIFO(int fd,unsigned char length,unsigned char *fifo_data)
{
    SPIWrite_Sequence(fd,length,FIFODataReg,fifo_data);
    return;
}


/*********************************************************************************************************
** Function name:       Clear_FIFO
** Descriptions:        清空FIFO              
** input parameters:   
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Clear_FIFO(int fd)
{
    Set_BitMask(fd,FIFOLevelReg,0x80);                                     /* 清除FIFO缓冲                 */
    if ( GetReg(fd,FIFOLevelReg) == 0 )
        return TRUE;
    else
        return FALSE;
}


/*********************************************************************************************************
** Function name:       Set_BitMask
** Descriptions:        置位寄存器操作    
** input parameters:    reg_add，寄存器地址
**                      mask，寄存器清除位
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Set_BitMask(int fd,unsigned char reg_add,unsigned char mask)
{
    unsigned char result;
    result=SetReg(fd,reg_add,Read_Reg(fd,reg_add) | mask);                /* set bit mask                 */
    return result;
}

/*********************************************************************************************************
** Function name:       Clear_BitMask
** Descriptions:        清除位寄存器操作
** input parameters:    reg_add，寄存器地址
**                      mask，寄存器清除位
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Clear_BitMask(int fd,unsigned char reg_add,unsigned char mask)
{
    unsigned char result;
    result=Write_Reg(fd,reg_add,Read_Reg(fd,reg_add) & ~mask);                /* clear bit mask               */
    return result;
}


/*********************************************************************************************************
** Function name:       Set_RF
** Descriptions:        设置射频输出
** input parameters:    mode，射频输出模式
**                      0，关闭输出
**                      1，仅打开TX1输出
**                      2，仅打开TX2输出
**                      3，TX1，TX2打开输出，TX2为反向输出
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Set_Rf(int fd,unsigned char mode)
{
    unsigned char result = 0;
    if( (Read_Reg(fd,TxControlReg)&0x03) == mode )
        return TRUE;
    if( mode == 0 )
        result = Clear_BitMask(fd,TxControlReg,0x03);                      /* 关闭TX1，TX2输出             */
    if( mode== 1 )
        result = Clear_BitMask(fd,TxControlReg,0x01);                      /* 仅打开TX1输出                */
    if( mode == 2)
        result = Clear_BitMask(fd,TxControlReg,0x02);                      /* 仅打开TX2输出                */
    if (mode==3)
        result=Set_BitMask(fd,TxControlReg,0x03);                          /* 打开TX1，TX2输出             */
    Delay100us(1000);
    return result;
}
 
/*********************************************************************************************************
** Function name:       Pcd_Comm
** Descriptions:        读卡器通信 不利用IRQ管脚的情况
** input parameters:    Command:通信操作命令
**                      pInData:发送数据数组
**                      InLenByte:发送数据数组字节长度
**                      pOutData:接收数据数组
**                      pOutLenBit:接收数据的位长度
** output parameters:   N/A
**                      flag:用于调试DEBUG，打印FIFO内容
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Pcd_Comm( int fd,   unsigned char Command, 
                        unsigned char *pInData, 
                        unsigned char InLenByte,
                        unsigned char *pOutData, 
                        unsigned int *pOutLenBit,int flag)
{
    uint8_t status  = FALSE;
    uint8_t irqEn   = 0x00;                                             /* 使能的中断                   */
    uint8_t waitFor = 0x00;                                             /* 等待的中断                   */
    uint8_t lastBits;
    uint8_t n;
    uint32_t i;
	unsigned char ret;
	unsigned char DEB[64];
    Write_Reg(fd,ComIrqReg, 0x7F);                                         /* 清楚IRQ标记                  */
    Write_Reg(fd,TModeReg,0x80);                                           /* 设置TIMER自动启动            */
    switch (Command) {
    case MFAuthent:                                                     /* Mifare认证                   */
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case Transceive:                                       /* 发送FIFO中的数据到天线，传输后激活接收电路*/
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }
   
    Write_Reg(fd,ComIEnReg, irqEn | 0x80);
   // Clear_BitMask(ComIrqReg, 0x80);
    Write_Reg(fd,CommandReg, Idle);
    Set_BitMask(fd,FIFOLevelReg, 0x80);
    
    for (i=0; i < InLenByte; i++) {
        Write_Reg(fd,FIFODataReg, pInData[i]);
    }
	
	if(flag)
	{
		printf("DEBUG Pcd_Comm\n");
		Read_FIFO(fd,InLenByte, DEB);
		for (i=0; i < InLenByte; i++) {
			
			printf("%x\t",DEB[i]);
		}
		printf("\n");
		return FALSE;
	}
    Write_Reg(fd,CommandReg, Command);

    if (Command == Transceive) {
        Set_BitMask(fd,BitFramingReg, 0x80);
    }

    i = 3000;                                              /* 根据时钟频率调整，操作M1卡最大等待时间25ms*/

    do {
        n = Read_Reg(fd,ComIrqReg);
        i--;                                                            /* i==0表示延时到了             */
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));            /* n&0x01!=1表示PCDsettimer时间未到 */
                                                                        /* n&waitFor!=1表示指令执行完成 */
    Clear_BitMask(fd,BitFramingReg, 0x80);
    if (i != 0) {
		ret = Read_Reg(fd,ErrorReg);
        if(!( ret& 0x1B)) {
			//printf("Read_Reg(fd,ErrorReg) = %x\n",ret);
			//printf("n = %x irqen = %x\n",n,irqEn);
            status = TRUE;
            if (n & irqEn & 0x01) {
				
                status = MI_NOTAGERR;
            }
            if (Command == Transceive) {
                n = Read_Reg(fd,FIFOLevelReg);
                lastBits = Read_Reg(fd,ControlReg) & 0x07;
                if (lastBits) {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                } else {
                    *pOutLenBit = n * 8;
                }
                if (n == 0) {
                    n = 1;
                }
                if (n > MAXRLEN) {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++) {
                    pOutData[i] = Read_Reg(fd,FIFODataReg);
                }
            }
        } else {
            status = FALSE;
        }
    }
       Clear_BitMask(fd,BitFramingReg,0x80);//关闭发送
    return status;
}


/*********************************************************************************************************
** Function name:       Pcd_SetTimer
** Descriptions:        设置接收延时
** input parameters:    delaytime，延时时间（单位为毫秒）  
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Pcd_SetTimer(int fd,unsigned long delaytime)
{
    unsigned long  TimeReload;
    unsigned int Prescaler;

    Prescaler=0;
    TimeReload=0;
    while(Prescaler<0xfff) {
        TimeReload = ((delaytime*(long)13560)-1)/(Prescaler*2+1);
        if( TimeReload<0xffff)
            break;
        Prescaler++;
    }
    TimeReload=TimeReload&0xFFFF;
    Set_BitMask(fd,TModeReg,Prescaler>>8);
    Write_Reg(fd,TPrescalerReg,Prescaler&0xFF);                    
    Write_Reg(fd,TReloadMSBReg,TimeReload>>8);
    Write_Reg(fd,TReloadLSBReg,TimeReload&0xFF);
    return TRUE;
}

/*********************************************************************************************************
** Function name:       Pcd_ConfigISOType
** Descriptions:        配置ISO14443A/B协议
** input parameters:    type = 0：ISO14443A协议；
**                                 type = 1，ISO14443B协议；   
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Pcd_ConfigISOType(int fd,unsigned char data type)
{
    if (type == 0)   {                                                  /* 配置为ISO14443_A             */
        Set_BitMask(fd,ControlReg, 0x10);                                /* ControlReg 0x0C 设置reader模式 */
        Set_BitMask(fd,TxAutoReg, 0x40);                                  /* TxASKReg 0x15 设置100%ASK有效 */
        Write_Reg(fd,TxModeReg, 0x00);                 /* TxModeReg 0x12 设置TX CRC无效，TX FRAMING =TYPE A */
        Write_Reg(fd,RxModeReg, 0x00);                 /* RxModeReg 0x13 设置RX CRC无效，RX FRAMING =TYPE A */
    }
    if (type == 1)   {                                                  /* 配置为ISO14443_B           */
        Write_Reg(fd,ControlReg,0x10);
        Write_Reg(fd,TxModeReg,0x83);                                      /* BIT1~0 = 2'b11:ISO/IEC 14443B */
        Write_Reg(fd,RxModeReg,0x83);                                      /* BIT1~0 = 2'b11:ISO/IEC 14443B */
        Write_Reg(fd,TxAutoReg,0x00);
        Write_Reg(fd,RxThresholdReg,0x55);
        Write_Reg(fd,RFCfgReg,0x48);
        Write_Reg(fd,TxBitPhaseReg,0x87);                                  /* 默认值                         */
        Write_Reg(fd,GsNReg,0x83);    
        Write_Reg(fd,CWGsPReg,0x30);
        Write_Reg(fd,GsNOffReg,0x38);
        Write_Reg(fd,ModGsPReg,0x20);
    
    }
    Delay100us(30);
    return TRUE;
}


/*********************************************************************************************************
** Function name:       FM175X_SoftReset
** Descriptions:        FM175xx软件复位
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char  FM175X_SoftReset(int fd)
{    
    Write_Reg(fd,CommandReg,SoftReset);
	usleep(1000);
	set_opt(fd,9600,8,'N',1);
    Set_BitMask(fd,ControlReg,0x10);                               /* 17520初始值配置              */
	SetSpeed(fd,115200);
	usleep(1000);
	
	return  TRUE  ;
}

/*********************************************************************************************************
** Function name:       FM175X_HardReset
** Descriptions:        FM175xx硬件复位
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char FM175X_HardReset(int fd)
{    
    /*gpio_setValue(REST_PIN,0);
	usleep(1000);
	gpio_setValue(REST_PIN,1);
	usleep(1000);*/
	int gpio_fd;
	
	set_opt(fd,9600,8,'N',1);
	gpio_fd = open(PIN_CTR_DEV,O_RDWR);
	if(gpio_fd < 0)
	{
		printf("%s:%d error!\n",__FUNCTION__,__LINE__);
		return FALSE;
	}
	if(ioctl(gpio_fd,GPIO_OUT_CMD,REST_PIN) < 0)
	{
		printf("REQUEST_GPIO_OUT error!\n");
		return FALSE;
	}
	if(ioctl(gpio_fd,GPIO_SET_CMD,0) < 0)
	{
		printf("SET_GPIO error!\n");
		return FALSE;
	}
	usleep(1000);
	if(ioctl(gpio_fd,GPIO_SET_CMD,1) < 0)
	{
		printf("SET_GPIO error!\n");
		return FALSE;
	}
	usleep(1000);

	close(gpio_fd);
    return TRUE;
}

    
/*********************************************************************************************************
** Function name:       FM175X_SoftPowerdown
** Descriptions:        硬件低功耗操作
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char FM175X_SoftPowerdown(int fd)
{
    if(Read_Reg(fd,CommandReg)&0x10) {
       Clear_BitMask(fd,CommandReg,0x10);                                 /* 退出低功耗模式               */
       return FALSE;
    }
    else
        Set_BitMask(fd,CommandReg,0x10);                                       /* 进入低功耗模式               */
    return TRUE;
}

/*********************************************************************************************************
** Function name:       FM175X_HardPowerdown
** Descriptions:        硬件低功耗操作
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char FM175X_HardPowerdown(void)
{    
    //NPD=~NPD;
    //if(NPD==1)                                                          /* 进入低功耗模式               */
    return TRUE;                                
//    else
        //return FALSE;                                                     /* 退出低功耗模式               */
}

/*********************************************************************************************************
** Function name:       Read_Ext_Reg
** Descriptions:        读取扩展寄存器
** input parameters:    reg_add，寄存器地址；         
** output parameters:   reg_value，寄存器数值
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Read_Ext_Reg(int fd,unsigned char reg_add)
{
     Write_Reg(fd,0x0F,0x80+reg_add);
     return Read_Reg(fd,0x0F);
}

/*********************************************************************************************************
** Function name:       Write_Ext_Reg
** Descriptions:        写入扩展寄存器
** input parameters:    reg_add，寄存器地址；
**                      reg_value，寄存器数值
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Write_Ext_Reg(int fd,unsigned char reg_add,unsigned char reg_value)
{
    Write_Reg(fd,0x0F,0x40+reg_add);
    return (Write_Reg(fd,0x0F,0xC0+reg_value));
}


