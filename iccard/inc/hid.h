#ifndef _HID_UNI_H
#define _HID_UNI_H

#define CMD_MAX_LEN 64
#define FRAM_TAIL 0x0a
#define HID_CARD_LEN 6

int read_hid_card(int fd,unsigned char *rx_buff,int len);

#endif
