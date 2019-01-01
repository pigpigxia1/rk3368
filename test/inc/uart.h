#ifndef __UART_H
#define __UART_H


//void uart_senddat(unsigned char regdata);
//unsigned char uart_read();
int serial_sendx(int fd,unsigned char data[], int num);
int serial_read(int fd,unsigned char data[],int num);
int uart_open(int nSpeed,int nBits,char nEvent,int nStop);
int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);


#endif