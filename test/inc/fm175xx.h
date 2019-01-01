#ifndef FM175X_H
#define FM175X_H

#define Anticollision   0x02

#define CommandReg    0x01
#define ComIEnReg    0x02
#define DivIEnReg    0x03
#define ComIrqReg    0x04
#define DivIrqReg    0x05
#define ErrorReg    0x06
#define Status1Reg    0x07
#define Status2Reg    0x08
#define FIFODataReg 0x09
#define FIFOLevelReg 0x0A
#define WaterLevelReg 0x0B
#define ControlReg    0x0C
#define BitFramingReg 0x0D
#define CollReg    0x0E
#define ModeReg 0x11
#define TxModeReg 0x12
#define RxModeReg 0x13
#define TxControlReg 0x14
#define TxAutoReg 0x15
#define TxSelReg 0x16
#define RxSelReg 0x17
#define RxThresholdReg 0x18
#define DemodReg 0x19
#define MfTxReg 0x1C
#define MfRxReg 0x1D
#define SerialSpeedReg 0x1F
#define CRCMSBReg 0x21
#define CRCLSBReg 0x22
#define ModWidthReg 0x24
#define GsNOffReg 0x23
#define TxBitPhaseReg 0x25
#define RFCfgReg 0x26
#define GsNReg 0x27
#define CWGsPReg 0x28
#define ModGsPReg 0x29
#define TModeReg 0x2A
#define TPrescalerReg 0x2B
#define TReloadMSBReg 0x2C
#define TReloadLSBReg 0x2D
#define TCounterValMSBReg 0x2E
#define TCounterValLSBReg 0x2F
#define TestSel1Reg 0x31
#define TestSel2Reg 0x32
#define TestPinEnReg 0x33
#define TestPinValueReg 0x34
#define TestBusReg 0x35
#define AutoTestReg 0x36
#define VersionReg 0x37
#define AnalogTestReg 0x38
#define TestDAC1Reg 0x39
#define TestDAC2Reg 0x3A
#define TestADCReg 0x3B

#define Idle    0x00
#define Mem        0x01
#define RandomID    0x02
#define CalcCRC    0x03
#define Transmit    0x04
#define NoCmdChange    0x07
#define Receive    0x08
#define Transceive    0x0C
#define MFAuthent    0x0E
#define SoftReset    0x0F

#define UART_SPEED_115200 0x7A

//unsigned char FM175XX_IRQ;
void pcd_Init(void);
void pcd_DeInit();
void pcd_RST(int fd,int gpio);
void SetSpeed(int fd,int speed);
unsigned char  MCU_TO_PCD_TEST(int fd,int gpio);
unsigned char Pcd_Comm( int fd,unsigned char Command, 
                               unsigned char *pInData, 
                               unsigned char InLenByte,
                               unsigned char *pOutData, 
                               unsigned int *pOutLenBit,int flag);
unsigned char Set_Rf(int fd,unsigned char mode);
unsigned char Read_Reg(int fd,unsigned char reg_add);
unsigned char Read_Reg_All(int fd,unsigned char *reg_value);
unsigned char Write_Reg(int fd,unsigned char reg_add,unsigned char reg_value);
void Write_FIFO(int fd,unsigned char length,unsigned char *fifo_data);
void Read_FIFO(int fd,unsigned char length,unsigned char *fifo_data);
unsigned char Clear_FIFO(int fd);
unsigned char Set_BitMask(int fd,unsigned char reg_add,unsigned char mask);
unsigned char Clear_BitMask(int fd,unsigned char reg_add,unsigned char mask);
unsigned char Pcd_ConfigISOType(int fd,unsigned char type);
unsigned char FM175X_SoftReset(int fd);
unsigned char FM175X_HardReset(int fd,int gpio);
unsigned char FM175X_SoftPowerdown(int fd);
unsigned char FM175X_HardPowerdown(void);
unsigned char Pcd_SetTimer(int fd,unsigned long delaytime);
unsigned char Read_Ext_Reg(int fd,unsigned char reg_add);
unsigned char Write_Ext_Reg(int fd,unsigned char reg_add,unsigned char reg_value);
#endif
