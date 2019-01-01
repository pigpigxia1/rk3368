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
** File name:           Utility.c
** Last modified Date:  2016-3-4
** Last Version:        V1.00
** Descriptions:        增加计算字节异或取反校验函数
**
**--------------------------------------------------------------------------------------------------------
*/
#include "Utility.h"
#include <unistd.h>
#include <stdio.h>


void Print(unsigned char *buff,int len)
{
	int i;
	printf("\nlen =%d\n",len);
	for(i = 0;i < len;i++)
	{
		printf("%x\t",buff[i]);
	}
	printf("\n");
}
/*********************************************************************************************************
** Function name:        GetByteSum
** Descriptions:        计数总和
** input parameters:    *p      -- 计算的数据
**                      nBytes  -- 字节数
** output parameters:   N/A
** Returned value:      字节累加和
*********************************************************************************************************/
uint32_t GetByteSum(const void *p, uint32_t nBytes)
{
    const uint8_t	*pBuf	= (const uint8_t *)p;
    uint32_t		sum		= 0;
    while (nBytes--) {
		    sum += *pBuf++;
    }
    return sum;
}


/*********************************************************************************************************
** Function name:        计算字节异或取反
** Descriptions:        延迟（100us为单位） 基于LPC824,24MHz
** input parameters:    *p      -- 计算的数据
**                      nBytes  -- 字节数
** output parameters:   N/A
** Returned value:      字节异或取反
*********************************************************************************************************/
uint8_t GetByteBCC(const void *p, uint32_t nBytes)
{
    const uint8_t *pBuf = (const uint8_t *)p;
    uint8_t		BCC = 0;
    uint32_t	i   = 0;

    for (i = 0; i < nBytes; i++) {
        BCC ^= *(pBuf + i);
    }
    return ~BCC;
}


/*********************************************************************************************************
** Function name:       Delay100us 
** Descriptions:        延迟（100us为单位） 基于LPC824,24MHz
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      
*********************************************************************************************************/
void Delay100us(uint32_t _100us)
{
    /*uint32_t i;
    for ( ; 0 != _100us; _100us--) {
        for(i = 0; i < 110; i++);
    }*/
	usleep(100 * _100us);
}
                        


