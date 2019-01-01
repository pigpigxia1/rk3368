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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "uart.h"
#include "exception.h"
//#include "cpu_card.h"


static unsigned char machine_type = 0;         //1:new  2:old


unsigned char Check_Machine(int fd,int gpio)
{
	if(machine_type == 0)
	{
		if(rename(LOG_FILE, LOG_FILE_B) < 0)
		{
			printf("rename failed\n");
		}
		pcd_RST(fd,gpio);
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



/****************************************************************
名称: Write_Card_Info                           
功能: 写数据到卡中                                   
输入:  new_key：新密钥
		buff：写入数据缓存，包括两字节长度信息和一字节数据校验信息																													
                                                                                                                                  
输出:                                                  
                                      
         TRUE: 应答正确                                            
         ERROR: 应答错误                                            
*****************************************************************/
/*
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
}*/

void HextoStr (unsigned char *pucStr,unsigned char num,unsigned char *str)
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
		
		*str++ = hChar;
		*str++ = lChar;
    }
	*str = '\0';
}

static void swp_uid(unsigned char *uid_data,int n)
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

static void create_uid(unsigned char *picc_atqa,unsigned char *picc_uid,unsigned char *buff)
{
	if( ( picc_atqa[0] & 0xc0 ) == 0x00 )	{							/* 4个字节UID					*/
		memcpy(buff,picc_uid,4);
	}
	if( ( picc_atqa[0] & 0xc0 ) == 0x40 )	{							/* 7字节UID 					*/
		memcpy(buff,picc_uid+1,3);

		memcpy(buff+3,picc_uid+5 ,4);
	}
	if( ( picc_atqa[0] & 0xc0 ) == 0x80 )	{
		memcpy(buff,picc_uid+1 ,3);
		memcpy(buff+3,picc_uid+5 ,3);
		memcpy(buff+6,picc_uid+10 ,4);
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
int Read_Card(int fd,unsigned char *key,unsigned char *buff,int len,int gpio)
{
	int n = 0;

	unsigned char statues = FALSE;

    unsigned char num=0;
	int ret_len = 0;
	unsigned char nbit = 0;
	unsigned int rec_len;        
	unsigned char rec_buf[64];  
    unsigned char pupi[4];
	unsigned char read_buf[512];
    unsigned char picc_atqa[2];                           // 储存寻卡返回卡片类型信息 
    unsigned char picc_sak[3];                            // 储存卡片应答信息
	unsigned char id[64] = {0};     
	unsigned char log_buf[64];


	if(fd <= 0 || !key || !buff || len <= 0)
	{
		printf("please check Parameters\n");
		return 0;
	}
	
	Check_Machine(fd,gpio);
	
	if(machine_type == 1)
	{
		//try{
			usleep(1000);
			pcd_RST(fd,gpio);
			FM175X_SoftReset( fd);             // FM175xx软件复位      
			Set_Rf(fd, 3 );                   // 打开双天线              
			Pcd_ConfigISOType(fd, 0 );       // ISO14443寄存器初始化     
			while(num <2 ) {
				try{
					statues = TypeA_CardActive(fd, picc_atqa,id,picc_sak ,&nbit);   //激活卡片 	
				}catch(MANPROCSIG_SEGV){ //Catch the exception
					//printf("NULL pointer !!\r\n");
				}finally{
					//printf("DONE \r\n");
				}end_try; 	
				if ( statues == TRUE ) 
				{

					buff[0] = nbit;
					if(len < nbit)
					{
						//memcpy(&buff[1],id,len-1);
						return 0;
					}

					create_uid(picc_atqa,id,&buff[1]);
					swp_uid(&buff[1],nbit);
					ret_len = nbit + 1;
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
			try{
				statues = TypeB_WUP(fd,&rec_len,id,pupi);              // 寻卡                      
				if ( statues == TRUE ) {
					statues = TypeB_Select(fd,pupi,&rec_len,id); 
				}
			}catch(MANPROCSIG_SEGV){ //Catch the exception
					//printf("NULL pointer !!\r\n");
			}finally{
				//printf("DONE \r\n");
			}end_try; 	
			if ( statues == TRUE ) {
				//LED_RedOn();
				nbit = rec_len;
				//buff[0] = TYPE_B_ID;
				buff[0] = nbit;
				//memcpy(&buff[1],&nbit,1);
				try{
					statues = TypeB_GetUID(fd,&rec_len,&id[0]);
				}catch(MANPROCSIG_SEGV){ //Catch the exception
						//printf("NULL pointer !!\r\n");
				}finally{
					//printf("DONE \r\n");
				}end_try; 	
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

		/* }catch(MANPROCSIG_SEGV){ //Catch the exception
			printf("NULL pointer !!\r\n");
		}catch_else{
			printf("Unknow Error!!\r\n");
		}finally{
			//printf("DONE \r\n");
		}end_try; */
		//typeA
		
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

	if(ret_len > 0)
	{
		HextoStr(buff,ret_len,log_buf);
		log_write(log_buf);
	}
	if(ret_len != buff[0] + 1)
		return 0;
	//printf("ret_len=%d\n",ret_len);
	return ret_len;
}




