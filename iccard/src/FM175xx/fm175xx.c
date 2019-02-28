/****************************************Copyright (c)****************************************************
**                            Guangzhou ZLGMCU Technology Co., LTD
**
**                                 http://www.zlgmcu.com
**
**      ������������Ƭ���Ƽ����޹�˾���ṩ�����з�������ּ��Э���ͻ����ٲ�Ʒ���з����ȣ��ڷ�����������ṩ
**  ���κγ����ĵ������Խ����������֧�ֵ����Ϻ���Ϣ���������ο����ͻ���Ȩ��ʹ�û����вο��޸ģ�����˾��
**  �ṩ�κε������ԡ��ɿ��Եȱ�֤�����ڿͻ�ʹ�ù��������κ�ԭ����ɵ��ر�ġ�żȻ�Ļ��ӵ���ʧ������˾��
**  �е��κ����Ρ�
**                                                                        ����������������Ƭ���Ƽ����޹�˾
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           type_a.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        ISO/IEC144443A����ز���
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
#include "uart.h"


#define GPIO_MAGIC    'O'
#define GPIO_REQUEST_NR    0x01
#define GPIO_SET_NR    0x02


#define GPIO_IN_CMD   _IOR(GPIO_MAGIC, GPIO_REQUEST_NR, char *)
#define GPIO_OUT_CMD   _IOW(GPIO_MAGIC, GPIO_REQUEST_NR, char *)
#define GPIO_SET_CMD   _IOW(GPIO_MAGIC, GPIO_SET_NR, char *)


#define MI_NOTAGERR 0xEE
#define MAXRLEN 64

//#define REST_PIN   229
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
/*
static int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop)
{
    struct termios newtio,oldtio;
    if(tcgetattr(fd,&oldtio)!=0)
    {
        perror("error:SetupSerial 3\n");
        return -1;
    }
    bzero(&newtio,sizeof(newtio));
    //ʹ�ܴ��ڽ���
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    newtio.c_lflag &=~ICANON;//ԭʼģʽ

    //newtio.c_lflag |=ICANON; //��׼ģʽ

    //���ô�������λ
    switch(nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |=CS8;
            break;
    }
    //������żУ��λ
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
    //���ô��ڲ�����
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
    //����ֹͣλ
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
*/

static unsigned char uart_read(int fd)
{
	unsigned char dat;
	int ret = -1;
	
	fd_set readfs;
	struct timeval Timeout;
	
	Timeout.tv_usec = 5000;
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
			
			if(read( fd, &dat, 1) < 0)
				return 0;
			
		}
	}
	
	//serial_read(fd,&dat,1);
	
	return dat;
}

static unsigned char uart_readbyte(int fd)
{
	unsigned char dat = 0;
	int ret = -1;
	
	ret = read( fd, &dat, 1);
	if(ret < 0)
		return -1;
	
	return dat;
}

/*********************************************************************************************************
** Function name:       pcd_Init
** Descriptions:        MCU��ʼ������������SPI��UART�ĳ�ʼ��
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      
*********************************************************************************************************/
void pcd_Init(void)
{
    //SPI_Init();
    //CD_CfgTPD();                                                        /* ���ø�λ�ܽ�                 */
	
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
** Descriptions:        MCU�����оƬͨ�Ų��Ժ���
**                      ��ͬ�Ķ���оƬ��ѡȡ���ڲ��ԵļĴ�����һ������Ҫ���߾���оƬ��ȷ��Ŀ��Ĵ���
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      TRUE---����ͨ��    FALSE----����ʧ��
*********************************************************************************************************/
uint8_t MCU_TO_PCD_TEST(int fd,int gpio)
{
    volatile uint8_t ucRegVal;
    pcd_RST(fd,gpio);                                                      /* Ӳ����λ                         */
    ucRegVal = GetReg(fd,ControlReg);
    SetReg(fd,ControlReg, 0x10);                                   /* ������д��ģʽ                   */
    ucRegVal = GetReg(fd,ControlReg);
    SetReg(fd,GsNReg, 0xF0 | 0x04);                                /* CWGsN = 0xF; ModGsN = 0x4        */
    ucRegVal = GetReg(fd,GsNReg);
    if(ucRegVal != 0xF4)                                            /* ��֤�ӿ���ȷ                     */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       pcd_RST
** Descriptions:        PCD����PICCӲ����λ����
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      �Ĵ�����ֵ
*********************************************************************************************************/
void pcd_RST(int fd,int gpio)
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
	if(ioctl(gpio_fd,GPIO_OUT_CMD,gpio) < 0)
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
** Descriptions:        ��ȡ�Ĵ���                
** input parameters:    reg_add:�Ĵ�����ֵ
** output parameters:   N/A
** Returned value:      �Ĵ�����ֵ
*********************************************************************************************************/
unsigned char Read_Reg(int fd,unsigned char reg_add)
{
    unsigned char  reg_value;       
    reg_value=GetReg(fd,reg_add);
    return reg_value;
}

/*********************************************************************************************************
** Function name:       Read_Reg_All
** Descriptions:        ��ȡȫ���Ĵ���                
** input parameters:    reg_value:�Ĵ�����ֵ
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
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
** Descriptions:        д�Ĵ�������                
** input parameters:    reg_add:�Ĵ�����ַ
**                      reg_value:�Ĵ�����ֵ
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
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
** Descriptions:        ����FIFO������         
** input parameters:    length:��ȡ���ݳ���
**                      *fifo_data:���ݴ��ָ��
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
void Read_FIFO(int fd,unsigned char length,unsigned char *fifo_data)
{     
    SPIRead_Sequence(fd,length,FIFODataReg,fifo_data);
    return;
}


/*********************************************************************************************************
** Function name:       Write_FIFO
** Descriptions:        д��FIFO         
** input parameters:    length:��ȡ���ݳ���
**                      *fifo_data:���ݴ��ָ��
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
void Write_FIFO(int fd,unsigned char length,unsigned char *fifo_data)
{
    SPIWrite_Sequence(fd,length,FIFODataReg,fifo_data);
    return;
}


/*********************************************************************************************************
** Function name:       Clear_FIFO
** Descriptions:        ���FIFO              
** input parameters:   
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Clear_FIFO(int fd)
{
    Set_BitMask(fd,FIFOLevelReg,0x80);                                     /* ���FIFO����                 */
    if ( GetReg(fd,FIFOLevelReg) == 0 )
        return TRUE;
    else
        return FALSE;
}


/*********************************************************************************************************
** Function name:       Set_BitMask
** Descriptions:        ��λ�Ĵ�������    
** input parameters:    reg_add���Ĵ�����ַ
**                      mask���Ĵ������λ
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Set_BitMask(int fd,unsigned char reg_add,unsigned char mask)
{
    unsigned char result;
    result=SetReg(fd,reg_add,Read_Reg(fd,reg_add) | mask);                /* set bit mask                 */
    return result;
}

/*********************************************************************************************************
** Function name:       Clear_BitMask
** Descriptions:        ���λ�Ĵ�������
** input parameters:    reg_add���Ĵ�����ַ
**                      mask���Ĵ������λ
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Clear_BitMask(int fd,unsigned char reg_add,unsigned char mask)
{
    unsigned char result;
    result=Write_Reg(fd,reg_add,Read_Reg(fd,reg_add) & ~mask);                /* clear bit mask               */
    return result;
}


/*********************************************************************************************************
** Function name:       Set_RF
** Descriptions:        ������Ƶ���
** input parameters:    mode����Ƶ���ģʽ
**                      0���ر����
**                      1������TX1���
**                      2������TX2���
**                      3��TX1��TX2�������TX2Ϊ�������
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Set_Rf(int fd,unsigned char mode)
{
    unsigned char result = 0;
    if( (Read_Reg(fd,TxControlReg)&0x03) == mode )
        return TRUE;
    if( mode == 0 )
        result = Clear_BitMask(fd,TxControlReg,0x03);                      /* �ر�TX1��TX2���             */
    if( mode== 1 )
        result = Clear_BitMask(fd,TxControlReg,0x01);                      /* ����TX1���                */
    if( mode == 2)
        result = Clear_BitMask(fd,TxControlReg,0x02);                      /* ����TX2���                */
    if (mode==3)
        result=Set_BitMask(fd,TxControlReg,0x03);                          /* ��TX1��TX2���             */
    Delay100us(1000);
    return result;
}
 
/*********************************************************************************************************
** Function name:       Pcd_Comm
** Descriptions:        ������ͨ�� ������IRQ�ܽŵ����
** input parameters:    Command:ͨ�Ų�������
**                      pInData:������������
**                      InLenByte:�������������ֽڳ���
**                      pOutData:������������
**                      pOutLenBit:�������ݵ�λ����
** output parameters:   N/A
**                      flag:���ڵ���DEBUG����ӡFIFO����
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Pcd_Comm( int fd,   unsigned char Command, 
                        unsigned char *pInData, 
                        unsigned char InLenByte,
                        unsigned char *pOutData, 
                        unsigned int *pOutLenBit,int flag)
{
    uint8_t status  = FALSE;
    uint8_t irqEn   = 0x00;                                             /* ʹ�ܵ��ж�                   */
    uint8_t waitFor = 0x00;                                             /* �ȴ����ж�                   */
    uint8_t lastBits;
    uint8_t n;
    uint32_t i;
	unsigned char ret;
	unsigned char DEB[64];
    Write_Reg(fd,ComIrqReg, 0x7F);                                         /* ���IRQ���                  */
    Write_Reg(fd,TModeReg,0x80);                                           /* ����TIMER�Զ�����            */
    switch (Command) {
    case MFAuthent:                                                     /* Mifare��֤                   */
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case Transceive:                                       /* ����FIFO�е����ݵ����ߣ�����󼤻���յ�·*/
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

    i = 3000;                                              /* ����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms*/

    do {
        n = Read_Reg(fd,ComIrqReg);
        i--;                                                            /* i==0��ʾ��ʱ����             */
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));            /* n&0x01!=1��ʾPCDsettimerʱ��δ�� */
                                                                        /* n&waitFor!=1��ʾָ��ִ����� */
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
       Clear_BitMask(fd,BitFramingReg,0x80);//�رշ���
    return status;
}


/*********************************************************************************************************
** Function name:       Pcd_SetTimer
** Descriptions:        ���ý�����ʱ
** input parameters:    delaytime����ʱʱ�䣨��λΪ���룩  
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
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
** Descriptions:        ����ISO14443A/BЭ��
** input parameters:    type = 0��ISO14443AЭ�飻
**                                 type = 1��ISO14443BЭ�飻   
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Pcd_ConfigISOType(int fd,unsigned char data type)
{
    if (type == 0)   {                                                  /* ����ΪISO14443_A             */
        Set_BitMask(fd,ControlReg, 0x10);                                /* ControlReg 0x0C ����readerģʽ */
        Set_BitMask(fd,TxAutoReg, 0x40);                                  /* TxASKReg 0x15 ����100%ASK��Ч */
        Write_Reg(fd,TxModeReg, 0x00);                 /* TxModeReg 0x12 ����TX CRC��Ч��TX FRAMING =TYPE A */
        Write_Reg(fd,RxModeReg, 0x00);                 /* RxModeReg 0x13 ����RX CRC��Ч��RX FRAMING =TYPE A */
    }
    if (type == 1)   {                                                  /* ����ΪISO14443_B           */
        Write_Reg(fd,ControlReg,0x10);
        Write_Reg(fd,TxModeReg,0x83);                                      /* BIT1~0 = 2'b11:ISO/IEC 14443B */
        Write_Reg(fd,RxModeReg,0x83);                                      /* BIT1~0 = 2'b11:ISO/IEC 14443B */
        Write_Reg(fd,TxAutoReg,0x00);
        Write_Reg(fd,RxThresholdReg,0x55);
        Write_Reg(fd,RFCfgReg,0x48);
        Write_Reg(fd,TxBitPhaseReg,0x87);                                  /* Ĭ��ֵ                         */
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
** Descriptions:        FM175xx�����λ
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char  FM175X_SoftReset(int fd)
{    
    Write_Reg(fd,CommandReg,SoftReset);
	usleep(1000);
	set_opt(fd,9600,8,'N',1);
    Set_BitMask(fd,ControlReg,0x10);                               /* 17520��ʼֵ����              */
	SetSpeed(fd,115200);
	usleep(1000);
	
	return  TRUE  ;
}

/*********************************************************************************************************
** Function name:       FM175X_HardReset
** Descriptions:        FM175xxӲ����λ
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char FM175X_HardReset(int fd,int gpio)
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
	if(ioctl(gpio_fd,GPIO_OUT_CMD,gpio) < 0)
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
** Descriptions:        Ӳ���͹��Ĳ���
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char FM175X_SoftPowerdown(int fd)
{
    if(Read_Reg(fd,CommandReg)&0x10) {
       Clear_BitMask(fd,CommandReg,0x10);                                 /* �˳��͹���ģʽ               */
       return FALSE;
    }
    else
        Set_BitMask(fd,CommandReg,0x10);                                       /* ����͹���ģʽ               */
    return TRUE;
}

/*********************************************************************************************************
** Function name:       FM175X_HardPowerdown
** Descriptions:        Ӳ���͹��Ĳ���
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char FM175X_HardPowerdown(void)
{    
    //NPD=~NPD;
    //if(NPD==1)                                                          /* ����͹���ģʽ               */
    return TRUE;                                
//    else
        //return FALSE;                                                     /* �˳��͹���ģʽ               */
}

/*********************************************************************************************************
** Function name:       Read_Ext_Reg
** Descriptions:        ��ȡ��չ�Ĵ���
** input parameters:    reg_add���Ĵ�����ַ��         
** output parameters:   reg_value���Ĵ�����ֵ
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Read_Ext_Reg(int fd,unsigned char reg_add)
{
     Write_Reg(fd,0x0F,0x80+reg_add);
     return Read_Reg(fd,0x0F);
}

/*********************************************************************************************************
** Function name:       Write_Ext_Reg
** Descriptions:        д����չ�Ĵ���
** input parameters:    reg_add���Ĵ�����ַ��
**                      reg_value���Ĵ�����ֵ
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Write_Ext_Reg(int fd,unsigned char reg_add,unsigned char reg_value)
{
    Write_Reg(fd,0x0F,0x40+reg_add);
    return (Write_Reg(fd,0x0F,0xC0+reg_value));
}


