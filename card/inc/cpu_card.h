#ifndef CPU_CARD_H
#define CPU_CARD_H


extern void CPU_PCB(void);
extern unsigned char CPU_BaudRate_Set(int fd,unsigned char mode);
extern unsigned char CPU_TPDU(int fd,unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_Rats(int fd,unsigned char param1,unsigned char param2,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_PPS(int fd,unsigned char pps0,unsigned char pps1,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_Mf_Erase(int fd,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_Slect(int fd,unsigned char *rece_len,unsigned char *buff);
extern unsigned char Creat_Bin_file(int fd,unsigned char *file);
extern unsigned char CPU_File_Select(int fd,unsigned char *file);
extern unsigned char CPU_Key_Create(int fd,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_Key_write(int fd,const unsigned char *key,unsigned char *rece_len,unsigned char *buff);
extern int CPU_WriteFlie(int fd,unsigned char *file,unsigned char *buff);
extern int CPU_ReadFlie(int fd,unsigned char *file,unsigned char *buff,int len);
extern unsigned char New_Cpu_Card_init(int fd,unsigned char *old_key,unsigned char *new_key);
extern unsigned char Check_RetValue(unsigned char *value,unsigned char len);
extern unsigned char CPU_Random(int fd,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_I_Block(int fd,unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_R_Block(int fd,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_S_Block(int fd,unsigned char pcb_byte,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_auth(int fd,unsigned char *key_buff,unsigned char *rece_len,unsigned char *buff);
#endif
