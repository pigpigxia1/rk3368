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
/*
** 头文件
*/
#include "fm175xx.h"
#include "type_a.h"
#include "Utility.h"
#include "string.h"
#include "uart.h"


/*********************************************************************************************************
** Function name:       TypeA_Request        
** Descriptions:        TypeA_Request卡片寻卡    
** input parameters:    N/A
** output parameters:   pTagType[0],pTagType[1] =ATQA 
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char TypeA_Request(int fd,unsigned char *pTagType)
{
    unsigned char  result,send_buff[1],rece_buff[64];
    unsigned int  rece_bitlen;  
    Clear_BitMask(fd,TxModeReg,0x80);                                      /* 关闭TX CRC                   */
    Clear_BitMask(fd,RxModeReg,0x80);                                      /* 关闭RX CRC                   */
    Set_BitMask(fd,RxModeReg, 0x08);                                       /* 关闭位接收                   */
    Clear_BitMask(fd,Status2Reg,0x08);                                     /* 清除加密标志，使用明文通信   */
    Write_Reg(fd,BitFramingReg,0x07);                                      /* 最后一字节发送7位            */
    send_buff[0] = 0x26;                                                /* 寻卡命令 0x26                */                  
    Pcd_SetTimer(fd,1);
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,1,rece_buff,&rece_bitlen,0);
    if ((result == TRUE) && (rece_bitlen == 2*8)) {                     /* 正常接收到2字节返回信息      */
        *pTagType     = rece_buff[0];
        *(pTagType+1) = rece_buff[1];
    }
    return result;
}

/*********************************************************************************************************
** Function name:       TypeA_WakeUp        
** Descriptions:        卡片唤醒
** input parameters:    N/A
** output parameters:   pTagType[0],pTagType[1] =ATQA 
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char TypeA_WakeUp(int fd,unsigned char *pTagType)
{
    unsigned char   result,send_buff[1],rece_buff[64];
    unsigned int   rece_bitlen;  
    Clear_BitMask(fd,TxModeReg,0x80);                                      /* 关闭TX CRC                   */
    Clear_BitMask(fd,RxModeReg,0x80);                                      /* 关闭RX CRC                   */
    Set_BitMask(fd,RxModeReg, 0x08);                                       /* 不接收小于4bit的数据         */
    Clear_BitMask(fd,Status2Reg,0x08);                                     /* 清除加密标志，使用明文通信   */
    Write_Reg(fd,BitFramingReg,0x07);                                      /* 最后一字节发送7位            */
    send_buff[0] = 0x52;
    Pcd_SetTimer(fd,1);
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,1,rece_buff,&rece_bitlen,0);
    if ((result == TRUE) && (rece_bitlen == 2*8)) {
        *pTagType     = rece_buff[0];
        *(pTagType+1) = rece_buff[1];
    }
    return result;
}

/*********************************************************************************************************
** Function name:       Save_UID        
** Descriptions:        该函数实现保存卡片收到的序列号 
** input parameters:    Bytes: 产生冲突的字节    
**                      Bits: 产生冲突的位    
**                      length: 接収到的UID数据长度 
**                      buff:读回来的UID信息
** output parameters:   picc_uid：需要保存的UID信息
** Returned value:      void
*********************************************************************************************************/
void TypeA_Save_UID(unsigned char Bytes,unsigned char Bits,unsigned char length,             \
                      unsigned char *buff,unsigned char *picc_uid)
{
    unsigned char i;
    unsigned char temp1;
    unsigned char temp2;
      
    temp2=buff[0];                                                      /* 后一次接收到的第一个有效UID  */
    temp1=picc_uid[Bytes-1];                                            /* 前一次接收到的最后一个有效UID*/
    switch (Bits) {                                                     /* 更具前一次的冲突位置         */
                                                                        /* 决定如何合并temp1和tmep2     */
        case 0:
            temp1=0x00;
            Bytes=Bytes+1;
            break;
        case 1:
            temp2=temp2 & 0xFE;
            temp1=temp1 & 0x01;
            break;            
        case 2:
            temp2=temp2 & 0xFC;
            temp1=temp1 & 0x03;
            break;
        case 3:
            temp2=temp2 & 0xF8;
            temp1=temp1 & 0x07;
            break;
        case 4:
            temp2=temp2 & 0xF0;
            temp1=temp1 & 0x0F;
            break;
        case 5:
            temp2=temp2 & 0xE0;
            temp1=temp1 & 0x1F;
            break;
        case 6:
            temp2=temp2 & 0xC0;
            temp1=temp1 & 0x3F;
            break;
        case 7:
            temp2=temp2 & 0x80;
            temp1=temp1 & 0x7F;
            break;
        default:
            break;
        }
        picc_uid[Bytes-1]=temp1 | temp2;                                /* 结合前后两部分               */
        for(i=1;i<length;i++) {
            picc_uid[Bytes-1+i]=buff[i];                                /* 将后续字节都拷贝到UID中      */
    }

}

/*********************************************************************************************************
** Function name:       Set_BitFraming        
** Descriptions:        该函数实现对收到的卡片的序列号的判断
** input parameters:    bytes: 已知的UID字节数  
**                      bits: 额外的已知UIDbits        
**                      length: 接収到的UID数据长度 
** output parameters:   NVB:防冲突种类
** Returned value:      void
*********************************************************************************************************/
void TypeA_Set_BitFraming(int fd,unsigned char  bytes,unsigned char  bits,unsigned char *NVB)
{    
    switch(bytes) {
        case 0: 
            *NVB = 0x20;
            break;
        case 1:
            *NVB = 0x30;
            break;
        case 2:
            *NVB = 0x40;
            break;
        case 3:
            *NVB = 0x50;
            break;
        case 4:
            *NVB = 0x60;
            break;
        default:
            break;
    }    
    switch(bits) {
        case 0:
            Write_Reg(fd,BitFramingReg,0x00);
            break;
        case 1:
            Write_Reg(fd,BitFramingReg,0x11);
            *NVB = (*NVB | 0x01);
            break;
        case 2:
            Write_Reg(fd,BitFramingReg,0x22);
            *NVB = (*NVB | 0x02);
            break;
        case 3:
            Write_Reg(fd,BitFramingReg,0x33);
            *NVB = (*NVB | 0x03);
            break;
        case 4:
            Write_Reg(fd,BitFramingReg,0x44);
            *NVB = (*NVB | 0x04);
            break;
        case 5:
            Write_Reg(fd,BitFramingReg,0x55);
            *NVB = (*NVB | 0x05);
            break;
        case 6:
            Write_Reg(fd,BitFramingReg,0x66);
            *NVB = (*NVB | 0x06);
            break;
        case 7:
            Write_Reg(fd,BitFramingReg,0x77);
            *NVB = (*NVB | 0x07);
            break;
        default:
            break;
    }
}

/*********************************************************************************************************
** Function name:       TypeA_Anticollision        
** Descriptions:        卡片防冲突    
** input parameters:    selcode：卡片选择编码 0x93，0x95，0x97    
**                      picc_uid：卡片UID号
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char TypeA_Anticollision(int fd,unsigned char selcode,unsigned char *picc_uid)
{
    unsigned char result,i;
    unsigned char send_buff[10];
    unsigned char rece_buff[64];
    unsigned int  rece_bitlen,bitCnt;                                   /* 可以使用 volatile            */
    unsigned char nBytes,Bits,Pre_nBytes,Pre_Bits;
    nBytes = 0;                                                         /* 当前防冲突指令接收字节数     */
    Bits = 0;                                                           /* 当前防冲突指令接收位数       */
    Pre_nBytes = 0;                                                     /* 防冲突指令已经接收字节数     */
    Pre_Bits = 0;                                                       /* 防冲突指令已经接收位数       */
    rece_bitlen = 0;                                                    /* 指令执行返回位数             */
    bitCnt=0;
    Clear_BitMask(fd,TxModeReg,0x80);                                      /* 关闭TX CRC                   */
    Clear_BitMask(fd,RxModeReg,0x80);                                      /* 关闭RX CRC                   */
    Clear_BitMask(fd,Status2Reg,0x08);                                     /* 清除验证标志，使用明文通信   */
    Write_Reg(fd,BitFramingReg,0x00);                                      /* 最后一字节发送8位            */
    Clear_BitMask(fd,CollReg,0x80);                                        /* 清除防冲突位                 */
  
    send_buff[0] = selcode;                                             /* 防冲突命令                   */
    send_buff[1] = 0x20;                                                /* NVB 冲突位                   */
    Pcd_SetTimer(fd,5);
    result = Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if(result == TRUE) {
        for (i=0; i<5; i++) { 
            *(picc_uid+i)  = rece_buff[i];           
        }
    }
    bitCnt = rece_bitlen;                                               /* 接收到的数据长度             */
        
    if(result==Anticollision){                                          /* 产生冲突                     */
        nBytes = bitCnt/8;                                              /* 接收到的字节数               */
        Bits  =  bitCnt%8;                                              /* 接收到的位数                 */
        if(bitCnt%8) nBytes++;                                          /* 算出第一次返回的数据字节数   */
            memcpy(picc_uid,rece_buff,nBytes);                          /* 保存接收到的部分UID          */
        Pre_nBytes += nBytes;                                           /* 保存上一次有效字节数         */
        Pre_Bits = Bits;                                                /* 保存上一次有效位             */                 

        while(result==Anticollision){                               /* 防冲突过程，只要有冲突，一直运行 */
            send_buff[0] = selcode;                                     /* 选择码                       */
            TypeA_Set_BitFraming(fd,Pre_nBytes-1,Pre_Bits,&send_buff[1]);  /* 设置NVB发送                  */
            memcpy(&send_buff[2],picc_uid,nBytes);                      /* 将有效字节拷贝到发送buff     */
                                                      /* 设置最后字节发送的位，和接收的数据存放的起始位 */

        Pcd_SetTimer(fd,5);
        Clear_FIFO(fd);
        result = Pcd_Comm(fd,Transceive,send_buff,2+nBytes,rece_buff,&rece_bitlen,0);    
        nBytes = rece_bitlen/8;
        Bits   = rece_bitlen%8;   
        if(Bits)
            nBytes+=1;
        TypeA_Save_UID(Pre_nBytes,Pre_Bits,nBytes,rece_buff,picc_uid);
        
        bitCnt += rece_bitlen;
        Pre_nBytes =  bitCnt/8;                                         /* 保存上一次有效字节数         */
        Pre_Bits = bitCnt%8;
        if(bitCnt%8) Pre_nBytes++; 
        }
    }
    if(picc_uid[4] != (picc_uid[0]^picc_uid[1]^picc_uid[2]^picc_uid[3])) /* 异或UID校验                 */
        result = FALSE;    
    return result;
}

/*********************************************************************************************************
** Function name:       TypeA_Select        
** Descriptions:        选择卡片
** input parameters:    selcode：卡片选择编码 0x93，0x95，0x97    
**                      pSnr：卡片UID号
**                      pSak：卡片选择应答
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char TypeA_Select(int fd,unsigned char selcode,unsigned char *pSnr,unsigned char *pSak)
{
    unsigned char   result,i,send_buff[7],rece_buff[64];
    unsigned int   rece_bitlen;
    Write_Reg(fd,BitFramingReg,0x00);
    Set_BitMask(fd,TxModeReg,0x80);                                        /* 打开TX CRC                   */
    Set_BitMask(fd,RxModeReg,0x80);                                        /* 打开接收RX 的CRC校验         */
    Clear_BitMask(fd,Status2Reg,0x08);                                     /* 清除验证标志位               */
    
    send_buff[0] = selcode;                                             /* select命令                   */
    send_buff[1] = 0x70;                                                /* NVB                          */
    for (i=0; i<5; i++) {
        send_buff[i+2] = *(pSnr+i);
    }
    send_buff[6] = pSnr[0]^pSnr[1]^pSnr[2]^pSnr[3];
    Pcd_SetTimer(fd,1);
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,7,rece_buff,&rece_bitlen,0);
    if (result == TRUE) {
        *pSak=rece_buff[0]; 
    }
    return result;
}

/*********************************************************************************************************
** Function name:       TypeA_Halt        
** Descriptions:        卡片睡眠
** input parameters:    AnticollisionFlag 休眠验证标志 
**                      AnticollisionFlag = 0 没有验证，使用明文通信，需要清除验证标志
**                      AnticollisionFlag = 1 密码验证函数通过，使用密文通信，不需要清除验证标志
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char TypeA_Halt(int fd,unsigned char AnticollisionFlag)
{
    unsigned char   result,send_buff[2],rece_buff[64];
    unsigned int   rece_bitlen;
    send_buff[0] = 0x50;
    send_buff[1] = 0x00;

    Write_Reg(fd,BitFramingReg,0x00);                                      /* 最后发送一字节8位            */
    Set_BitMask(fd,TxModeReg,0x80);                                        /* 打开TX CRC                   */
    Set_BitMask(fd,RxModeReg,0x80);                                        /* 打开RX CRC                   */
    if( !AnticollisionFlag ) {
        Clear_BitMask(fd,Status2Reg,0x08);
    }
    Pcd_SetTimer(fd,1);
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transmit,send_buff,2,rece_buff,&rece_bitlen,0);
    return result;
}

/*********************************************************************************************************
** Function name:       TypeA_CardActive        
** Descriptions:        函数实现寻卡防冲突和选择卡
** input parameters:    pTagType: 卡片类型 ATQA
**                      pSnr: 卡片UID
**                      pSak: 卡片应答数据 SAK
**						nbit: 卡片ID字节数
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char TypeA_CardActive(int fd,unsigned char *pTagType,unsigned char *pSnr,unsigned char *pSak,unsigned char *nbit)
{
    unsigned char   result;
    result=TypeA_Request(fd,pTagType);                                     /* 寻卡 Standard */
        if (result==FALSE) {
        return FALSE;
    }

    if( (pTagType[0]&0xC0) == 0x00 ) {                                  /* M1卡,ID号只有4位*/
        result=TypeA_Anticollision(fd,0x93,pSnr);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Select(fd,0x93,pSnr,pSak);                            /* 选择UID */
        if (result == FALSE) {
            return FALSE;
        }  
		*nbit = 4;
    }
        
    if( (pTagType[0]&0xC0) == 0x40 )  {                                 /* ID号有7位 */
        result=TypeA_Anticollision(fd,0x93,pSnr);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Select(fd,0x93,pSnr,pSak);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Anticollision(fd,0x95,pSnr+5);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Select(fd,0x95,pSnr+5,pSak+1);
        if (result==FALSE) {
            return FALSE;
        }
		*nbit = 7;
    }
		
    if( (pTagType[0]&0xC0) == 0x80 )  {                                 /* ID号有10位                   */
        result=TypeA_Anticollision(fd,0x93,pSnr);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Select(fd,0x93,pSnr,pSak);
        if (result==FALSE) {
             return FALSE;
        }
        result=TypeA_Anticollision(fd,0x95,pSnr+5);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Select(fd,0x95,pSnr+5,pSak+1);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Anticollision(fd,0x97,pSnr+10);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Select(fd,0x97,pSnr+10,pSak+2);
        if (result==FALSE) {
            return FALSE;
        }
		*nbit = 10;
    }
    return result;
}


