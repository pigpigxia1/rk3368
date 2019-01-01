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
** File name:           NTAG.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        ISO/IEC144443A TAG����ز���
**
**--------------------------------------------------------------------------------------------------------
*/
#include "fm175xx.h"
#include "Utility.h"
#include <string.h>

/*********************************************************************************************************
** Function name:       NtagBlockread        
** Descriptions:        Ntag��Ƭ������    
** input parameters:    block����ţ�0x00~0x3f
** output parameters:   buff��4�ֽ��������ָ��    
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char NtagBlockread(unsigned char block,unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0x30;                                                  /* ��������                     */
    send_buff[1]=block;                                                 /* �����ַ                     */
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);//
    if ((result!=TRUE )|(rece_bitlen!=16*8))                            /* ���յ������ݳ���Ϊ16         */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       NtagBlockwrite        
** Descriptions:        Ntag��Ƭд�����    
** input parameters:    block����ţ�0x00~0x3f
**                      buff��4�ֽ�д����������    
** output parameters:   buff������ֵ���ָ��    
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char NtagBlockwrite(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[16],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0xa2;                                                  /* 0xa2 д������                */
    send_buff[1]=block;                                                 /* ���ַ                       */
    send_buff[2]=buff[0];                                               /* д������                     */
    send_buff[3]=buff[1];
    send_buff[4]=buff[2];
    send_buff[5]=buff[3];
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,6,rece_buff,&rece_bitlen);//
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))    //���δ���յ�0x0A����ʾ��ACK
        return(FALSE);
    return(TRUE);
}


/*********************************************************************************************************
** Function name:       NtagBlockwriteCompatibility        
** Descriptions:        Ntag��Ƭд�����
** input parameters:    block����ţ�0x00~0x3f
**                      buff��16�ֽ�д����������    
** output parameters:   buff������ֵ���ָ��    
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char NtagBlockwriteCompatibility(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[16],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0xa0;
    send_buff[1]=block;                                                 /* ���ַ                       */

    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                      /* ���δ���յ�0x0A����ʾ��ACK  */
    return(FALSE);

    Pcd_SetTimer(5);
    Clear_FIFO();
    result =Pcd_Comm(Transceive,buff,16,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* ���δ���յ�0x0A����ʾ��ACK  */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       NtagCounterRead        
** Descriptions:        ��ȡ��Ƭ����ֵ
** input parameters:    
** output parameters:   buff������ֵ���ָ��    
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
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
** Descriptions:        NtagPWD_AUTH��Ƭ����У��
** input parameters:    password ��֤����
**                      passwordACK ��֤���뷵��ֵ
** output parameters:   buff��Ӧ�����ݵ�ָ��    
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char NtagPWD_AUTH(unsigned char *password,unsigned char *passwordACK)
{    
    unsigned char  send_buff[6],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0x1b;                                                    /* ��֤����ָ��               */
    send_buff[1]=password[0];                                             /* ��֤������                 */
    send_buff[2]=password[1];
    send_buff[3]=password[2];
    send_buff[4]=password[3];
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,5,passwordACK,&rece_bitlen);
    if ((result!=TRUE )|(rece_bitlen!=2*8))                               /* ���յ������ݳ���Ϊ16       */
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       NtagReadSIG        
** Descriptions:        NtagReadSIG��Ƭ��Ϣ
** input parameters:    N/A
** output parameters:   buff��Ӧ�����ݵ�ָ��    
** Returned value:      TRUE�������ɹ� ERROR������ʧ��    
*********************************************************************************************************/
unsigned char NtagReadSIG(unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(10);
    send_buff[0]=0x3C;                                                  /* ��ȡ��Ϣֵ����               */
    send_buff[1]=0x00;                                                
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);
    if ((result!=TRUE )|(rece_bitlen!=32*8))                            /* ���յ������ݳ���Ϊ32         */
        return FALSE;
    return TRUE;
}




































