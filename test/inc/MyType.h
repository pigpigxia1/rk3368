// =======================================================================================
//  Copyright (c) 2011 Guangzhou Zhiyuan Electronics Co., Ltd. All rights reserved. 
//  http://www.ecardsys.com
//! @file       MyType.h
//! @author     曾本森
//! @date       2012/07/30
//! @version    V1.02
//! @brief      自定义的类型
//!					- V1.02 2012/07/30 曾本森 将stdint.h的包含放到该文件，并增加C51的stdint.h
//!												即自动选择合适的stdint.h的文件
//!					- V1.01 2012/03/07 曾本森 增加uintxx_t <-->p8的转换定义
//!					- V1.00 2011/08/01 曾本森 创建文件
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
//!定义数据大小端模式
//#define BIG_ENDIAN							//<! 定义大端模式
//#define LITTLE_ENDIAN							//<! 定义小端模式
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

//! 为了能针对C51进行优化，并且又使代码可用于ARM和PC等环境，
//! 在非C51环境（没有定义__C51__）下需要把C51特定的关键字定义为空
#ifndef __C51__
	#define code
	#define data
	#define idata
	#define xdata
//	#define pdata								// uC/OS中使用了该关键字
	#ifdef __cplusplus
		typedef bool BOOL;
	#else
		typedef unsigned int BOOL;
	#endif			// 
		
#else
		typedef bit BOOL;
#endif				// __C51__

// =======================================================================================
//! @struct InputPara Buf输入参数
typedef struct InputPara
{
	unsigned long 		nBytes;					//<! 输入的字节数
	const void			*pBuf;					//<! 输入数据
}InputPara;										//<! Buf 输入参数

//! @struct OutputPara Buf输入参数
typedef struct OutputPara
{											    
	unsigned long		nBufSize;				//<! Buf 的的大小
	unsigned long		nBytes;					//<! 输出字节数
	void 				*pBuf;					//<! 输出的数据
} OutputPara;									//<! Buf 输出参数

//! @struct CoordinatePoint 坐标点
typedef struct CoordinatePoint
{
	int x;											//!< 横坐标		
	int y;											//!< 纵坐标
} CoordinatePoint;
// =======================================================================================
//! @brief			计算数组元素个数的宏
//! @param[in]		array			-- 数组
//! @return			数组元素个数
// =======================================================================================
#ifndef COUNT_OF
	#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))
#endif				// COUNT_OF
	
// =======================================================================================
//! @brief			创建一个掩码值的宏
//! @param[in]		si				-- 掩码位数（低位为1的位数）
//! @param[in]		sh				-- 掩码起始位置(从0开始, 左移的位数)
//! @return			掩码
// =======================================================================================
#define CREATE_BITS_MASK(si, sh)		((((uint32_t) 0xFFFFFFFF) >> (32 - (si))) << (sh))

// =======================================================================================
//! @brief			((va) &  (mask))的宏
//! @param[in]		va				-- 操作数
//! @param[in]		mask			-- 掩码位数
//! @return			((va) &  (mask))
// =======================================================================================
#define GET_BITS_FIELD(va, mask)		((va) &  (mask))

// =======================================================================================
//! @brief			(((sa) & (va)) | ((va) &  (mask))) 的宏
//! @param[in]		sa				-- 源操作数
//! @param[in]		va				-- 操作数
//! @param[in]		mask			-- 掩码位数
//! @return			(((sa) & (va)) | ((va) &  (mask)))
// =======================================================================================		
#define SET_BITS_FIELD(sa, va, mask)	(((sa) & ~(mask)) | ((va) &  (mask)))

// =======================================================================================
//! @brief			((va) &  ~(mask)) 的宏
//! @param[in]		sa				-- 源操作数
//! @param[in]		mask			-- 掩码位数
//! @return			((Sa) &  ~(mask))
// =======================================================================================		
#define CLR_BITS_FIELD(sa, mask)	((Sa) &  ~(mask))

// =======================================================================================
//! @brief			死循环
//! @param[in]		f		TRUE -- 死循环；	FALSE -- 退出不执行。
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
