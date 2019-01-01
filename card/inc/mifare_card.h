#ifndef __MIFARE_CARD_H
#define __MIFARE_CARD_H


unsigned char Mifare_Transfer(int fd,unsigned char block);
unsigned char Mifare_Restore(int fd,unsigned char block);
unsigned char Mifare_Blockset(int fd,unsigned char block,unsigned char *buff);
unsigned char Mifare_Blockinc(int fd,unsigned char block,unsigned char *buff);
unsigned char Mifare_Blockdec(int fd,unsigned char block,unsigned char *buff);
unsigned char Mifare_Blockwrite(int fd,unsigned char block,unsigned char *buff);
unsigned char Mifare_Blockread(int fd,unsigned char block,unsigned char *buff);
unsigned char Mifare_Auth(int fd,unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid);
unsigned char Mifare_SectorRead(int fd,unsigned char sector,unsigned char start_block,unsigned char end_block,unsigned char *mifare_key,unsigned char *card_uid,unsigned char *buff);
unsigned char Mifare_SectorWrite(int fd,unsigned char sector,unsigned char start_block,unsigned char end_block,unsigned char *mifare_key,unsigned char *card_uid,unsigned char *buff);


#endif