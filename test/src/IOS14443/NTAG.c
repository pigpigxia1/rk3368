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
** File name:           NTAG.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        ISO/IEC144443A TAG卡相关操作
**
**--------------------------------------------------------------------------------------------------------
*/
#include "fm175xx.h"
#include "Utility.h"
#include <string.h>

/*********************************************************************************************************
** Function name:       NtagBlockread        
** Descriptions:        Ntag卡片读操作    
** input parameters:    block，块号（0x00~0x3f
** output parameters:   buff：4字节数据输出指针    
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char NtagBlockread(unsigned char block,unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0x30;                                                  /* 读块命令                     */
    send_buff[1]=block;                                                 /* 读块地址                     */
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);//
    if ((result!=TRUE )|(rece_bitlen!=16*8))                            /* 接收到的数据长度为16         */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       NtagBlockwrite        
** Descriptions:        Ntag卡片写块操作    
** input parameters:    block，块号（0x00~0x3f
**                      buff，4字节写块数据数组    
** output parameters:   buff：计数值输出指针    
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char NtagBlockwrite(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[16],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0xa2;                                                  /* 0xa2 写块命令                */
    send_buff[1]=block;                                                 /* 块地址                       */
    send_buff[2]=buff[0];                                               /* 写块数据                     */
    send_buff[3]=buff[1];
    send_buff[4]=buff[2];
    send_buff[5]=buff[3];
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,6,rece_buff,&rece_bitlen);//
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))    //如果未接收到0x0A，表示无ACK
        return(FALSE);
    return(TRUE);
}


/*********************************************************************************************************
** Function name:       NtagBlockwriteCompatibility        
** Descriptions:        Ntag卡片写块操作
** input parameters:    block，块号（0x00~0x3f
**                      buff，16字节写块数据数组    
** output parameters:   buff：计数值输出指针    
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char NtagBlockwriteCompatibility(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[16],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0xa0;
    send_buff[1]=block;                                                 /* 块地址                       */

    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                      /* 如果未接收到0x0A，表示无ACK  */
    return(FALSE);

    Pcd_SetTimer(5);
    Clear_FIFO();
    result =Pcd_Comm(Transceive,buff,16,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       NtagCounterRead        
** Descriptions:        读取卡片计数值
** input parameters:    
** output parameters:   buff：计数值输出指针    
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char NtagCounterRead(unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0x39;
    send_buff[1]=0x02;
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);
    if ((result!=TRUE )|(rece_bitlen!=3*8)) 
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       NtagPWD_AUTH        
** Descriptions:        NtagPWD_AUTH卡片密码校验
** input parameters:    password 验证密码
**                      passwordACK 验证密码返回值
** output parameters:   buff：应答数据的指针    
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char NtagPWD_AUTH(unsigned char *password,unsigned char *passwordACK)
{    
    unsigned char  send_buff[6],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0x1b;                                                    /* 验证密码指令               */
    send_buff[1]=password[0];                                             /* 验证的密码                 */
    send_buff[2]=password[1];
    send_buff[3]=password[2];
    send_buff[4]=password[3];
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,5,passwordACK,&rece_bitlen);
    if ((result!=TRUE )|(rece_bitlen!=2*8))                               /* 接收到的数据长度为16       */
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       NtagReadSIG        
** Descriptions:        NtagReadSIG卡片信息
** input parameters:    N/A
** output parameters:   buff：应答数据的指针    
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char NtagReadSIG(unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(10);
    send_buff[0]=0x3C;                                                  /* 读取信息值命令               */
    send_buff[1]=0x00;                                                
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);
    if ((result!=TRUE )|(rece_bitlen!=32*8))                            /* 接收到的数据长度为32         */
        return FALSE;
    return TRUE;
}




































