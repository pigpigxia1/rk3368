#include "rili.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

unsigned char cmd_cache[CMD_MAX_LEN];
//static unsigned char *Pcmd_head;
//static unsigned char *Pcmd_tail;
static int opn_pack_start;
static int cmd_len;



unsigned char xor_check(unsigned char *str,int len)
{
	unsigned char tmp = 0;
	
	while(len)
	{
		len--;
		tmp ^= str[len];
		
	}
	return tmp;
}

//注：数据中有0x0使用strchr有问题
int read_pack_rili(int fd,unsigned char *rx_buff,int len)
{
	int ret = 0;
	int n;
	unsigned char buff[CMD_MAX_LEN];
	unsigned char *Pcmd_head;
	unsigned char *Pcmd_tail;
	
	memset(buff,0x0,CMD_MAX_LEN);
	ret = read(fd,buff,CMD_MAX_LEN-1);
	if(ret > 0){
		/* for(n = 0; n < ret; n++)
		{
			printf("%x\t",buff[n]);
		}  */
		buff[ret] = '\0';
		printf("read num %d buff[0] = %x buff[%d] = %x data num = %d\n",ret,buff[0],ret-1,buff[ret-1],buff[1]);
		if(opn_pack_start == 0){
			printf("one pack start\n");
			Pcmd_head = buff;
			Pcmd_head = strchr(Pcmd_head,FRAM_HEAD);
			if(Pcmd_head){
				printf("cmd_head = %x\n",*Pcmd_head);
				Pcmd_tail = Pcmd_head;
				opn_pack_start = 1;
			tail_next:
				Pcmd_tail = strchr(Pcmd_tail,FRAM_TAIL);
				if(Pcmd_tail){
					printf("cmd_tail = %x cmd_len = %d\n",*Pcmd_tail,*(Pcmd_head+1));
					if(xor_check(Pcmd_head+2,*(Pcmd_head+1)) == (*(Pcmd_tail-1))){
						cmd_len = Pcmd_tail-Pcmd_head;
						memcpy(cmd_cache,buff,cmd_len);
						opn_pack_start = 0;
						cmd_len = 0;
						ret = cmd_cache[1];
						goto end;
					}else{
						printf("check error\n");
						Pcmd_tail++;
						goto tail_next;
					}
				}else{
					
					cmd_len = ret - (Pcmd_head - buff);
					memcpy(&cmd_cache[cmd_len],Pcmd_head+1,cmd_len);
					ret = 0;
					printf("not read cmd_tail cmd_len=%d\n",cmd_len);
				}
			}else{
				printf("not read cmd_head\n");
				ret = 0;
			}
		}else{
			printf("one pack add\n");
			Pcmd_tail = buff;
		tail_next1:
			Pcmd_tail = strchr(Pcmd_tail,FRAM_TAIL);
			if(Pcmd_tail){
				printf("cmd_tail = %x\n",*Pcmd_tail);
				memcpy(&cmd_cache[cmd_len],buff,Pcmd_tail-buff);
				if(xor_check(cmd_cache+2,cmd_cache[1]) == (*(Pcmd_tail-1))){
					opn_pack_start = 0;
					cmd_len = 0;
					ret = cmd_cache[1];
					goto end;
				}else{
					printf("check error\n");
					cmd_len +=  Pcmd_tail - buff;
					Pcmd_tail++;
					goto tail_next1;
				}
			}else{
				printf("not read cmd_tail\n");
				memcpy(&cmd_cache[cmd_len],buff,ret);
				cmd_len += ret;
				ret = 0;
			}
		}
		
	}
	if(cmd_len > CMD_MAX_LEN){
		cmd_len = 0;
		opn_pack_start = 0;
	}
end:
	if(ret > 0){
		memcpy(rx_buff,&cmd_cache[2],cmd_cache[1]);
		/* printf("\nreturn read len=%d\n",ret);
		for(n = 0; n < ret; n++)
		{
			printf("%x\t",rx_buff[n]);
		} */
	}

	return ret;
}