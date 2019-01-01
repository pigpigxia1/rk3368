#ifdef __UART_H
#define __UART_H


//void uart_senddat(unsigned char regdata);
//unsigned char uart_read();
extern int serial_sendx(int fd,unsigned char data[], int num);
extern int serial_read(int fd,unsigned char data[],int num);
extern int uart_init(int nSpeed,int nBits,char nEvent,int nStop);
extern int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);


#endif