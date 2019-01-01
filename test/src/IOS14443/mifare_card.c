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
** File name:           mifare_card.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        mifareϵ�п���ز���
**
**--------------------------------------------------------------------------------------------------------
*/
#include "mifare_card.h"
#include "fm175xx.h"
#include "Utility.h"

/*********************************************************************************************************
** Function name:       Mifare_Auth        
** Descriptions:        mifare��ƬУ��
** input parameters:    mode����֤ģʽ��0��key A��֤��1��key B��֤��
**                      sector����֤�������ţ�0~15��
**                      *mifare_key��6�ֽ���֤��Կ����
**                      card_uid 4�ֽڿ�ƬUID����        
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
 unsigned char Mifare_Auth(int fd,unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{
    unsigned char  send_buff[12],rece_buff[1],result;
    unsigned int  rece_bitlen;
    if(mode==0x0)
        send_buff[0]=0x60;                                              /* kayA��֤ 0x60                */
    if(mode==0x1)
        send_buff[0]=0x61;                                              /* keyB��֤                     */
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
    result =Pcd_Comm(fd,MFAuthent,send_buff,12,rece_buff,&rece_bitlen,0);    /* Authent��֤                  */
    if (result==TRUE) {
        if(Read_Reg(fd,Status2Reg)&0x08)                                   /* �жϼ��ܱ�־λ��ȷ����֤��� */
            return TRUE;
        else
            return FALSE;
    }
    return FALSE;
}

/*********************************************************************************************************
** Function name:       Mifare_Blockset        
** Descriptions:        mifare�����ÿ�Ƭ����ֵ
** input parameters:    block:���
**                      buff:4�ֽڳ�ʼֵ
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
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
** Descriptions:        mifare����ȡ�麯��
** input parameters:    block:���   0x00-0x3f       
** output parameters:   buff:����16�ֽ����� 
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockread(int fd,unsigned char block,unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,1);
    send_buff[0]=0x30;                                                  /* ����������                   */
    send_buff[1]=block;                                                 /* ��������ַ                   */
    Clear_FIFO(fd);
    result =Pcd_Comm(fd,Transceive,send_buff,2,buff,&rece_bitlen,0);//
    if ((result!=TRUE )|(rece_bitlen!=16*8))                            /* ���յ������ݳ���Ϊ16         */
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       Mifare_SectorRead        
** Descriptions:        mifare��д�麯��
** input parameters:    sector:������   0x00-0x0f   
**						start_block:    0x00-0x03
**                      end_block:      0x00-0x03
**                      buff:����16�ֽ����� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
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
** Descriptions:        mifare��д�麯��
** input parameters:    block:���   0x00-0x3f    
**                      buff:����16�ֽ����� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockwrite(int fd,unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[16],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,1);
    send_buff[0]=0xa0;                                                  /* д������                     */
    send_buff[1]=block;                                                 /* ���ַ                       */

    Clear_FIFO(fd);
    result =Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* ���δ���յ�0x0A����ʾ��ACK  */
        return(FALSE);

    Pcd_SetTimer(fd,5);
    Clear_FIFO(fd);
    result =Pcd_Comm(fd,Transceive,buff,16,rece_buff,&rece_bitlen,0);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       mifare_SectorWrite        
** Descriptions:        mifare��д�麯��
** input parameters:    sector:������   0x00-0x0f   
**						start_block:    0x00-0x03
**                      end_block:      0x00-0x03
**                      buff:����16�ֽ����� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
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
** Descriptions:        mifare��д����
** input parameters:    block:��ʼ���   0x01-0x3f    
**                      buff:д���ݻ��� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
/*unsigned char Mifare_Write(int fd,unsigned char start_block,unsigned char *mifare_key,unsigned char *card_uid,unsigned char *buff,int len)
{
	unsigned char block_num = 0;
	unsigned char last_block;
	unsigned char dat[16] = {0};          //���ڴ��������16�ֽ�����
	unsigned char sector_num = 0;
	unsigned char last_num = 0;
	unsigned char *Pbuf = buff;
	unsigned char i;
	
	sector_num = start_block >> 2;        //��ʼ����
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
** Descriptions:        mifare��Ƭ��ֵ����    
** input parameters:    block:���   0x00-0x3f    
**                      buff:����4�ֽ����� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockinc(int fd,unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[2],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,5);
    send_buff[0]=0xc1;                                                  /* ��ֵ��������                 */
    send_buff[1]=block;                                                 /* ���ַ                       */
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    Pcd_SetTimer(fd,5);
    Clear_FIFO(fd);
    Pcd_Comm(fd,Transceive,buff,4,rece_buff,&rece_bitlen,0);
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Blockdec        
** Descriptions:        mifare��Ƭ��ֵ����    
** input parameters:    block:���   0x00-0x3f    
**                      buff:����4�ֽ����� 
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Blockdec(int fd,unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[2],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,5);
    send_buff[0]=0xc0;
    send_buff[1]=block;                                                 /* ���ַ                       */
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    Pcd_SetTimer(fd,5);
    Clear_FIFO(fd);
    Pcd_Comm(fd,Transceive,buff,4,rece_buff,&rece_bitlen,0);
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Transfer        
** Descriptions:        mifare��Ƭ�����
** input parameters:    block:���   0x00-0x3f    
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Transfer(int fd,unsigned char block)
{    
    unsigned char   result,send_buff[2],rece_buff[1];
    unsigned int   rece_bitlen;
    Pcd_SetTimer(fd,5);
    send_buff[0] = 0xb0;
    send_buff[1] = block;                                               /* ���ַ                       */
    Clear_FIFO(fd);
    result=Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Restore        
** Descriptions:        mifare��Ƭ��������
** input parameters:    block:���   0x00-0x3f    
** output parameters:   
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char Mifare_Restore(int fd,unsigned char block)
{    
    unsigned char result,send_buff[4],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,5);
    send_buff[0]=0xc2;
    send_buff[1]=block;                                                 /* ���ַ                       */
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* ���δ���յ�0x0A����ʾ��ACK  */
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
