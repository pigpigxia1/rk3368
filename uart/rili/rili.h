#ifndef _RI_UNI_H
#define _RI_UNI_H

#define CMD_MAX_LEN 1024
#define FRAM_HEAD   0x55
#define FRAM_TAIL   0xAA


extern int read_pack_rili(int fd,unsigned char *rx_buff,int len);
extern unsigned char xor_check(unsigned char *str,int len);




#endif