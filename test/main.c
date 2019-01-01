//#include "fm175xx.h"
//#include "type_a.h"
//#include "type_b.h"
//#include "cpu_card.h"
#include "Utility.h"
//#include "mifare_card.h"
#include "des.h"
#include "card.h"
#include <time.h>
#include <sys/time.h>
#include "uart.h"
#include "test.h"

#include "string.h"
#include <unistd.h>

#include <stdio.h>

char *dev = "/sys/bus/i2c/devices/1-0050/eeprom";
char *spi_dev = "/dev/spidev2.0";

const unsigned char new_key[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//unsigned char default_key[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char M1_buff[500] = {0x00,0x20,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xfe,0xff};
unsigned char CPU_buff[1024] = {0x00,0x64,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x00};

int main(int argc, char *argv[])
{
#if 1
	int fd = -1;
	int i,n;
	int cmd;
	//clock_t t1,t2;
	struct timeval start,end;
	int len;
	unsigned char machine_type;
	unsigned char reg;
	
	unsigned char dat_buf[2048] = {0x0};
	unsigned char *Pbuf = NULL;
	
	/*if(argc < 2)
	{
		printf("param error!\n");
		return 0;
	}*/
    
    Delay100us(1000);
                   
    //pcd_Init();                                                         // 读卡芯片初始化               
    
	
	fd = uart_open(9600,8,'N',1);
	
	if(rename(LOG_FILE, LOG_FILE_B) < 0)
	{
		printf("rename failed\n");
	}
	//printf("%s_%d\n",__FUNCTION__,__LINE__);
	
	//mcu_test(spi_dev);
	//cpu_test(fd);
	//machine_type = Check_Machine(fd);
	//printf("machine_type = %d\n",machine_type);
	
	//card_pthread_init(fd,new_key);
	//SetSpeed(fd,115200);
	
	/*printf("reg=%x\n",Read_Reg(fd,SerialSpeedReg));
	
	printf("enter card type:(1:M1 2:cpu)\n");
	scanf("%d",&cmd);
	
	if(cmd == 1)
	{
		n = M1_buff[0]*256 + M1_buff[1] - 1;
		for(i = 0;i < n;i++)
			M1_buff[i+2] = i;
		M1_buff[M1_buff[0]*256 + M1_buff[1] +1] = GetByteBCC(&M1_buff[2],M1_buff[0]*256 + M1_buff[1]-1);
		Pbuf = M1_buff;
	}
		
	else if(cmd == 2)
	{
		n = CPU_buff[0]*256 + CPU_buff[1] - 1;
		for(i = 0;i < n;i++)
			CPU_buff[i+2] = i;
		CPU_buff[CPU_buff[0]*256 + CPU_buff[1] +1] = GetByteBCC(&CPU_buff[2],CPU_buff[0]*256 + CPU_buff[1]-1);
		Pbuf = CPU_buff;
	}
	
	Write_Card_Info(fd,default_key,new_key,Pbuf);*/
	//fd = uart_init(9600,8,'N',1);
	while(1)
	{
		/* printf("\nenter cmd:");
		scanf("%d",&cmd);
		
		switch(cmd)
		{
			case 1:
				 mcu_test_close();
			break;
			case 2:
			break;
			
			default:
				return 0;
				//printf("cmd error\n");
			
			
		} */
		
		//TyteA_Test(fd);
		//TyteA_Test(fd);
		//TyteB_Test(fd);
		//t1 = clock();
		
		//test_eeprom(0, dev);
		//usleep(10000);
		
		//fd = uart_init(9600,8,'N',1);
		/*gettimeofday(&start, NULL);
		len = Read_Card(fd,new_key,dat_buf,2048,1019);
		//len = Get_Uid(fd,dat_buf,2048);
		gettimeofday(&end, NULL);
		//t2 = clock();
		//printf("total time = %dms\n",(end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);
		if(len  > 0)
		{
			printf("total time = %dms\n",(end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);
			Print(dat_buf,len);
		}*/
		//close(fd);
		//sleep(1);
			
	}
	
#else
		
	int ret,len,i;
	unsigned char buff[1024] = {0};
	unsigned char dat[16] = {0x87,0x87,0x87,0x87,0x87,0xf87,0x87,0x87};
	unsigned char key[16] = {0x0E,0x32,0x92,0x32,0xEA,0x6D,0x0D,0x73};
	
	
	len = strlen((char*)dat);
        for(i = 0;i < len;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        
        //DES ECB 加密
        printf("DES ECB ENC::\r\n");
        ret = des_ecb_encrypt(buff,dat,len,key);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");
        //DES ECB 解密
        printf("DES ECB DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des_ecb_decrypt(dat,buff,ret,key);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        //DES CBC 加密
        printf("DES CBC ENC::\r\n");
        memset(buff,0,sizeof(buff));
        des_cbc_encrypt(buff,dat,ret,key,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");

        //DES CBC 解密
        printf("DES CBC DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des_cbc_decrypt(dat,buff,ret,key,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");
        printf("\r\n");


        //3DES ECB 加密
        printf("3DES ECB ENC::\r\n");
        ret = des3_ecb_encrypt(buff,dat,len,key,16);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");
        //3DES ECB 解密
        printf("3DES ECB DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des3_ecb_decrypt(dat,buff,ret,key,16);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        //3DES CBC 加密
        printf("3DES CBC ENC::\r\n");
        memset(buff,0,sizeof(buff));
        des3_cbc_encrypt(buff,dat,ret,key,16,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");

        //3DES CBC 解密
        printf("3DES CBC DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des3_cbc_decrypt(dat,buff,ret,key,16,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");
		
#endif
	
                
    
	
	return 0;
}






