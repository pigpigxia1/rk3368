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
** File name:           mifare_card.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        mifare系列卡相关操作
**
**--------------------------------------------------------------------------------------------------------
*/
#include "mifare_card.h"
#include "fm175xx.h"
#include "Utility.h"

/*********************************************************************************************************
** Function name:       Mifare_Auth        
** Descriptions:        mifare卡片校验
** input parameters:    mode，认证模式（0：key A认证，1：key B认证）
**                      sector，认证的扇区号（0~15）
**                      *mifare_key，6字节认证密钥数组
**                      card_uid 4字节卡片UID数组        
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
 unsigned char Mifare_Auth(int fd,unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{
    unsigned char  send_buff[12],rece_buff[1],result;
    unsigned int  rece_bitlen;
    if(mode==0x0)
        send_buff[0]=0x60;                                              /* kayA认证 0x60                */
    if(mode==0x1)
        send_buff[0]=0x61;                                              /* keyB认证                     */
    send_buff[1] = sector*4;
    send_buff[2] = mifare_key[0];
    send_buff[3] = mifare_key[1];
    send_buff[4] = mifare_key[2];
    send_buff[5] = mifare_key[3];
    send_buff[6] = mifare_key[4];
    send_buff[7] = mifare_key[5];
    send_buff[8] = card_uid[0];
    send_buff[9] = card_uid[1];
    send_buff[10] = card_uid[2];
    send_buff[11] = card_uid[3];

    Pcd_SetTimer(fd,1);
    Clear_FIFO(fd);
    result =Pcd_Comm(fd,MFAuthent,send_buff,12,rece_buff,&rece_bitlen,0);    /* Authent认证                  */
    if (result==TRUE) {
        if(Read_Reg(fd,Status2Reg)&0x08)                                   /* 判断加密标志位，确认认证结果 */
            return TRUE;
        else
            return FALSE;
    }
    return FALSE;
}

/*********************************************************************************************************
** Function name:       Mifare_Blockset        
** Descriptions:        mifare卡设置卡片计数值
** input parameters:    block:块号
**                      buff:4字节初始值
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockset(int fd,unsigned char block,unsigned char *buff)
{
    unsigned char  block_data[16],result;
    block_data[0]=buff[3];
    block_data[1]=buff[2];
    block_data[2]=buff[1];
    block_data[3]=buff[0];
    block_data[4]=~buff[3];
    block_data[5]=~buff[2];
    block_data[6]=~buff[1];
    block_data[7]=~buff[0];
    block_data[8]=buff[3];
    block_data[9]=buff[2];
    block_data[10]=buff[1];
    block_data[11]=buff[0];
    block_data[12]=block;
    block_data[13]=~block;
    block_data[14]=block;
    block_data[15]=~block;
    result= Mifare_Blockwrite(fd,block,block_data);
    return result;
}

/*********************************************************************************************************
** Function name:       Mifare_Blockread        
** Descriptions:        mifare卡读取块函数
** input parameters:    block:块号   0x00-0x3f       
** output parameters:   buff:读出16字节数据 
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockread(int fd,unsigned char block,unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,1);
    send_buff[0]=0x30;                                                  /* 读操作命令                   */
    send_buff[1]=block;                                                 /* 读操作地址                   */
    Clear_FIFO(fd);
    result =Pcd_Comm(fd,Transceive,send_buff,2,buff,&rece_bitlen,0);//
    if ((result!=TRUE )|(rece_bitlen!=16*8))                            /* 接收到的数据长度为16         */
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       Mifare_SectorRead        
** Descriptions:        mifare卡写块函数
** input parameters:    sector:扇区号   0x00-0x0f   
**						start_block:    0x00-0x03
**                      end_block:      0x00-0x03
**                      buff:读出16字节数据 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_SectorRead(int fd,unsigned char sector,unsigned char start_block,unsigned char end_block,unsigned char *mifare_key,unsigned char *card_uid,unsigned char *buff)
{
	unsigned char *Pbuf = buff;
	unsigned char block ;
	
	if( start_block > end_block)
	{
		return FALSE;
	}
	if(Mifare_Auth(fd,0x00,sector,mifare_key,card_uid))
	{
		block = sector*4 + start_block;
		
		do{
			if(!Mifare_Blockread(fd,block,Pbuf))
			{
				return FALSE;
			}
			//Print(Pbuf,16);
			start_block++;
			block++;
			//if(start_block <= end_block )
			//	Pbuf += 16;
			
		}while((start_block <= end_block) && (Pbuf += 16));
		return TRUE;
	}
	
	return FALSE;
}

/*********************************************************************************************************
** Function name:       mifare_blockwrite        
** Descriptions:        mifare卡写块函数
** input parameters:    block:块号   0x00-0x3f    
**                      buff:读出16字节数据 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockwrite(int fd,unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[16],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,1);
    send_buff[0]=0xa0;                                                  /* 写块命令                     */
    send_buff[1]=block;                                                 /* 块地址                       */

    Clear_FIFO(fd);
    result =Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* 如果未接收到0x0A，表示无ACK  */
        return(FALSE);

    Pcd_SetTimer(fd,5);
    Clear_FIFO(fd);
    result =Pcd_Comm(fd,Transceive,buff,16,rece_buff,&rece_bitlen,0);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       mifare_SectorWrite        
** Descriptions:        mifare卡写块函数
** input parameters:    sector:扇区号   0x00-0x0f   
**						start_block:    0x00-0x03
**                      end_block:      0x00-0x03
**                      buff:读出16字节数据 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_SectorWrite(int fd,unsigned char sector,unsigned char start_block,unsigned char end_block,unsigned char *mifare_key,unsigned char *card_uid,unsigned char *buff)
{
	unsigned char *Pbuf = buff;
	unsigned char block ;
	
	if(end_block == 0)
	{
		return FALSE;
	}
	if(Mifare_Auth(fd,0x00,sector,mifare_key,card_uid))
	{
		block = sector*4 + start_block;
		
		do{
			Mifare_Blockwrite(fd,block,Pbuf);
			start_block++;
			block++;
			//if(start_block <= end_block )
			//	Pbuf += 16;
			
		}while((start_block <= end_block)&&(Pbuf += 16));
		return TRUE;
	}
	
	return FALSE;
}
/*********************************************************************************************************
** Function name:       Mifare_Write        
** Descriptions:        mifare卡写函数
** input parameters:    block:起始块号   0x01-0x3f    
**                      buff:写数据缓存 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
/*unsigned char Mifare_Write(int fd,unsigned char start_block,unsigned char *mifare_key,unsigned char *card_uid,unsigned char *buff,int len)
{
	unsigned char block_num = 0;
	unsigned char last_block;
	unsigned char dat[16] = {0};          //用于处理最后不足16字节数据
	unsigned char sector_num = 0;
	unsigned char last_num = 0;
	unsigned char *Pbuf = buff;
	unsigned char i;
	
	sector_num = start_block >> 2;        //起始扇区
	last_num = len % 16;                   
	block_num = len / 16;
	
	if(last_num != 0)
	{
		block_num += 1;
		memcpy(dat,Pbuf+block_num*16,last_num);
			
	}
	last_block = start_block + block_num;
	
	do{
		if()
		Mifare_Auth(fd,0x00,sector_num,mifare_key,card_uid)
		
		start_block++;
		
	}while(start_block < last_block)
	
}*/
/*********************************************************************************************************
** Function name:       Mifare_Blockinc        
** Descriptions:        mifare卡片增值操作    
** input parameters:    block:块号   0x00-0x3f    
**                      buff:增加4字节数据 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockinc(int fd,unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[2],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,5);
    send_buff[0]=0xc1;                                                  /* 增值操作命令                 */
    send_buff[1]=block;                                                 /* 块地址                       */
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    Pcd_SetTimer(fd,5);
    Clear_FIFO(fd);
    Pcd_Comm(fd,Transceive,buff,4,rece_buff,&rece_bitlen,0);
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Blockdec        
** Descriptions:        mifare卡片减值操作    
** input parameters:    block:块号   0x00-0x3f    
**                      buff:减少4字节数据 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockdec(int fd,unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[2],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,5);
    send_buff[0]=0xc0;
    send_buff[1]=block;                                                 /* 块地址                       */
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    Pcd_SetTimer(fd,5);
    Clear_FIFO(fd);
    Pcd_Comm(fd,Transceive,buff,4,rece_buff,&rece_bitlen,0);
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Transfer        
** Descriptions:        mifare卡片传输块
** input parameters:    block:块号   0x00-0x3f    
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Transfer(int fd,unsigned char block)
{    
    unsigned char   result,send_buff[2],rece_buff[1];
    unsigned int   rece_bitlen;
    Pcd_SetTimer(fd,5);
    send_buff[0] = 0xb0;
    send_buff[1] = block;                                               /* 块地址                       */
    Clear_FIFO(fd);
    result=Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Restore        
** Descriptions:        mifare卡片储存命令
** input parameters:    block:块号   0x00-0x3f    
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Restore(int fd,unsigned char block)
{    
    unsigned char result,send_buff[4],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,5);
    send_buff[0]=0xc2;
    send_buff[1]=block;                                                 /* 块地址                       */
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    Pcd_SetTimer(fd,5);
    send_buff[0]=0x00;
    send_buff[1]=0x00;
    send_buff[2]=0x00;
    send_buff[3]=0x00;
    Clear_FIFO(fd);
    Pcd_Comm(fd,Transceive,send_buff,4,rece_buff,&rece_bitlen,0);
    return result;
}
