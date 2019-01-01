#ifndef TYPE_B_H
#define TYPE_B_H

//extern unsigned char PUPI[4];
extern unsigned char TypeB_Request(int fd ,unsigned int *rece_len,unsigned char *buff,unsigned char *card_sn);
extern unsigned char TypeB_Select(int fd,unsigned char *card_sn,unsigned int *rece_len,unsigned char *buff);
extern unsigned char TypeB_GetUID(int fd,unsigned int *rece_len,unsigned char *buff);
extern unsigned char TypeB_Halt(int fd,unsigned char *card_sn);
extern unsigned char TypeB_WUP(int fd,unsigned int *rece_len,unsigned char *buff,unsigned char *card_sn);

#endif

