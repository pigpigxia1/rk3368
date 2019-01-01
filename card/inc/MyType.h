// =======================================================================================
//  Copyright (c) 2011 Guangzhou Zhiyuan Electronics Co., Ltd. All rights reserved. 
//  http://www.ecardsys.com
//! @file       MyType.h
//! @author     ����ɭ
//! @date       2012/07/30
//! @version    V1.02
//! @brief      �Զ��������
//!					- V1.02 2012/07/30 ����ɭ ��stdint.h�İ����ŵ����ļ���������C51��stdint.h
//!												���Զ�ѡ����ʵ�stdint.h���ļ�
//!					- V1.01 2012/03/07 ����ɭ ����uintxx_t <-->p8��ת������
//!					- V1.00 2011/08/01 ����ɭ �����ļ�
// =======================================================================================
#ifndef __MY_TYPE_H
#define __MY_TYPE_H

//#include "../Config.h"
/*#ifndef  __CC_ARM
	#ifdef __C51__
		#include ".\C51\stdint.h"
	#else
		#include ".\stdint.h"
	#endif
#else
	#include <stdint.h>
#endif*/

#include <stdint.h>
#ifdef __cplusplus
    extern "C" {
#endif
// =======================================================================================
//!�������ݴ�С��ģʽ
//#define BIG_ENDIAN							//<! ������ģʽ
//#define LITTLE_ENDIAN							//<! ����С��ģʽ
//#define DEBUG
//#define RELEASE
// ======================================================================================

// =======================================================================================
			
#ifndef TRUE
	#define	TRUE	1
#endif

#ifndef FALSE
	#define FALSE	0
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL 	0ul
	#else
		#define NULL 	((void *) 0)
	#endif
#endif

typedef unsigned int		uint;
typedef	float				fp32;
typedef	double				fp64;				

//! Ϊ�������C51�����Ż���������ʹ���������ARM��PC�Ȼ�����
//! �ڷ�C51������û�ж���__C51__������Ҫ��C51�ض��Ĺؼ��ֶ���Ϊ��
#ifndef __C51__
	#define code
	#define data
	#define idata
	#define xdata
//	#define pdata								// uC/OS��ʹ���˸ùؼ���
	#ifdef __cplusplus
		typedef bool BOOL;
	#else
		typedef unsigned int BOOL;
	#endif			// 
		
#else
		typedef bit BOOL;
#endif				// __C51__

// =======================================================================================
//! @struct InputPara Buf�������
typedef struct InputPara
{
	unsigned long 		nBytes;					//<! ������ֽ���
	const void			*pBuf;					//<! ��������
}InputPara;										//<! Buf �������

//! @struct OutputPara Buf�������
typedef struct OutputPara
{											    
	unsigned long		nBufSize;				//<! Buf �ĵĴ�С
	unsigned long		nBytes;					//<! ����ֽ���
	void 				*pBuf;					//<! ���������
} OutputPara;									//<! Buf �������

//! @struct CoordinatePoint �����
typedef struct CoordinatePoint
{
	int x;											//!< ������		
	int y;											//!< ������
} CoordinatePoint;
// =======================================================================================
//! @brief			��������Ԫ�ظ����ĺ�
//! @param[in]		array			-- ����
//! @return			����Ԫ�ظ���
// =======================================================================================
#ifndef COUNT_OF
	#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))
#endif				// COUNT_OF
	
// =======================================================================================
//! @brief			����һ������ֵ�ĺ�
//! @param[in]		si				-- ����λ������λΪ1��λ����
//! @param[in]		sh				-- ������ʼλ��(��0��ʼ, ���Ƶ�λ��)
//! @return			����
// =======================================================================================
#define CREATE_BITS_MASK(si, sh)		((((uint32_t) 0xFFFFFFFF) >> (32 - (si))) << (sh))

// =======================================================================================
//! @brief			((va) &  (mask))�ĺ�
//! @param[in]		va				-- ������
//! @param[in]		mask			-- ����λ��
//! @return			((va) &  (mask))
// =======================================================================================
#define GET_BITS_FIELD(va, mask)		((va) &  (mask))

// =======================================================================================
//! @brief			(((sa) & (va)) | ((va) &  (mask))) �ĺ�
//! @param[in]		sa				-- Դ������
//! @param[in]		va				-- ������
//! @param[in]		mask			-- ����λ��
//! @return			(((sa) & (va)) | ((va) &  (mask)))
// =======================================================================================		
#define SET_BITS_FIELD(sa, va, mask)	(((sa) & ~(mask)) | ((va) &  (mask)))

// =======================================================================================
//! @brief			((va) &  ~(mask)) �ĺ�
//! @param[in]		sa				-- Դ������
//! @param[in]		mask			-- ����λ��
//! @return			((Sa) &  ~(mask))
// =======================================================================================		
#define CLR_BITS_FIELD(sa, mask)	((Sa) &  ~(mask))

// =======================================================================================
//! @brief			��ѭ��
//! @param[in]		f		TRUE -- ��ѭ����	FALSE -- �˳���ִ�С�
// =======================================================================================		

#ifndef ASSERT
	#ifdef DEBUG
		#define ASSERT(f)	while (!(f))
	#else
		#define ASSERT(f)
	#endif			// DEBUG
#endif				// ASSERT(f)

#ifndef ASCII_TO_UNICODE_LENGTH
	#define ASCII_TO_UNICODE_LENGTH(length)		((length) * 2)
#endif				// ASCII_TO_UNICODE_LENGTH

#ifndef ASCII_TO_UNICODE
	#define ASCII_TO_UNICODE(ascii)		(ascii), 0
#endif				// ASCII_TO_UNICODE

#ifndef WORD_TO_ABYTE
	#ifdef BIG_ENDIAN
		#define WORD_TO_ABYTE(wValue)		(unsigned char)(wValue >> 8), (unsigned char)(wValue)
	#else
		#define WORD_TO_ABYTE(wValue)		(unsigned char)(wValue), (unsigned char)(wValue >> 8)
	#endif			// BIG_ENDIAN
#endif				// WORD_TO_ABYTE
	
#ifndef DWORD_TO_ABYTE
	#ifdef BIG_ENDIAN
		#define DWORD_TO_ABYTE(dwValue)		(unsigned char)(dwValue >> 24), (unsigned char)(dwValue >> 16),\
											(unsigned char)(dwValue >> 8), (unsigned char)(dwValue)
	#else
		#define DWORD_TO_ABYTE(dwValue)		(unsigned char)(dwValue), (unsigned char)(dwValue >> 8),\
											(unsigned char)(dwValue >> 16), (unsigned char)(dwValue >> 24)	
	#endif			// BIG_ENDIAN
#endif				// DWORD_TO_ABYTE

#ifndef REPLACE_IP_ADDRESS
	#define REPLACE_IP_ADDRESS(nField1, nField2, nField3, nField4) \
			((unsigned long)((((unsigned char)(nField1)) << 24) | \
							 (((unsigned char)(nField2)) << 16) | \
							 (((unsigned char)(nField3)) <<  8) | \
							 (((unsigned char)(nField4))      )))
#endif				// REPLACE_IP_ADDRESS
	
//#define P8_TO_UINT8(p8, n)		p8##[##n##]
#ifndef UINT16_TO_P8
	#ifdef BIG_ENDIAN
		#define UINT16_TO_P8(v16, p8) \
			*((unsigned char *)(p8))		= (unsigned char)((v16) >> 8);\
			*((unsigned char *)(p8) + 1)	= (unsigned char)((v16))
	#else
		#define UINT16_TO_P8(v16, p8) \
			*((unsigned char *)(p8))		= (unsigned char)((v16));\
			*((unsigned char *)(p8) + 1)	= (unsigned char)((v16) >> 8)
 	#endif			// BIG_ENDIAN
#endif				// UINT16_TO_UINT8

#ifndef UINT32_TO_P8
	#ifdef BIG_ENDIAN
		#define UINT32_TO_P8(v32, p8) \
			*((unsigned char *)(p8))		= (unsigned char)((v32) >> 24);\
			*((unsigned char *)(p8) + 1)	= (unsigned char)((v32) >> 16);\
			*((unsigned char *)(p8) + 2)	= (unsigned char)((v32) >>  8);\
			*((unsigned char *)(p8) + 3)	= (unsigned char)((v32))
	#else
		#define UINT32_TO_P8(v32, p8) \
			*((unsigned char *)(p8))		= (unsigned char)((v32));\
			*((unsigned char *)(p8) + 1)	= (unsigned char)((v32) >>  8);\
			*((unsigned char *)(p8) + 2)	= (unsigned char)((v32) >> 16);\
			*((unsigned char *)(p8) + 3)	= (unsigned char)((v32) >> 24)
	#endif			// BIG_ENDIAN
#endif				// UINT32_TO_P8


#ifndef P8_TO_UINT16
	#ifdef BIG_ENDIAN
		#define P8_TO_UINT16(p8) \
			((unsigned short)((*((unsigned char *)(p8)    ) << 8) | \
							  (*((unsigned char *)(p8) + 1)     )))
	#else
		#define P8_TO_UINT16(p8) \
			((unsigned short)((*((unsigned char *)(p8)    )     ) | \
							  (*((unsigned char *)(p8) + 1) << 8)))
 	#endif			// BIG_ENDIAN
#endif				// P8_TO_UINT16

#ifndef P8_TO_UINT32
	#ifdef BIG_ENDIAN
		#define P8_TO_UINT32(p8) \
			((unsigned long)((*((unsigned char *)(p8)    ) << 24) | \
							 (*((unsigned char *)(p8) + 1) << 16) | \
							 (*((unsigned char *)(p8) + 2) <<  8) | \
							 (*((unsigned char *)(p8) + 3)      )))
	#else
		#define P8_TO_UINT32(p8) \
			((unsigned long)((*((unsigned char *)(p8)    )      ) | \
							 (*((unsigned char *)(p8) + 1) <<  8) | \
							 (*((unsigned char *)(p8) + 2) << 16) | \
							 (*((unsigned char *)(p8) + 3) << 24)))
 	#endif			// BIG_ENDIAN
#endif				// P8_TO_UINT32
//	#define P_EP(n) ((USB_EP_EVENT & (1 << (n))) ? USB_EndPoint##n : NULL)
// =======================================================================================
#ifdef __cplusplus
}
#endif

#endif              // __MY_TYPE_H
