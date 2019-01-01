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
#include "type_a.h"
#include "type_b.h"
#include "Utility.h"
#include "mifare_card.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "card.h"
#include <pthread.h>
#include <sys/time.h>
#include <termios.h>
#include "uart.h"
#include "cpu_card.h"



#define INC(a,b)  \
		do{          \
			a = (a+b)%BUFFSIZE ;   \
		}while(0)

static unsigned char machine_type = 0;         //1:new  2:old

const unsigned char tail_flag[2] = {0xAA,0xBB};

static struct {
	pthread_t pid;
	pthread_mutex_t mutex;
	int run;
	int mfd;
	
} card_pthread;

static struct {
	unsigned char *mbuff;

	int mtail;
	int mhead;
	int mlen;
	int readlen;
	
}card_info;


int serial_read(int fd,unsigned char dat[],int num)
{
	int read_num = num;
	int count = 3;
	unsigned char *pdat = dat;
	int ret;
	fd_set readfs;
	struct timeval Timeout;
	
	Timeout.tv_usec = 200000;
	Timeout.tv_sec  = 0;
	FD_ZERO(&readfs);
	FD_SET(fd, &readfs);
	
	do
	{
		ret = select(fd+1, &readfs, NULL, NULL, &Timeout);
		if(ret == 0)
		{
			printf("time out!\n");
			//continue;
			return 0;
		
				
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


//const unsigned char new_key[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//unsigned char default_key[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

void *card_fun(void *arg)
{
	struct timeval start,end;
	int n;
	unsigned char statues = FALSE;
	int read_len = 0;
    unsigned char num=0;
	int ret_len = 0;
	unsigned char nbit = 0;
	unsigned char cmd[64] = {0};	
	unsigned char rec_len;        
	unsigned char rec_buf[64];  
    unsigned char pupi[4];
	unsigned char read_buf[512];
    unsigned char picc_atqa[2];                           // 储存寻卡返回卡片类型信息 
    unsigned char picc_sak[3];                            // 储存卡片应答信息
	unsigned char id[15] = {0};     
	unsigned char sector_num;
	unsigned char read_nbyte;
	unsigned char sector_buff[48];
	//unsigned char *Pbuf = buff;
	unsigned char *key = (unsigned char *)arg;
	
	if(!key)
	{
		printf("key error!\n");
		return (void *)-1;
	}
	Print(key,8);
	if(card_pthread.mfd < 0)
	{
		printf("failed fd = %d!\n",card_pthread.mfd);
		return (void *)-1;
	}
	while(card_pthread.run)
	{
run_end:
		printf("run\n");
		gettimeofday(&start, NULL);
		
		if(machine_type == 1)
		{
			//card_info.mhead = card_info.mtail;
			//typeA
			FM175X_SoftReset( card_pthread.mfd);             // FM175xx软件复位      
			Set_Rf(card_pthread.mfd, 3 );                   // 打开双天线              
			Pcd_ConfigISOType(card_pthread.mfd, 0 );       // ISO14443寄存器初始化     
			
			
			statues = TypeA_CardActive(card_pthread.mfd, picc_atqa,id,picc_sak ,&nbit);      // 激活卡片    
			if ( statues == TRUE ) 
			{
				memcpy(&card_info.mbuff[card_info.mhead],tail_flag,2);
				INC(card_info.mhead,2);
				memcpy(&card_info.mbuff[card_info.mhead],&nbit,1);
				INC(card_info.mhead,1);
				memcpy(&card_info.mbuff[card_info.mhead],id,nbit);
				Print(&card_info.mbuff[card_info.mhead],nbit);
				INC(card_info.mhead,nbit);
				//memcpy(&card_info.mbuff[card_info.mhead],0x0,EMPT_NUM);    //预留字节为0x0
				//INC(card_info.mhead,EMPT_NUM);
				
				if(picc_sak[0] & 0x04)
				{
					ret_len = nbit + 1;
					goto run_end;

					//*IC_type = TYPE_A_NFC;
				}
				else if(picc_sak[0] & 0x20)
				{
					//*IC_type = TYPE_A_CPU;
					
					if(!CPU_Rats(card_pthread.mfd,0x0A,0x51,&rec_len,rec_buf))
					{
						printf("CPU_Rats error!\n");
						goto run_end;
					}
						
					if(!CPU_auth(card_pthread.mfd,key,&rec_len,rec_buf))
					{
						printf("CPU_auth error!\n");
						goto run_end;
					}
		
					cmd[0] = 0x00;
					cmd[1] = 0x05;
					if(!CPU_File_Select(card_pthread.mfd,cmd))
					{
						printf("CPU_File_Select error!\n");
						goto run_end;
					}

					//printf("read bin file!\n");
					cmd[0] = 0x00;
					cmd[1] = 0x00;
					
					read_len = CPU_ReadFlie(card_pthread.mfd,cmd,rec_buf,2);
					if(read_len != 2)
					{
						printf("CPU_ReadFlie error!\n");
						goto run_end;
					}
					read_len = rec_buf[0]*256 + rec_buf[1] + 2;
					do{
						read_nbyte = ONE_SEC_BYTE;
						if(read_len < ONE_SEC_BYTE)
						{
							read_nbyte = read_len;
						}
						if(CPU_ReadFlie(card_pthread.mfd,cmd,&card_info.mbuff[card_info.mhead],read_nbyte) != read_nbyte)
						{
							printf("CPU_ReadFlie error!\n");
							goto run_end;
						}
						INC(card_info.mhead,read_nbyte);
						
						read_len -= read_nbyte;
						
						cmd[0] += (cmd[1] + read_nbyte) / 256;
						cmd[1] += read_nbyte; 
					}while(read_len > 0);
					//read_len = CPU_ReadFlie(fd,cmd,&buff[nbit+1],rec_buf[0]*256 + rec_buf[1] + 2);
					//printf("read bin file end!read_len=%d\n",read_len);
					//if(/*rec_buf[rec_len-1] == 0x00 && rec_buf[rec_len-2] == 0x90 &&*/read_len > 0 &&(rec_buf[rec_buf[0]*256 + rec_buf[1] + 1] == GetByteBCC(&rec_buf[2],rec_buf[0]*256 + rec_buf[1]-1)))
					//	memcpy(&buff[nbit+1],rec_buf,rec_buf[0]*256 + rec_buf[1] + 2);
					//else
						//return read_len + nbit+1;
					//if((buff[buff[nbit+1]*256 + buff[nbit+2] + nbit + 2] != GetByteBCC(&buff[nbit+3],buff[nbit+1]*256 + buff[nbit+2]-1)))
					//	return 0;
					
				}
				else
				{
					//*IC_type = TYPE_A_M;
					
					if(!Mifare_SectorRead(card_pthread.mfd,M1_START_SECTOR,0x0,0x01,key,id,rec_buf))
					{
						printf("Mifare_SectorRead error!\n");
						goto run_end;
					}
					read_len = rec_buf[0] * 256 + rec_buf[1] + 2;
					sector_num = 0;
					//Pbuf = &buff[nbit+1];
					do{
						read_nbyte = ONE_SEC_BYTE;
						if(read_len < ONE_SEC_BYTE)
						{
							read_nbyte = read_len;
						}
						
						if(!Mifare_SectorRead(card_pthread.mfd,M1_START_SECTOR + sector_num,0x0,0x02,key,id,sector_buff))
						{
							printf("Mifare_SectorRead error!\n");
							goto run_end;
						}
						//Print(sector_buff,48);
						//printf("read_nbyte=%d\n",read_nbyte);
						memcpy(&card_info.mbuff[card_info.mhead],sector_buff,read_nbyte);
						INC(card_info.mhead,read_nbyte);
						read_len -= read_nbyte;
						sector_num++;
					}while(read_len > 0);
					
					//if((buff[buff[nbit+1]*256 + buff[nbit+2] + nbit + 2] != GetByteBCC(&buff[nbit+3],buff[nbit+1]*256 + buff[nbit+2]-1)))
					//	return 0;
					
				}
				if(card_info.mhead != card_info.mtail)
				{
					gettimeofday(&end, NULL);
					printf("total time = %dms\n",(end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);
					Print(&card_info.mbuff[card_info.mtail],card_info.mhead - card_info.mtail);
					card_info.mhead = card_info.mtail = 0;
				}
				//ret_len = nbit + 3 + buff[nbit+1]*256 + buff[nbit+2];
				goto run_end;
			}
			   
			
					 
			
			//Delay100us(1000);
			//typeB
			FM175X_SoftReset( card_pthread.mfd);                                      // FM175xx软件复位       
			Pcd_ConfigISOType(card_pthread.mfd, 1 );                              // ISO14443寄存器初始化      
			Set_Rf( card_pthread.mfd,3 );                                         // 打开双天线                 
			statues = TypeB_WUP(card_pthread.mfd,&rec_len,id,pupi);              // 寻卡                      
			if ( statues == TRUE ) {
				statues = TypeB_Select(card_pthread.mfd,pupi,&rec_len,id); 
			}
			if ( statues == TRUE ) {
				//LED_RedOn();
				nbit = rec_len;
				memcpy(&card_info.mbuff[card_info.mhead],tail_flag,2);
				INC(card_info.mhead,2);
				memcpy(&card_info.mbuff[card_info.mhead],&nbit,1);
				INC(card_info.mhead,1);
					
				statues = TypeB_GetUID(card_pthread.mfd,&rec_len,&id[0]);
				if(statues == TRUE) {
					memcpy(&card_info.mbuff[card_info.mhead],id,nbit);
					Print(&card_info.mbuff[card_info.mhead],nbit);
					INC(card_info.mhead,nbit);
					//memcpy(&card_info.mbuff[card_info.mhead],0x0,EMPT_NUM);    //预留字节为0x0
					//INC(card_info.mhead,EMPT_NUM);
					//printf("CardUID:0x");                                  // 输出UID号码   
					//uartSendHex( id, 8 );
					printf("\r\nTyteB_Test\n");
					//return nbit;
					//ret_len = nbit +1;
					goto run_end;
				
				}
				else
				{
					nbit = 0;
				}
				
			}
			Set_Rf(card_pthread.mfd, 0 );                                                // 关闭天线   
		}
		else if(machine_type == 2 )
		{
			if(n = read(card_pthread.mfd,read_buf,512) > 0)
			{
				memcpy(&card_info.mbuff[card_info.mhead],tail_flag,2);
				INC(card_info.mhead,2);
				if((read_buf[0] == 0x04)&&(n > 4))
				{
					memcpy(&card_info.mbuff[card_info.mhead],read_buf,5);
					INC(card_info.mhead,5);
					//memcpy(&card_info.mbuff[card_info.mhead],0x0,EMPT_NUM);    //预留字节为0x0
					//INC(card_info.mhead,EMPT_NUM);
					//ret_len = 5;
				}
				else if((read_buf[0] == 0x08)&&(n > 8))
				{
					memcpy(&card_info.mbuff[card_info.mhead],tail_flag,2);
					INC(card_info.mhead,2);
					memcpy(&card_info.mbuff[card_info.mhead],read_buf,9);
					//ret_len = 9;
					INC(card_info.mhead,9);
					//memcpy(&card_info.mbuff[card_info.mhead],0x0,EMPT_NUM);    //预留字节为0x0
					//INC(card_info.mhead,EMPT_NUM);
				}
				else
				{
				
					memset(read_buf,0,512);
					//ret_len = 0;
				}
					
			}
			
			
		}
		else
		{
			//printf("please check type!\n");
			//return -1;
		}
		
		//printf("ret_len=%d\n",ret_len);
		
		
	}
		
		return (void *)0;
}

/********************************************************
函数：card_pthread_init
功能：初始化卡操作信息，创建线程
输入：fd：串口文件描述符
返回：正常返回0，错误返回-1
************************************************************/
int card_pthread_init(int fd,unsigned char *key)
{
	int ret = -1;
	
	if(fd < 0)
	{
		return -1;
	}
	Check_Machine(fd);
	if(card_pthread.run == TRUE)
	{
		printf("card_pthread is running!\n");
		return -1;
	}
	
	card_info.mbuff = (unsigned char *)malloc(BUFFSIZE);
	if(!card_info.mbuff)
	{
		printf("malloc error!\n");
		return -1;
	}
	card_info.mhead = card_info.mtail = 0;
	card_info.mlen = 0;
	card_info.readlen = 0;
	card_pthread.run = TRUE;
	card_pthread.mfd  = fd;
	ret = pthread_create(&card_pthread.pid,NULL,card_fun,key);
	if(ret != 0)
	{
		card_pthread.run = FALSE;
		free(card_info.mbuff);
	}
	return ret;
}

void card_pthread_close()
{
	card_pthread.run = FALSE;
	card_pthread.mfd = -1;
	if(card_info.mbuff)
	{
		free(card_info.mbuff);
	}
}
unsigned char Check_Machine(int fd)
{
	if(machine_type == 0)
	{
		pcd_RST(fd);
		Read_Reg(fd,ControlReg);
		Write_Reg(fd,ControlReg, 0x10);
		if(Read_Reg(fd,ControlReg) == 0x10)
			machine_type = 1;
		else 
		{
			machine_type = 2;
		}
		printf("machine_type =%d\n",machine_type);
	}
	
	
	return machine_type;
}

unsigned char Get_Machine_Type()
{
	return machine_type;
}


void Card_init(int fd,int type)
{
	FM175X_SoftReset( fd);             /* FM175xx软件复位        */
	if(type == 0)           //typeA
	{
		Pcd_ConfigISOType(fd, 0 );       /* ISO14443寄存器初始化     */
	}
	else             //typeB
	{
		Pcd_ConfigISOType(fd, 1 );                              /* ISO14443寄存器初始化      */
	}
	Set_Rf( fd,3 );                                         /* 打开双天线                 */ 
}

/*********************************************************************************************************
** Function name:       Card_WakeUp        
** Descriptions:        卡片唤醒
** input parameters:    N/A
**                      type:0:typeA else:typeB
** output parameters:   typeA:pTagType[0],pTagType[1] =ATQA 
**                      typeB:
**                      rece_len:卡片应答的数据长度
**                      buff：卡片应答的数据指针    
**                      card_sn:卡片的PUPI字节
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/

unsigned char Card_WakeUp(int fd,int type,unsigned char *pTagType,unsigned int *rece_len,unsigned char *buff,unsigned char *card_sn)
{
	if(type == 0)
	{
		return TypeA_WakeUp(fd,pTagType);
	}
	else
	{
		return TypeB_WUP(fd,rece_len,buff,card_sn);
	}
}

/*********************************************************************************************************
** Function name:       card_Halt        
** Descriptions:        卡片睡眠
** input parameters:    type:0:typeA  else typeB
**						typeA:
**						AnticollisionFlag 休眠验证标志 
**                      AnticollisionFlag = 0 没有验证，使用明文通信，需要清除验证标志
**                      AnticollisionFlag = 1 密码验证函数通过，使用密文通信，不需要清除验证标志
**						typeB
**                      card_sn：卡片的PUPI
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/

unsigned char Card_Halt(int fd,int type,unsigned char AnticollisionFlag,unsigned char *card_sn)
{
	if(type == 0)
	{
		return TypeA_Halt(fd,AnticollisionFlag);
	}
	else
	{
		return TypeB_Halt(fd,card_sn);
	}
}

/*********************************************************************************************************
** Function name:       Get_Card_ID        
** Descriptions:        获取卡片ID    
** output parameters:   Id_buf：返回ID
**                      IC_type：IC类型1:M1 2:NFC 3:CPU
** Returned value:      ID字节    
*********************************************************************************************************/
int Get_Card_ID(int fd,unsigned char *Id_buf,int type,int *IC_type)
{
	int n;
	int i;
	unsigned char statues = FALSE;
	
    unsigned char num=0;
	unsigned char nbit = 0;
	uint32_t rec_len;
    unsigned char pupi[4];
	unsigned char read_buf[512];
    unsigned char picc_atqa[2];                           // 储存寻卡返回卡片类型信息 
    unsigned char picc_sak[3];                            // 储存卡片应答信息
	
	if(machine_type == 1)
	{
		if(type == 0)
		{
			statues = TypeA_CardActive(fd, picc_atqa,Id_buf,picc_sak ,&nbit);      // 激活卡片    
			if ( statues == TRUE ) 
			{
				if(picc_sak[0] & 0x04)
					*IC_type = TYPE_A_NFC;
				else if(picc_sak[0] & 0x20)
					*IC_type = TYPE_A_CPU;
				else
					*IC_type = TYPE_A_M;
				printf("\nSAK=%x\n",picc_sak[0]);
				printf("ATQA:%x %x\n",picc_atqa[0],picc_atqa[1]);
				goto end;
				
				//printf("CardUID:0x");
				//UartSendUid(picc_atqa,id);    
				//printf("\r\nTyteA_Test\n");     
				//return nbit;
				//memset(id,0x00,15);                    
			}
			else {
				nbit = 0;
				
			}  
		}
		else
		{
			statues = TypeB_WUP(fd,&rec_len,Id_buf,pupi);              // 寻卡                      
			if ( statues == TRUE ) 
			{
				statues = TypeB_Select(fd,pupi,&rec_len,Id_buf); 
			}
			if ( statues == TRUE ) 
			{
				nbit = rec_len;
				statues = TypeB_GetUID(fd,&rec_len,&Id_buf[0]);
				if(statues == TRUE) {
					goto end;
			}
			else
			{
				nbit = 0;
			}
			
		}
		}
	}
	else if(machine_type == 2 )
	{
		if(n = read(fd,read_buf,512) > 0)
		{
			if((read_buf[0] == 0x04)&&(n > 4))
			{
				memcpy(Id_buf,read_buf,4);
				nbit = 4;
			}
			else if((read_buf[0] == 0x08)&&(n > 8))
			{
				memcpy(Id_buf,read_buf,8);
				nbit = 8;
			}
			else
			{
			
				memset(read_buf,0,512);
				nbit = 0;
			}
				
		}
		
	}
	else
	{
		printf("please check type!\n");
		return -1;
	}
	
end:
	return nbit;
}

/****************************************************************
名称: Write_Card_Info                           
功能: 写数据到卡中                                   
输入:  new_key：新密钥
		buff：写入数据缓存，包括两字节长度信息和一字节数据校验信息																													
                                                                                                                                  
输出:                                                  
                                      
         TRUE: 应答正确                                            
         ERROR: 应答错误                                            
*****************************************************************/
unsigned char Write_Card_Info(int fd,unsigned char *old_key,unsigned char *new_key,unsigned char *buff,int len)
{
	int i,n;
	int num=0;
	unsigned char statues = FALSE;
	
	unsigned char nbit = 0;
	
	unsigned char rec_len;        
	unsigned char rec_buf[64] = {0x01,0x02};  
	unsigned char cmd[64] = {0};	
    
	
    unsigned char picc_atqa[2];                           // 储存寻卡返回卡片类型信息 
    unsigned char picc_sak[3];                            // 储存卡片应答信息
	unsigned char id[15];   
	unsigned char sector_num;
	unsigned char block_num;
	unsigned char last_block;
	unsigned char write_nbyte;
	unsigned char *Pbuf = buff;
	unsigned char sector_buff[48];
	
	
	if(new_key == NULL || old_key == NULL || Pbuf == NULL)
	{
		printf("please check Parameters\n");
		return FALSE;
	}
	
	num = len;
	
	Card_init(fd,0);
	
	statues = TypeA_CardActive(fd, picc_atqa,id,picc_sak ,&nbit);      // 激活卡片    
	if ( statues == TRUE ) 
	{
		if(picc_sak[0] & 0x04)
		{
			//*IC_type = TYPE_A_NFC;
		}
		else if(picc_sak[0] & 0x20)
		{
			//*IC_type = TYPE_A_CPU;
			
			/*if(num > CPU_FILE_LEN)
			{
				printf("out of range!\n");
				return FALSE;
			}*/
			if(New_Cpu_Card_init(fd,old_key,new_key))
			{
				printf("create file!\n");
			
				cmd[0] = 0x00;
				cmd[1] = 0x05;      //文件标识0005
				cmd[2] = len/256;     //
				cmd[3] = len%256;
				if(Creat_Bin_file(fd,cmd))
				{
					printf("Creat_Bin_file sucess!\n");
				
				
					printf("write bin dat!\n");
					cmd[0] = 0x00;
					cmd[1] = 0x00;
					
					Pbuf[Pbuf[0]*256 + Pbuf[1] +1] = GetByteBCC(&Pbuf[2],Pbuf[0]*256 + Pbuf[1]-1);
					//Print(send_buff1,n + 3);
					CPU_WriteFlie(fd,cmd,Pbuf);
					//memcpy(&cmd[5],send_buff1,9);
					
				}
				else
				{
					printf("Creat_Bin_file failed!\n");
					return FALSE;
				}
			}
			else
			{
				printf("New_Cpu_Card_init failed!\n");
				return FALSE;
			}		
		}
		else
		{
			//*IC_type = TYPE_A_M;
			
			if(num > M1_FILE_LEN)
			{
				printf("out of range!\n");
				return FALSE;
			}			
			sector_num = 0;
			do{
				write_nbyte = ONE_SEC_BYTE;
				if(num < ONE_SEC_BYTE)
				{
					write_nbyte = num;
					memcpy(sector_buff,Pbuf,num);
					Pbuf = sector_buff;
				}
				Print(Pbuf,48);
				Mifare_SectorWrite(fd,M1_START_SECTOR + sector_num,0x0,0x02,new_key,id,Pbuf);
				num -= ONE_SEC_BYTE;
				sector_num++;
			}while((num > 0)&&(Pbuf += ONE_SEC_BYTE));		
		}
		printf("\nSAK=%x\n",picc_sak[0]);
		printf("ATQA:%x %x\n",picc_atqa[0],picc_atqa[1]);
 
	}
	else {
		nbit = 0;
		printf("NO Card!\n");
		
		
	}  
	return TRUE;
}

int Get_Uid(int fd,unsigned char *buff,int len)
{
	int n;
	unsigned char statues = FALSE;
	int read_len = 0;
    unsigned char num=0;
	int ret_len = 0;
	unsigned char nbit = 0;
	unsigned char cmd[64] = {0};	
	unsigned int rec_len;        
	unsigned char rec_buf[64];  
    unsigned char pupi[4];
	unsigned char read_buf[512];
    unsigned char picc_atqa[2];                           // 储存寻卡返回卡片类型信息 
    unsigned char picc_sak[3];                            // 储存卡片应答信息
	unsigned char id[15] = {0};     
	unsigned char sector_num;
	unsigned char read_nbyte;
	unsigned char sector_buff[48];
	unsigned char *Pbuf = buff;
	
	if(!buff || len <= 0)
	{
		printf("please check Parameters\n");
		return 0;
	}
	if(machine_type == 1)
	{
		//typeA
		FM175X_SoftReset( fd);             // FM175xx软件复位      
		Set_Rf(fd, 3 );                   // 打开双天线              
		Pcd_ConfigISOType(fd, 0 );       // ISO14443寄存器初始化     
		
		while(num <2 ) {
			statues = TypeA_CardActive(fd, picc_atqa,id,picc_sak ,&nbit);      // 激活卡片    
			if ( statues == TRUE ) 
			{
				if(len < nbit)
					goto end;
				buff[1] = nbit;
				//memcpy(&buff[1],&nbit,1);
				memcpy(&buff[2],id,nbit);
				//Print(buff,nbit+1);
				if(picc_sak[0] & 0x04)
				{
					buff[0] = TYPE_A_NFC;
					//*IC_type = TYPE_A_NFC;
				}
				else if(picc_sak[0] & 0x20)
				{
					//*IC_type = TYPE_A_CPU;
					buff[0] = TYPE_A_CPU;
					
				}
				else
				{
					//*IC_type = TYPE_A_M;
					buff[0] = TYPE_A_M;
				}
				
				ret_len = nbit + 2;
				goto end;
			}
			else {
				
				num++;
			}           
		}
		         
		
		//Delay100us(1000);
		//typeB
		FM175X_SoftReset( fd);                                      // FM175xx软件复位       
		Pcd_ConfigISOType(fd, 1 );                              // ISO14443寄存器初始化      
		Set_Rf( fd,3 );                                         // 打开双天线                 
		statues = TypeB_WUP(fd,&rec_len,id,pupi);              // 寻卡                      
		if ( statues == TRUE ) {
			statues = TypeB_Select(fd,pupi,&rec_len,id); 
		}
		if ( statues == TRUE ) {
			//LED_RedOn();
			nbit = rec_len;
			buff[0] = TYPE_B_ID;
			buff[1] = nbit;
			//memcpy(&buff[1],&nbit,1);
			
			statues = TypeB_GetUID(fd,&rec_len,&id[0]);
			if(statues == TRUE) {
				memcpy(&buff[2],id,nbit);
				ret_len = nbit +2;
				goto end;
			
			}
			else
			{
				nbit = 0;
			}
			
		}
		Set_Rf(fd, 0 );                                                // 关闭天线   
	}
	else if(machine_type == 2 )
	{
		if(n = read(fd,read_buf,512) > 0)
		{
			if((read_buf[0] == 0x04)&&(n > 4))
			{
				memcpy(buff,read_buf,5);
				ret_len = 5;
			}
			else if((read_buf[0] == 0x08)&&(n > 8))
			{
				memcpy(buff,read_buf,9);
				ret_len = 9;
			}
			else
			{
			
				memset(read_buf,0,512);
				ret_len = 0;
			}
				
		}
		
		
	}
	else
	{
		printf("please check type!\n");
		return -1;
	}
	
	

end:
	//printf("ret_len=%d\n",ret_len);
	return ret_len;
}

void swp_uid(unsigned char *uid_data,int n)
{
	unsigned char tmp;
	int i ;
	int num = n/2;
	
	for(i = 0; i < num; i++)
	{
		tmp = uid_data[i];
		uid_data[i] = uid_data[n - i - 1];
		uid_data[n - i - 1] = tmp;
	
	}
}
/****************************************************************
名称: Read_Card                           
功能: 读卡信息                                   
输入:  key：卡密钥
		buff：数据缓存，包括两卡号以及卡信息																													
                                                                                                                                  
输出:                                                  
                                      
    返回读数据长度                                           
*****************************************************************/
int Read_Card(int fd,unsigned char *key,unsigned char *buff,int len)
{
	int n = 0;
	int timeout;
	unsigned char statues = FALSE;
	int read_len = 0;
    unsigned char num=0;
	int ret_len = 0;
	unsigned char nbit = 0;
	unsigned char cmd[64] = {0};	
	unsigned int rec_len;        
	unsigned char rec_buf[64];  
    unsigned char pupi[4];
	unsigned char read_buf[512];
    unsigned char picc_atqa[2];                           // 储存寻卡返回卡片类型信息 
    unsigned char picc_sak[3];                            // 储存卡片应答信息
	unsigned char id[64] = {0};     
	unsigned char sector_num;
	unsigned char read_nbyte;
	unsigned char sector_buff[48];
	unsigned char *Pbuf = buff;
	
	if(fd < 0 || !key || !buff || len <= 0)
	{
		printf("please check Parameters\n");
		return 0;
	}
	
	Check_Machine(fd);
	
	if(machine_type == 1)
	{
		//typeA
		usleep(1000);
		pcd_RST(fd);
		FM175X_SoftReset( fd);             // FM175xx软件复位      
	//	printf(">>>>>>>>>>start2\n");
		Set_Rf(fd, 3 );                   // 打开双天线              
		Pcd_ConfigISOType(fd, 0 );       // ISO14443寄存器初始化     
//		printf(">>>>>>>>>>111");
		while(num <2 ) {
			statues = TypeA_CardActive(fd, picc_atqa,id,picc_sak ,&nbit);   //激活卡片 		
			if ( statues == TRUE ) 
			{

				buff[0] = nbit;
				if(len < nbit)
				{
					//memcpy(&buff[1],id,len-1);
					return 0;
				}
				//memcpy(&buff[1],&nbit,1);
				memcpy(&buff[1],id,nbit);
				swp_uid(&buff[1],nbit);
				//Print(buff,nbit+1);
				if(picc_sak[0] & 0x04)
				{
					//buff[0] = TYPE_A_NFC;
					ret_len = nbit + 1;
					goto end;

					//*IC_type = TYPE_A_NFC;
				}
				else if(picc_sak[0] & 0x20)
				{
					//*IC_type = TYPE_A_CPU;
					//buff[0] = TYPE_A_CPU;
					if(!CPU_Rats(fd,0x0A,0x51,&rec_len,rec_buf))
					{
						ret_len = nbit + 1;
						printf("CPU_auth error!\n");
						goto end;
					}
					if(!CPU_auth(fd,key,&rec_len,rec_buf))
					{
						ret_len = nbit + 1;
						printf("CPU_Rats error!\n");
						goto end;
					}
		
					cmd[0] = 0x00;
					cmd[1] = 0x05;
					if(!CPU_File_Select(fd,cmd))
					{
						ret_len = nbit + 1;
						printf("CPU_File_Select error!\n");
						goto end;
					}

					//printf("read bin file!\n");
					cmd[0] = 0x00;
					cmd[1] = 0x00;
					
					read_len = CPU_ReadFlie(fd,cmd,rec_buf,2);
					if(read_len != 2)
					{
						ret_len = nbit + 1;
						goto end;
					}
					if(len < rec_buf[0]*256 + rec_buf[1] + 2)
					{
						printf("no enough space!\n");
						ret_len = nbit + 1;
						goto end;
					}
					read_len = CPU_ReadFlie(fd,cmd,&buff[nbit+1],rec_buf[0]*256 + rec_buf[1] + 2);
					
					if((buff[buff[nbit+1]*256 + buff[nbit+2] + nbit + 2] != GetByteBCC(&buff[nbit+3],buff[nbit+1]*256 + buff[nbit+2]-1)))
						return 0;
					
				}
				else
				{
					//*IC_type = TYPE_A_M;
					//buff[0] = TYPE_A_M;
					if(!Mifare_SectorRead(fd,M1_START_SECTOR,0x0,0x01,key,id,rec_buf))
					{
						printf("Mifare_SectorRead error!\n");
						ret_len = nbit + 1;
						goto end;
					}
					
					read_len = rec_buf[0] * 256 + rec_buf[1] + 2;
					if(read_len <= 2)
					{
						printf("read_len = %d\n",read_len);
						ret_len = nbit + 1;
						goto end;
					}
					else if(len < read_len)
					{
						printf("no enough space!\n");
						ret_len = nbit + 1;
						goto end;
					}
					printf("read_len = %d\n",read_len);
					sector_num = 0;
					Pbuf = &buff[nbit+1];
					do{
						read_nbyte = ONE_SEC_BYTE;
						if(read_len < ONE_SEC_BYTE)
						{
							read_nbyte = read_len;
						}
						
						if(!Mifare_SectorRead(fd,M1_START_SECTOR + sector_num,0x0,0x02,key,id,sector_buff))
						{
							printf("Mifare_SectorRead error!\n");
							ret_len = nbit + 2;
							goto end;
						}
						//Print(sector_buff,48);
						printf("read_nbyte=%d\n",read_nbyte);
						memcpy(Pbuf,sector_buff,read_nbyte);
						read_len -= read_nbyte;
						sector_num++;
					}while((read_len > 0)&&(Pbuf += read_nbyte));
					printf("read end\n");
					if((buff[buff[nbit+1]*256 + buff[nbit+2] + nbit + 2] != GetByteBCC(&buff[nbit+3],buff[nbit+1]*256 + buff[nbit+2]-1)))
						return 0;
					
				}
				ret_len = nbit + 3 + buff[nbit+1]*256 + buff[nbit+2];
				goto end;
			}
			else {
				
				num++;
			}           
		}
		         
		
		//Delay100us(1000);
		//typeB
		FM175X_SoftReset( fd);                                      // FM175xx软件复位       
		Pcd_ConfigISOType(fd, 1 );                              // ISO14443寄存器初始化      
		Set_Rf( fd,3 );                                         // 打开双天线                 
		statues = TypeB_WUP(fd,&rec_len,id,pupi);              // 寻卡                      
		if ( statues == TRUE ) {
			statues = TypeB_Select(fd,pupi,&rec_len,id); 
		}
		if ( statues == TRUE ) {
			//LED_RedOn();
			nbit = rec_len;
			//buff[0] = TYPE_B_ID;
			buff[0] = nbit;
			//memcpy(&buff[1],&nbit,1);
			
			statues = TypeB_GetUID(fd,&rec_len,&id[0]);
			if(statues == TRUE) {
				memcpy(&buff[1],id,buff[0]);
				swp_uid(&buff[1],buff[0]);
				printf("nbit = %d\n",nbit);
			
				ret_len = buff[0] +1;
				goto end;
			
			}
			else
			{
				ret_len = 0;
			}
			
		}
		Set_Rf(fd, 0 );                                                // 关闭天线   
	}
	else if(machine_type == 2 )
	{
		usleep(50000);
		n = serial_read(fd,buff,1);
		
		if( n > 0)
		{	
			if(buff[0] > 10)
			{
				tcflush(fd, TCIFLUSH);
				//printf("read_buf[0]=%x\n",read_buf[0]);
				return 0;
			}
			n = serial_read(fd,&buff[1],buff[0]);
			if(n > 0)
				ret_len = n + 1;
			tcflush(fd, TCIFLUSH);
			//printf("read_buf[0]=%x\n",read_buf[0]);
			/*buff[0] = read_buf[0];
			read_len = read_buf[0];
			Pbuf = buff;
			printf("read_len = %d",read_len);
			while(read_len > 0 && timeout < 10)
			{
				usleep(50000);
				Pbuf += n;
				n = read(fd,Pbuf,read_len);
				read_len -= n;
				timeout++;
				
			}
			/*do{
				Pbuf += n;
				n = read(fd,Pbuf,read_len);
				read_len -= n; 
				usleep(20000);
				timeout++;
			}while(read_len > 0 && timeout < 10);*/
			/*if(timeout < 10)
			{
				//tcflush(fd, TCIFLUSH);
				ret_len = read_buf[0] + 1;
			}
			else
			{
				ret_len = 0;
			}*/
						
		}
		//
		
		
	}
	else
	{	
		return 0;
	}
	
	

end:
	//printf("ret_len=%d\n",ret_len);
	return ret_len;
}
/*
int Get_Card_ID(int fd,unsigned char *id)
{
	int n;
	int i;
	unsigned char statues = TRUE;
	
    unsigned char num=0;
	unsigned char nbit = 0;
	uint32_t rec_len;
    unsigned char pupi[4];
	unsigned char read_buf[512];
    unsigned char picc_atqa[2];                           // 储存寻卡返回卡片类型信息 
    unsigned char picc_sak[3];                            // 储存卡片应答信息
	
	if(machine_type == 1)
	{
		//typeA
		FM175X_SoftReset( fd);             // FM175xx软件复位      
		Set_Rf(fd, 3 );                   // 打开双天线              
		Pcd_ConfigISOType(fd, 0 );       // ISO14443寄存器初始化     
		
		while(num <2 ) {
			statues = TypeA_CardActive(fd, picc_atqa,id,picc_sak ,&nbit);      // 激活卡片    
			if ( statues == TRUE ) {
				num = 0;
				TypeA_Halt(fd,0);       // 睡眠卡片   
				goto end;
				
				//printf("CardUID:0x");
				//UartSendUid(picc_atqa,id);    
				printf("\r\nTyteA_Test\n");     
				//return nbit;
				//memset(id,0x00,15);                    
			}
			else {
				nbit = 0;
				num++;
			}                    
		}
		//Delay100us(1000);
		//typeB
		FM175X_SoftReset( fd);                                      // FM175xx软件复位       
		Pcd_ConfigISOType(fd, 1 );                              // ISO14443寄存器初始化      
		Set_Rf( fd,3 );                                         // 打开双天线                 
		statues = TypeB_WUP(fd,&rec_len,id,pupi);              // 寻卡                      
		if ( statues == TRUE ) {
			statues = TypeB_Select(fd,pupi,&rec_len,id); 
		}
		if ( statues == TRUE ) {
			//LED_RedOn();
			nbit = rec_len;
			statues = TypeB_GetUID(fd,&rec_len,&id[0]);
			if(statues == TRUE) {
				//printf("CardUID:0x");                                  // 输出UID号码   
				//uartSendHex( id, 8 );
				printf("\r\nTyteB_Test\n");
				//return nbit;
				goto end;
			}
			else
			{
				nbit = 0;
			}
			
		}
		Set_Rf(fd, 0 );                                                // 关闭天线   
	}
	else if(machine_type == 2 )
	{
		if(n = read(fd,read_buf,512) > 0)
		{
			if((read_buf[0] == 0x04)&&(n > 4))
			{
				memcpy(id,read_buf,4);
				nbit = 4;
			}
			else if((read_buf[0] == 0x08)&&(n > 8))
			{
				memcpy(id,read_buf,8);
				nbit = 8;
			}
			else
			{
			
				memset(read_buf,0,512);
				nbit = 0;
			}
				
		}
		
	}
	else
	{
		printf("please check type!\n");
		return -1;
	}
	
	

end:
	return nbit;
	
}


unsigned char get_version(int fd)
{
	return Read_Reg(fd,VersionReg);
}*/




