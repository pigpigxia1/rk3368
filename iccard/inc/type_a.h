#ifndef __TYPEA_H
#define __TYPEA_H

//extern unsigned char  picc_atqa[2],picc_uid[15],picc_sak[3];

unsigned char TypeA_Request(int fd,unsigned char *pTagType);
unsigned char TypeA_WakeUp(int fd,unsigned char *pTagType);
unsigned char TypeA_Anticollision(int fd,unsigned char selcode,unsigned char *pSnr);
unsigned char TypeA_Select(int fd,unsigned char selcode,unsigned char *pSnr,unsigned char *pSak);
unsigned char TypeA_Halt(int fd,unsigned char AnticollisionFlag);
unsigned char TypeA_CardActive(int fd,unsigned char *pTagType,unsigned char *pSnr,unsigned char *pSak,unsigned char *nbit);

#endif