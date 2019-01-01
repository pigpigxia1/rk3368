#ifndef __MCU_CTR_H
#define __MCU_CTR_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


#define OPEN_DOOR 		0x81
#define GET_ALARM 		0x82
#define SET_MODE_485 	0x83
#define RS485_TX 		0x84
#define RS485_RX 		0x85
#define WG_OOUT 		0x86
#define WDOG_EN         0x87
#define WDOG_FEED       0x88
#define REBOOT          0x89
#define GET_MAC_INFO 	0xF1


#define SUCESS			0X00
#define CMD_ERR			0x01
#define LEN_ERR         0x02
#define STA_ERR			0x03
#define TIM_OUT			0x04
#define PAT_ERR         0x05
#define ERR				0x0F			









#endif