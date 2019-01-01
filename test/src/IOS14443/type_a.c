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
/*
** ͷ�ļ�
*/
#include "fm175xx.h"
#include "type_a.h"
#include "Utility.h"
#include "string.h"
#include "uart.h"


/*********************************************************************************************************
** Function name:       TypeA_Request        
** Descriptions:        TypeA_Request��ƬѰ��    
** input parameters:    N/A
** output parameters:   pTagType[0],pTagType[1] =ATQA 
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeA_Request(int fd,unsigned char *pTagType)
{
    unsigned char  result,send_buff[1],rece_buff[64];
    unsigned int  rece_bitlen;  
    Clear_BitMask(fd,TxModeReg,0x80);                                      /* �ر�TX CRC                   */
    Clear_BitMask(fd,RxModeReg,0x80);                                      /* �ر�RX CRC                   */
    Set_BitMask(fd,RxModeReg, 0x08);                                       /* �ر�λ����                   */
    Clear_BitMask(fd,Status2Reg,0x08);                                     /* ������ܱ�־��ʹ������ͨ��   */
    Write_Reg(fd,BitFramingReg,0x07);                                      /* ���һ�ֽڷ���7λ            */
    send_buff[0] = 0x26;                                                /* Ѱ������ 0x26                */                  
    Pcd_SetTimer(fd,1);
    Clear_FIFO(fd);
    result = Pcd_Comm(fd,Transceive,send_buff,1,rece_buff,&rece_bitlen,0);
    if ((result == TRUE) && (rece_bitlen == 2*8)) {                     /* �������յ�2�ֽڷ�����Ϣ      */
        *pTagType     = rece_buff[0];
        *(pTagType+1) = rece_buff[1];
    }
    return result;
}

/*********************************************************************************************************
** Function name:       TypeA_WakeUp        
** Descriptions:        ��Ƭ����
** input parameters:    N/A
** output parameters:   pTagType[0],pTagType[1] =ATQA 
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeA_WakeUp(int fd,unsigned char *pTagType)
{
    unsigned char   result,send_buff[1],rece_buff[64];
    unsigned int   rece_bitlen;  
    Clear_BitMask(fd,TxModeReg,0x80);                                      /* �ر�TX CRC                   */
    Clear_BitMask(fd,RxModeReg,0x80);                                      /* �ر�RX CRC                   */
    Set_BitMask(fd,RxModeReg, 0x08);                                       /* ������С��4bit������         */
    Clear_BitMask(fd,Status2Reg,0x08);                                     /* ������ܱ�־��ʹ������ͨ��   */
    Write_Reg(fd,BitFramingReg,0x07);                                      /* ���һ�ֽڷ���7λ            */
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
** Descriptions:        �ú���ʵ�ֱ��濨Ƭ�յ������к� 
** input parameters:    Bytes: ������ͻ���ֽ�    
**                      Bits: ������ͻ��λ    
**                      length: �Ӆ�����UID���ݳ��� 
**                      buff:��������UID��Ϣ
** output parameters:   picc_uid����Ҫ�����UID��Ϣ
** Returned value:      void
*********************************************************************************************************/
void TypeA_Save_UID(unsigned char Bytes,unsigned char Bits,unsigned char length,             \
                      unsigned char *buff,unsigned char *picc_uid)
{
    unsigned char i;
    unsigned char temp1;
    unsigned char temp2;
      
    temp2=buff[0];                                                      /* ��һ�ν��յ��ĵ�һ����ЧUID  */
    temp1=picc_uid[Bytes-1];                                            /* ǰһ�ν��յ������һ����ЧUID*/
    switch (Bits) {                                                     /* ����ǰһ�εĳ�ͻλ��         */
                                                                        /* ������κϲ�temp1��tmep2     */
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
        picc_uid[Bytes-1]=temp1 | temp2;                                /* ���ǰ��������               */
        for(i=1;i<length;i++) {
            picc_uid[Bytes-1+i]=buff[i];                                /* �������ֽڶ�������UID��      */
    }

}

/*********************************************************************************************************
** Function name:       Set_BitFraming        
** Descriptions:        �ú���ʵ�ֶ��յ��Ŀ�Ƭ�����кŵ��ж�
** input parameters:    bytes: ��֪��UID�ֽ���  
**                      bits: �������֪UIDbits        
**                      length: �Ӆ�����UID���ݳ��� 
** output parameters:   NVB:����ͻ����
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
** Descriptions:        ��Ƭ����ͻ    
** input parameters:    selcode����Ƭѡ����� 0x93��0x95��0x97    
**                      picc_uid����ƬUID��
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeA_Anticollision(int fd,unsigned char selcode,unsigned char *picc_uid)
{
    unsigned char result,i;
    unsigned char send_buff[10];
    unsigned char rece_buff[64];
    unsigned int  rece_bitlen,bitCnt;                                   /* ����ʹ�� volatile            */
    unsigned char nBytes,Bits,Pre_nBytes,Pre_Bits;
    nBytes = 0;                                                         /* ��ǰ����ͻָ������ֽ���     */
    Bits = 0;                                                           /* ��ǰ����ͻָ�����λ��       */
    Pre_nBytes = 0;                                                     /* ����ͻָ���Ѿ������ֽ���     */
    Pre_Bits = 0;                                                       /* ����ͻָ���Ѿ�����λ��       */
    rece_bitlen = 0;                                                    /* ָ��ִ�з���λ��             */
    bitCnt=0;
    Clear_BitMask(fd,TxModeReg,0x80);                                      /* �ر�TX CRC                   */
    Clear_BitMask(fd,RxModeReg,0x80);                                      /* �ر�RX CRC                   */
    Clear_BitMask(fd,Status2Reg,0x08);                                     /* �����֤��־��ʹ������ͨ��   */
    Write_Reg(fd,BitFramingReg,0x00);                                      /* ���һ�ֽڷ���8λ            */
    Clear_BitMask(fd,CollReg,0x80);                                        /* �������ͻλ                 */
  
    send_buff[0] = selcode;                                             /* ����ͻ����                   */
    send_buff[1] = 0x20;                                                /* NVB ��ͻλ                   */
    Pcd_SetTimer(fd,5);
    result = Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
    if(result == TRUE) {
        for (i=0; i<5; i++) { 
            *(picc_uid+i)  = rece_buff[i];           
        }
    }
    bitCnt = rece_bitlen;                                               /* ���յ������ݳ���             */
        
    if(result==Anticollision){                                          /* ������ͻ                     */
        nBytes = bitCnt/8;                                              /* ���յ����ֽ���               */
        Bits  =  bitCnt%8;                                              /* ���յ���λ��                 */
        if(bitCnt%8) nBytes++;                                          /* �����һ�η��ص������ֽ���   */
            memcpy(picc_uid,rece_buff,nBytes);                          /* ������յ��Ĳ���UID          */
        Pre_nBytes += nBytes;                                           /* ������һ����Ч�ֽ���         */
        Pre_Bits = Bits;                                                /* ������һ����Чλ             */                 

        while(result==Anticollision){                               /* ����ͻ���̣�ֻҪ�г�ͻ��һֱ���� */
            send_buff[0] = selcode;                                     /* ѡ����                       */
            TypeA_Set_BitFraming(fd,Pre_nBytes-1,Pre_Bits,&send_buff[1]);  /* ����NVB����                  */
            memcpy(&send_buff[2],picc_uid,nBytes);                      /* ����Ч�ֽڿ���������buff     */
                                                      /* ��������ֽڷ��͵�λ���ͽ��յ����ݴ�ŵ���ʼλ */

        Pcd_SetTimer(fd,5);
        Clear_FIFO(fd);
        result = Pcd_Comm(fd,Transceive,send_buff,2+nBytes,rece_buff,&rece_bitlen,0);    
        nBytes = rece_bitlen/8;
        Bits   = rece_bitlen%8;   
        if(Bits)
            nBytes+=1;
        TypeA_Save_UID(Pre_nBytes,Pre_Bits,nBytes,rece_buff,picc_uid);
        
        bitCnt += rece_bitlen;
        Pre_nBytes =  bitCnt/8;                                         /* ������һ����Ч�ֽ���         */
        Pre_Bits = bitCnt%8;
        if(bitCnt%8) Pre_nBytes++; 
        }
    }
    if(picc_uid[4] != (picc_uid[0]^picc_uid[1]^picc_uid[2]^picc_uid[3])) /* ���UIDУ��                 */
        result = FALSE;    
    return result;
}

/*********************************************************************************************************
** Function name:       TypeA_Select        
** Descriptions:        ѡ��Ƭ
** input parameters:    selcode����Ƭѡ����� 0x93��0x95��0x97    
**                      pSnr����ƬUID��
**                      pSak����Ƭѡ��Ӧ��
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeA_Select(int fd,unsigned char selcode,unsigned char *pSnr,unsigned char *pSak)
{
    unsigned char   result,i,send_buff[7],rece_buff[64];
    unsigned int   rece_bitlen;
    Write_Reg(fd,BitFramingReg,0x00);
    Set_BitMask(fd,TxModeReg,0x80);                                        /* ��TX CRC                   */
    Set_BitMask(fd,RxModeReg,0x80);                                        /* �򿪽���RX ��CRCУ��         */
    Clear_BitMask(fd,Status2Reg,0x08);                                     /* �����֤��־λ               */
    
    send_buff[0] = selcode;                                             /* select����                   */
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
** Descriptions:        ��Ƭ˯��
** input parameters:    AnticollisionFlag ������֤��־ 
**                      AnticollisionFlag = 0 û����֤��ʹ������ͨ�ţ���Ҫ�����֤��־
**                      AnticollisionFlag = 1 ������֤����ͨ����ʹ������ͨ�ţ�����Ҫ�����֤��־
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeA_Halt(int fd,unsigned char AnticollisionFlag)
{
    unsigned char   result,send_buff[2],rece_buff[64];
    unsigned int   rece_bitlen;
    send_buff[0] = 0x50;
    send_buff[1] = 0x00;

    Write_Reg(fd,BitFramingReg,0x00);                                      /* �����һ�ֽ�8λ            */
    Set_BitMask(fd,TxModeReg,0x80);                                        /* ��TX CRC                   */
    Set_BitMask(fd,RxModeReg,0x80);                                        /* ��RX CRC                   */
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
** Descriptions:        ����ʵ��Ѱ������ͻ��ѡ��
** input parameters:    pTagType: ��Ƭ���� ATQA
**                      pSnr: ��ƬUID
**                      pSak: ��ƬӦ������ SAK
**						nbit: ��ƬID�ֽ���
** output parameters:   N/A
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char TypeA_CardActive(int fd,unsigned char *pTagType,unsigned char *pSnr,unsigned char *pSak,unsigned char *nbit)
{
    unsigned char   result;
    result=TypeA_Request(fd,pTagType);                                     /* Ѱ�� Standard */
        if (result==FALSE) {
        return FALSE;
    }

    if( (pTagType[0]&0xC0) == 0x00 ) {                                  /* M1��,ID��ֻ��4λ*/
        result=TypeA_Anticollision(fd,0x93,pSnr);
        if (result==FALSE) {
            return FALSE;
        }
        result=TypeA_Select(fd,0x93,pSnr,pSak);                            /* ѡ��UID */
        if (result == FALSE) {
            return FALSE;
        }  
		*nbit = 4;
    }
        
    if( (pTagType[0]&0xC0) == 0x40 )  {                                 /* ID����7λ */
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
		
    if( (pTagType[0]&0xC0) == 0x80 )  {                                 /* ID����10λ                   */
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


