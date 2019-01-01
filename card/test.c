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
** File name:           test.c
** Last modified Date:  2016-3-4
** Last Version:        V1.00
** Descriptions:        用于演示读卡功能
**
**--------------------------------------------------------------------------------------------------------
*/
#include "fm175xx.h"
//#include "FM175XX_REG.h"
#include "type_a.h"
#include "type_b.h"
#include "Utility.h"
#include "mifare_card.h"

#include "string.h"
#include "uart.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//#include "NTAG_API.h"
//#include "CARD_EMULATE_API.h"

#include <stdio.h>

void uartSendByte(unsigned char value)
{
	printf("%c",value);
}

void uartSendHex (unsigned char *pucStr,unsigned char num)
{
    unsigned char hChar,lChar;
    unsigned char *pChar;
    pChar = pucStr;
    while(num--)                 
    {
        hChar = *(pChar)>>4;
        lChar = *(pChar++) & 0x0f;
        if( hChar < 0x0a )  {
            hChar += '0';
        }
        else {
            hChar = hChar-10+'A';
        }
        if( lChar < 0x0a )  {
            lChar += '0';
        }
        else {
            lChar = lChar-10+'A';
        }
        uartSendByte(hChar);
        uartSendByte(lChar);
    }
}
/*
static void Print(unsigned char *buff,unsigned char len)
{
	unsigned char i;
	printf("len =%x\n",len);
	for(i = 0;i < len;i++)
	{
		printf("%x\t",buff[i]);
	}
	printf("\n");
}
*/

/*********************************************************************************************************
** Function name:       UartSendUid
** Descriptions:        使用串口将UID发送出来
** input parameters:    picc_atqa  卡片类型信息
**                      picc_uid   卡片UID信息
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UartSendUid( unsigned char *picc_atqa,unsigned char *picc_uid )
{
    if( ( picc_atqa[0] & 0xc0 ) == 0x00 )   {                           /* 4个字节UID                   */
        uartSendHex(picc_uid ,4);
    }
    if( ( picc_atqa[0] & 0xc0 ) == 0x40 )   {                           /* 7字节UID                     */
        uartSendHex(picc_uid+1 ,3);
        uartSendHex(picc_uid+5 ,4);
    }
    if( ( picc_atqa[0] & 0xc0 ) == 0x80 )   {
        uartSendHex(picc_uid+1 ,3);
        uartSendHex(picc_uid+5 ,3);
        uartSendHex(picc_uid+10 ,4);
    }
}

/*********************************************************************************************************
** Function name:       TyteA_Test()
** Descriptions:        ISO14443A协议测试
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void TyteA_Test (int fd)
{
    unsigned char statues = TRUE;
	//unsigned char ret;
    unsigned char num=0;
	unsigned char nbit = 0;
	//unsigned char rec_len;
	//unsigned char rec_buf[64];
	
    unsigned char picc_atqa[2];                               /* 储存寻卡返回卡片类型信息     */
    static unsigned char picc_uid[15];                       /* 储存卡片UID信息              */
    unsigned char picc_sak[3];            /* 储存卡片应答信息      ,可判断是否为CPU卡  b6 = 0:不是cpu     */
	
	static int test = 0;
	//if(test == 0)
	{
		test = 1;
		FM175X_SoftReset( fd);                                  /* FM175xx软件复位              */
		Set_Rf(fd, 3 );                                         /* 打开双天线                   */
		Pcd_ConfigISOType(fd, 0 );                              /* ISO14443寄存器初始化         */
	}
    
    
	while(num <2 ) {
        statues = TypeA_CardActive(fd, picc_atqa,picc_uid,picc_sak ,&nbit);      /* 激活卡片                     */
        if ( statues == TRUE ) {
			printf("SAK=%x\n",picc_sak[0]);
            num = 0;
            //TypeA_Halt(fd,0);                                              // 睡眠卡片                     
            //LED_RedOn();
			printf("ATQA:%x %x\n",picc_atqa[0],picc_atqa[1]);
            printf("CardUID:0x");
            UartSendUid(picc_atqa,picc_uid);    
            printf("\r\nTyteA_Test\n");     
            //memset(picc_uid,0x00,15);                    
        }
        else {
            num++;
        }                    
    }
	
    //Delay100us(100);
    //LED_RedOff();
}


/*********************************************************************************************************
** Function name:       TyteB_Test()
** Descriptions:        ISO14443B协议测试
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void TyteB_Test (int fd)
{
    unsigned char statues;
    uint32_t rec_len;
    unsigned char pupi[4];
    unsigned char buff[12];
    FM175X_SoftReset(fd );                                                /* FM175xx软件复位              */
    Pcd_ConfigISOType(fd, 1 );                                             /* ISO14443寄存器初始化         */
    Set_Rf( fd,3 );                                                        /* 打开双天线                   */ 
    statues = TypeB_WUP(fd,&rec_len,buff,pupi);                            /* 寻卡                         */
    if ( statues == TRUE ) {
        statues = TypeB_Select(fd,pupi,&rec_len,buff); 
    }
    if ( statues == TRUE ) {
        //LED_RedOn();
        statues = TypeB_GetUID(fd,&rec_len,&buff[0]);
        if(statues == TRUE) {
            printf("CardUID:0x");                                   /* 输出UID号码                  */
            uartSendHex( buff, 8 );
            printf("\r\nTyteB_Test\n");
        }
        //Delay100us(100);
        //LED_RedOff();    
    }
    Set_Rf(fd, 0 );                                                        /* 关闭天线                     */    
}

/*********************************************************************************************************
** Function name:       MifareCard_Test()
** Descriptions:        mifareCard测试
** input parameters:    
**                    0x01： Demo板会返回改例程的版本及相关信息
**                    0x02： Demo板执行激活TypeA卡命令，如果此时有卡片在天线附件则会通过串口打印卡片的ID号
**                    0x03： TypeA卡片密钥认证，卡片被激活后可以通过认证步骤打开卡片的读写权限
**                    0x04： 读TypeA卡片01块数据，读取成功后会通过串口打印读到的16字节数据(16进制格式)
**                    0x05： 向TypeA卡片01块写入16字节数据，写入的是BlockData数据中的数据，并且返回执行结果
**                    0x06： 读TypeB卡命令，Demo板收到这条命令后，会寻找TypeB类型的卡则读取卡
**                           得ID号并通过串口返回
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void MifareCard_Test (int fd,unsigned char command)
{
    unsigned char statues;
	unsigned char nbit = 0;
    unsigned char picc_atqa[2];                                               /* 储存寻卡返回卡片类型信息     */
    static unsigned char picc_uid[15];                                        /* 储存卡片UID信息              */
    unsigned char picc_sak[3];                                                /* 储存卡片应答信息             */
    unsigned char key[6]= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};                    /* 默认密码                     */
    unsigned char blockAddr;
    unsigned char blockData[16];
    unsigned char i;
    uint32_t rec_len;
    unsigned char pupi[4];
    unsigned char buff[12];
    switch (command)
    {
        case 1:                                                         /* Demo板系统信息               */
            printf("FM17550Demo V1.0 2016/03/16.\r\nSupport ISO14443A/Mifare/B.\r\nSupport ISO18092/NFC\r\n");
            break;    
        case 2:                                                         /* 激活卡片                     */
            Set_Rf(fd, 3 );                                                /* 打开双天线                   */
            Pcd_ConfigISOType(fd, 0 );                                     /* ISO14443寄存器初始化         */
            statues = TypeA_CardActive(fd, picc_atqa,picc_uid,picc_sak ,&nbit);  /* 激活卡片                     */
            if ( statues == TRUE ) {
                printf("CardUID:0x");
                UartSendUid(picc_atqa,picc_uid);    
                printf("\r\n");
            }
            else {
                printf("No Card!\r\n");
            }
            break;
        case 3:                                                         // 校验密钥                     
            /*if(GulNum == 1) {
                blockAddr = 0x00;                                       // 块地址  默认为0扇区          
            }
            else if(GulNum == 2)  {                                     // 第二字节为块地址             
                blockAddr = Recv_Buff[1];     
                memset(key,0xff,6);                                     // 默认密钥6个0xff              
            }
            else if(GulNum >= 8) {                                      // 密钥为第3-8字节              
                blockAddr = Recv_Buff[1];
                memcpy(key,&Recv_Buff[2],6);
            }
            else
            {
            }
            statues = Mifare_Auth(0x00,blockAddr>>2,key,picc_uid);      // 校验密码                     
            if ( statues == TRUE ) {
                printf("Auth Ok!\r\n");
            }
            else {
                printf("Auth error!\r\n");
            }
			*/
            break;
        case 4:                                                         // 读块数据                     
         /*   if(GulNum == 1) {
                blockAddr = 0x01;                                       // 读1块数据                    
            }
            else if(GulNum == 2)   {                                    // 传递块地址                    
                blockAddr = Recv_Buff[1];
            }
            else
            {
            }
            statues = Mifare_Blockread(blockAddr,blockData);            // 读块数据函数                 
            if ( statues == TRUE )  {                            //返回成功将数据输出             
            
                printf("Read  %d block:0x",blockAddr);
                uartSendHex(blockData,16);
                printf("\r\n");
            }
            else  {                                                     // 返回失败                     
                printf("Read  error!\r\n");
            }*/
            break;
        case 5:                                                         // 写数据块信息                 
           /* if(GulNum == 1) {
                blockAddr = 0x01;                                       // 默认是1块地址                
            
            else if(GulNum == 2) {                                      // 串口第二字节为块地址         
            blockAddr = Recv_Buff[1];
            for(i=0;i<16;i++)                                           // 默认写入数据为00-0x0f        
                blockData[i] = i;
            }
            else if(GulNum >= 8)  {                                  
                blockAddr = Recv_Buff[1];
                memcpy(blockData,&Recv_Buff[2],16);                     // 串口输入第三字节到十八字节   
            }
            else
            {
            }
            statues = Mifare_Blockwrite(blockAddr,blockData);
             if ( statues == TRUE )  {                                  // 输出信息                    
                 printf("Write %d block:0x",blockAddr);
                 uartSendHex(blockData,16);
                 printf("\r\n");
            }
            else {
                  printf("Write error!\r\n");
            }*/
            break;
        case 6:
            /*statues = TypeA_Halt(Recv_Buff[1]);
            printf("Halt command over.\r\n");*/
            break;
        case 7:
            FM175X_SoftReset(fd );                                        // FM175xx软件复位              
            Pcd_ConfigISOType(fd, 1 );                                     // ISO14443寄存器初始化         
            Set_Rf( fd,3 );                                                // 打开双天线                    
            statues = TypeB_WUP(fd,&rec_len,buff,pupi);                    // 寻卡                         
            if ( statues == TRUE ) {
                statues = TypeB_Select(fd,pupi,&rec_len,buff); 
            }
            if ( statues == TRUE ) {
                statues = TypeB_GetUID(fd,&rec_len,&buff[0]);              // 读取身份证卡号               
            }
            if(statues == TRUE) {
                printf("CardUID:0x");
                uartSendHex( buff, 8 );
                 printf("\r\n");
             }
             else {
                printf("No card!\r\n");
            }
            break;
        default:
            printf("Input error!\r\n");
            break;
        }
}

const unsigned char *help = "\n	please enter cmd:\n \
	1:CardActive!\n  \
	2:CPU_Rats\n!    \
	3:CPU_BaudRate_Set!\n \
	4:CPU_auth!\n      \
	5:MF_erase!\n       \
	6:select!\n        \
	7:create key file!\n    \
	8:write key!\n       \
	9:create bin file!\n  \
	10:write bin file!\n   \
	11:select file!\n     \
	12:read bin file !\n  \
	13:send data!\n    \
	14: write record!\n \
	15:read record!\n    \
	";

const unsigned char *file = "/data/log";
int logfd;
void cpu_test(int fd)
{
	
	int cmd;
	unsigned char ret;
	unsigned char cmd_len;
	unsigned char nbit = 0;
	unsigned char cmd_buf[128] = {0};
	unsigned char dat[64] = {0};
	unsigned char key[16] = {0};
	unsigned char rec_len;        
	unsigned char rec_buf[64];       
	unsigned char picc_atqa[2];                /* 储存寻卡返回卡片类型信息     */
    static unsigned char picc_uid[15];          /* 储存卡片UID信息              */
    unsigned char picc_sak[3];            /* 储存卡片应答信息      ,可判断是否为CPU卡  b6 = 0:不是cpu */
	unsigned char statues = FALSE;
	
	FM175X_SoftReset( fd);                                  /* FM175xx软件复位              */
    Set_Rf(fd, 3 );                                         /* 打开双天线                   */
    Pcd_ConfigISOType(fd, 0 );                              /* ISO14443寄存器初始化         */
	
	logfd = open(file,O_CREAT|O_WRONLY,0);
	if(logfd > 0)
	{
		printf("create log file sucess!\n");
	}
	else
	{
		printf("create log file error!\n");
	}
	int run = 1;
	while(run)
	{
		printf(help);
		scanf("%d",&cmd);
		switch(cmd)
		{
			case 1:
				FM175X_SoftReset( fd);                                  /* FM175xx软件复位              */
				Set_Rf(fd, 3 );                                         /* 打开双天线                   */
				Pcd_ConfigISOType(fd, 0 );                              /* ISO14443寄存器初始化         */
				statues = TypeA_CardActive(fd, picc_atqa,picc_uid,picc_sak ,&nbit);   /* 激活卡片*/
				if ( statues == TRUE )
				{
					printf("CardActive sucess!\n");
					printf("CardUID:0x");
					UartSendUid(picc_atqa,picc_uid);
				}
				else
				{
					printf("CardActive failed!\n");
					
				}
				break;
			case 2:
				if(statues == TRUE)
				{
					if(picc_sak[0]  &  0x20)
					{
						printf("CPU_Rats\n");
						CPU_Rats(fd,0x0A,0x51,&rec_len,rec_buf);
						Print(rec_buf,rec_len);
					}
				}
				else
				{
					printf("please active card!\n");
				}
				break;
			case 3:
				if(picc_sak[0]  &  0x20)
				{
					printf("CPU_PPS\n");
					ret = CPU_PPS(fd,0x01,0,&rec_len,rec_buf);
					printf("ret = %x\n",ret);
					Print(rec_buf,rec_len);
					
					printf("CPU_BaudRate_Set\n");
					ret = CPU_BaudRate_Set(fd,0);
					printf("ret = %x\n",ret);
				}
				break;
			case 4:
				if(picc_sak[0]  &  0x20)
				{
					printf("enter key(8 byte):\n");
					scanf("%s",cmd_buf);
					sscanf(cmd_buf,"%2x%2x%2x%2x%2x%2x%2x%2x",&key[0],&key[1],&key[2],&key[3],&key[4],&key[5],&key[6],&key[7]);
					printf("key:%2x%2x%2x%2x%2x%2x%2x%2x\n",key[0],key[1],key[2],key[3],key[4],key[5],key[6],key[7]);
					CPU_auth(fd,key,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			case 5:
				if(picc_sak[0]  &  0x20)
				{
					printf("CPU_Mf_Erase!\n");
					CPU_Mf_Erase(fd,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				
				break;
			case 6:
				if(picc_sak[0]  &  0x20)
				{
					printf("CPU_Slect!\n");
					CPU_Slect(fd,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				
				break;
			case 7:
				if(picc_sak[0]  &  0x20)
				{
					printf("CPU_Key_Create!\n");
					CPU_Key_Create(fd,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			case 8:
				
				if(picc_sak[0]  &  0x20)
				{
					printf("CPU_Key_write!\n");
					printf("enter dat!\n");
					dat[0] = 0x80;
					dat[1] = 0xD4;
					dat[2] = 0x01;
					dat[3] = 0x00;
					dat[4] = 0x0D;
					dat[5] = 0x39;
					dat[6] = 0xF0;
					dat[7] = 0xF0;
					dat[8] = 0xAA;
					dat[9] = 0x88;
					scanf("%s",cmd_buf);
					sscanf(cmd_buf,"%2x%2x%2x%2x%2x%2x%2x%2x",&dat[10],&dat[11],&dat[12],&dat[13],&dat[14],&dat[15],&dat[16],&dat[17]);
					
					CPU_I_Block(fd,18,dat,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			case 9:
				
				if(picc_sak[0]  &  0x20)
				{
					printf("create file!enter dat!\n");
					scanf("%s",cmd_buf);
					sscanf(cmd_buf,"%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",&dat[2],&dat[3],&dat[4],&dat[5],&dat[6],&dat[7],&dat[8],&dat[9],&dat[10],&dat[11]);
					dat[0] = 0x80;
					dat[1] = 0xE0;
					CPU_I_Block(fd,12,dat,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			case 10:
				
				if(picc_sak[0]  &  0x20)
				{
					printf("write bin dat!\n");
					dat[0] = 0x00;
					dat[1] = 0xD6;
					//dat[2] = 0x00;
					//dat[3] = 0x00;
					//dat[4] = 0x03;
					scanf("%s",cmd_buf);
					sscanf(cmd_buf,"%2x%2x%2x%2x%2x%2x",&dat[2],&dat[3],&dat[4],&dat[5],&dat[6],&dat[7]);
					CPU_I_Block(fd,8,dat,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			case 11:
				
				if(picc_sak[0]  &  0x20)
				{
					printf("select file!\n");
					dat[0] = 0x00;
					dat[1] = 0xA4;
					dat[2] = 0x00;
					dat[3] = 0x00;
					dat[4] = 0x02;
					scanf("%s",cmd_buf);
					sscanf(cmd_buf,"%2x%2x",&dat[5],&dat[6]);
					CPU_I_Block(fd,7,dat,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			case 12:
				
				if(picc_sak[0]  &  0x20)
				{
					printf("read bin file!enter dat\n");
					dat[0] = 0x00;
					dat[1] = 0xB0;
					scanf("%s",cmd_buf);
					sscanf(cmd_buf,"%2x%2x%2x",&dat[2],&dat[3],&dat[4]);
					CPU_I_Block(fd,5,dat,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			case 13:
			
				scanf("%s",cmd_buf);
				sscanf(cmd_buf,"%2x%2x%2x",&dat[0],&dat[1],&dat[2]);
				ret=Pcd_Comm(fd,Transceive,dat,3,rec_buf,&rec_len,0);
				if(ret == 1)
					printf("send sucess!\n");
				Print(rec_buf,rec_len);
				break;
			case 14:	
				if(picc_sak[0]  &  0x20)
				{
					printf("write record dat!\n");
					dat[0] = 0x00;
					dat[1] = 0xDC;
					//dat[2] = 0x00;
					//dat[3] = 0x00;
					//dat[4] = 0x03;
					scanf("%s",cmd_buf);
					printf("sucess\n");
					sscanf(cmd_buf,"%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",&dat[2],&dat[3],&dat[4],&dat[5],&dat[6],&dat[7],&dat[8],&dat[9],&dat[10],&dat[11],&dat[12],&dat[13],&dat[14],&dat[15],&dat[16],&dat[17],&dat[18],&dat[19],&dat[20]);
					printf("sucess1\n");
					CPU_I_Block(fd,21,dat,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			case 15:	
				if(picc_sak[0]  &  0x20)
				{
					printf("read record dat!\n");
					dat[0] = 0x00;
					dat[1] = 0xB2;
					scanf("%s",cmd_buf);
					sscanf(cmd_buf,"%2x%2x%2x",&dat[2],&dat[3],&dat[4]);
					CPU_I_Block(fd,5,dat,&rec_len,rec_buf);
					Print(rec_buf,rec_len);
				}
				break;
			default:
				run = 0;
				break;
		}
		
	}
	close(logfd);
}

void CPU_Test(int fd)
{
	 unsigned char statues = TRUE;
	unsigned char ret;
    unsigned char num=0;
	unsigned char nbit = 0;
	unsigned char rec_len;
	unsigned char rec_buf[64];
	unsigned char dat[16] = {0};
	unsigned char buff[16] = {0};
	unsigned char key[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
    unsigned char picc_atqa[2];                               /* 储存寻卡返回卡片类型信息     */
    static unsigned char picc_uid[15];                       /* 储存卡片UID信息              */
    unsigned char picc_sak[3];            /* 储存卡片应答信息      ,可判断是否为CPU卡  b6 = 0:不是cpu     */
    FM175X_SoftReset( fd);                                  /* FM175xx软件复位              */
    Set_Rf(fd, 3 );                                         /* 打开双天线                   */
    Pcd_ConfigISOType(fd, 0 );                              /* ISO14443寄存器初始化         */
	
	statues = TypeA_CardActive(fd, picc_atqa,picc_uid,picc_sak ,&nbit);      /* 激活卡片    */
	if ( statues == TRUE ) {
			if(picc_sak[0]  &  0x20)
			{
				printf("CPU_Rats\n");
				CPU_Rats(fd,0x0A,0x51,&rec_len,rec_buf);
				Print(rec_buf,rec_len);
				
				printf("CPU_PPS\n");
				ret = CPU_PPS(fd,0x01,0,&rec_len,rec_buf);
				printf("ret = %x\n",ret);
				Print(rec_buf,rec_len);
				
				printf("CPU_BaudRate_Set\n");
				ret = CPU_BaudRate_Set(fd,0);
				printf("ret = %x\n",ret);
				
				printf("CPU_Mf_Slect\n");
				ret = CPU_Slect(fd,&rec_len,rec_buf);
				printf("ret = %x\n",ret);
				Print(rec_buf,rec_len);
				
				printf("CPU_auth\n");
				ret = CPU_auth(fd,NULL,&rec_len,rec_buf);
				printf("ret = %x\n",ret);
				Print(rec_buf,rec_len);
				
				/*printf("CPU_Random\n");
				ret = CPU_Random(fd,&rec_len,rec_buf);
				printf("ret = %x\n",ret);
				Print(rec_buf,rec_len);
				
				if((rec_buf[rec_len -1] == 0x00)&&(rec_buf[rec_len -2] == 0x90))
				{
					memcpy(dat,&rec_buf[2],8);
					des_ecb_encrypt(buff,dat,8,key);
				}*/
				
				
				
				
			}
			//printf("SAK=%x\n",picc_sak[0])
			
            //num = 0;
            TypeA_Halt(fd,0);                                              // 睡眠卡片                     
            //LED_RedOn();
			//printf("ATQA:%x %x\n",picc_atqa[0],picc_atqa[1]);
            //printf("CardUID:0x");
            //UartSendUid(picc_atqa,picc_uid);    
            //printf("\r\nTyteA_Test\n");     
            //memset(picc_uid,0x00,15);                    
        }
		
}


