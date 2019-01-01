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
** File name:           type_b.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        ISO/IEC144443B����ز���
**
**--------------------------------------------------------------------------------------------------------
*/

#include "fm175xx.h"
#include "type_b.h"
#include "Utility.h"
#include <string.h>



/*********************************************************************************************************
** Function name:       TypeB_Halt
** Descriptions:        ����TYPE B��Ƭ����ֹͣ״̬
** input parameters:    card_sn����Ƭ��PUPI
** output parameters:   TRUE�������ɹ�
** Returned value:      ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeB_Halt(int fd,unsigned char *card_sn)
{
    unsigned char  ret,send_byte[5],rece_byte[64],i;
    unsigned int  rece_len;
    Pcd_SetTimer(fd,10);
    send_byte[0] = 0x50;
    for(i = 0; i < 4; i++)
    {
        send_byte[i + 1] =card_sn[i];
    }
    ret=Pcd_Comm(fd,Transceive,send_byte,5,rece_byte,&rece_len,0);
    return (ret);
}

/*********************************************************************************************************
** Function name:       TypeB_WUP
** Descriptions:        TYPE B��Ƭ����
** input parameters:    ��
** output parameters:   rece_len:��ƬӦ������ݳ���
**                      buff����ƬӦ�������ָ��    
**                      card_sn:��Ƭ��PUPI�ֽ�
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeB_WUP(int fd,unsigned int *rece_len,unsigned char *buff,unsigned char *card_sn)
{                        
    unsigned char  ret,send_byte[3];
    Pcd_SetTimer(fd,10);
    send_byte[0]=0x05;                                                  /* APf                          */
    send_byte[1]=0x00;                                                  /* AFI (00:for all cards)       */
    send_byte[2]=0x08;                                                  /* PARAM(WUP,Number of slots =0)*/
    ret=Pcd_Comm(fd,Transceive,send_byte,3,buff,&(*rece_len),0);
    if (ret==TRUE)
        memcpy(card_sn,&buff[1],4);        
    return (ret);
}

/*********************************************************************************************************
** Function name:       TypeB_Request    
** Descriptions:        TYPE B��Ƭѡ��
** input parameters:    ��
** output parameters:   ��
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeB_Request(int fd,unsigned int *rece_len,unsigned char *buff,unsigned char *card_sn)
{            
    unsigned char data ret,send_byte[5];
    Pcd_SetTimer(fd,10);
    send_byte[0]=0x05;                                                    /* APf                          */
    send_byte[1]=0x00;                                                    /* AFI (00:for all cards)       */
    send_byte[2]=0x00;                                                    /* PARAM(REQB,Number of slots =0)*/
    ret=Pcd_Comm(fd,Transceive,send_byte,3,buff,rece_len,0);
    if (ret==TRUE)    
    memcpy(card_sn,buff+1,4);
    return (ret);
}        
/*********************************************************************************************************
** Function name:       TypeB_Select        
** Descriptions:        TYPE B��Ƭѡ��    
** input parameters:    card_sn����ƬPUPI�ֽڣ�4�ֽڣ�
** output parameters:   rece_len��Ӧ�����ݵĳ���
**                      buff��Ӧ�����ݵ�ָ��    
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeB_Select(int fd,unsigned char *card_sn,unsigned int *rece_len,unsigned char *buff)
{
    unsigned char data ret,send_byte[9],i;
    Pcd_SetTimer(fd,20);
    send_byte[0] = 0x1d;
    for(i = 0; i < 4; i++)
    {
        send_byte[i + 1] =card_sn[i];
    }
    send_byte[5] = 0x00;                                                /* Param1                       */
    send_byte[6] = 0x08;                                                /* 01--24��08--256------Param2  */
    send_byte[7] = 0x01;                                                /* COMPATIBLE WITH 14443-4      */
    send_byte[8] = 0x02;                                                /* CID��01 ------Param4         */
         
    ret=Pcd_Comm(fd,Transceive,send_byte,9,buff,&(*rece_len),0);
    return (ret);
}    

/*********************************************************************************************************
** Function name:       TypeB_GetUID        
** Descriptions:        ���֤ר��ָ��
** input parameters:    N/A
** output parameters:   rece_len��Ӧ�����ݵĳ���
**                      buff��Ӧ�����ݵ�ָ��    
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeB_GetUID(int fd,unsigned int *rece_len,unsigned char *buff)
{
    unsigned char data ret,send_byte[5];
    Pcd_SetTimer(fd,15);
    send_byte[0]=0x00;
    send_byte[1]=0x36;
    send_byte[2]=0x00;
    send_byte[3]=0x00;
    send_byte[4]=0x08;
    ret=Pcd_Comm(fd,Transceive,send_byte,5,buff,&(*rece_len),0);
    return (ret);
}    

