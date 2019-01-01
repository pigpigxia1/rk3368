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
** File name:           type_b.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        ISO/IEC144443B卡相关操作
**
**--------------------------------------------------------------------------------------------------------
*/

#include "fm175xx.h"
#include "type_b.h"
#include "Utility.h"
#include <string.h>



/*********************************************************************************************************
** Function name:       TypeB_Halt
** Descriptions:        设置TYPE B卡片进入停止状态
** input parameters:    card_sn：卡片的PUPI
** output parameters:   TRUE：操作成功
** Returned value:      ERROR：操作失败    
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
** Descriptions:        TYPE B卡片唤醒
** input parameters:    无
** output parameters:   rece_len:卡片应答的数据长度
**                      buff：卡片应答的数据指针    
**                      card_sn:卡片的PUPI字节
** Returned value:      TRUE：操作成功 ERROR：操作失败    
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
** Descriptions:        TYPE B卡片选卡
** input parameters:    无
** output parameters:   无
** Returned value:      TRUE：操作成功 ERROR：操作失败    
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
** Descriptions:        TYPE B卡片选卡    
** input parameters:    card_sn：卡片PUPI字节（4字节）
** output parameters:   rece_len：应答数据的长度
**                      buff：应答数据的指针    
** Returned value:      TRUE：操作成功 ERROR：操作失败    
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
    send_byte[6] = 0x08;                                                /* 01--24，08--256------Param2  */
    send_byte[7] = 0x01;                                                /* COMPATIBLE WITH 14443-4      */
    send_byte[8] = 0x02;                                                /* CID：01 ------Param4         */
         
    ret=Pcd_Comm(fd,Transceive,send_byte,9,buff,&(*rece_len),0);
    return (ret);
}    

/*********************************************************************************************************
** Function name:       TypeB_GetUID        
** Descriptions:        身份证专用指令
** input parameters:    N/A
** output parameters:   rece_len：应答数据的长度
**                      buff：应答数据的指针    
** Returned value:      TRUE：操作成功 ERROR：操作失败    
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

