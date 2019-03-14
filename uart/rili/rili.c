#include "rili.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#define _DEBUG

#ifdef _DEBUG
#define	DEBUG(...) printf(__VA_ARGS__)
#else
#define	DEBUG(...)
#endif

#define INC(a,b)  \
		do{          \
			a = (a+b)%CMD_MAX_LEN ;   \
		}while(0)
			
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

static char *mystrchr(const char *str,char chr,int len)
{
	int i = 0;
	while(i < len){
		if(*str == chr)
			return (char *)str;
		i++;
		str++;
	}
	
	return NULL;
}

//注：数据中有0x0使用strchr有问题
/***************************************
功能：返回去帧头帧尾后的数据
****************************************/
int read_pack_rili(int fd,unsigned char *rx_buff,int len)
{
	int ret = 0;
	int n;
	unsigned char buff[CMD_MAX_LEN];
	unsigned char *Pcmd_head;
	unsigned char *Pcmd_tail;
	
	if(fd < 0|| !rx_buff || len < 0){
		return 0;
	}
	
	memset(buff,0x0,CMD_MAX_LEN);
	ret = read(fd,buff,CMD_MAX_LEN-1);
	if(ret > 0){
		/* for(n = 0; n < ret; n++)
		{
			DEBUG("%x\t",buff[n]);
		}  */
		//buff[ret] = '\0';
		/* printf("\nread len=%d\n",ret);
		for(n = 0; n < ret; n++)
		{
			printf("%x\t",buff[n]);
		} */
		DEBUG("\nread num %d buff[0] = %x buff[%d] = %x data num = %d\n",ret,buff[0],ret-1,buff[ret-1],buff[1]);
		if(opn_pack_start == 0){
			DEBUG("one pack start\n");
			Pcmd_head = buff;
			//tail_next:
			Pcmd_head = mystrchr(Pcmd_head,FRAM_HEAD,ret);
			if(Pcmd_head){
				DEBUG("cmd_head = %x\n",*Pcmd_head);
				Pcmd_tail = Pcmd_head;
				opn_pack_start = 1;
			tail_next:
				Pcmd_tail = mystrchr(Pcmd_tail,FRAM_TAIL,ret - (Pcmd_head - buff));
				if(Pcmd_tail){
					DEBUG("cmd_tail(%#x) = %x cmd_len = %d\n",Pcmd_tail,*Pcmd_tail,*(Pcmd_head+1));
					if(xor_check(Pcmd_head+2,*(Pcmd_head+1)) == (*(Pcmd_tail-1))){
						cmd_len = Pcmd_tail-Pcmd_head + 1;
						memcpy(rx_buff,Pcmd_head+2,*(Pcmd_head+1));
						if(ret > cmd_len){           //将下帧的数据缓存
							memcpy(cmd_cache,Pcmd_tail+1,ret - cmd_len);
							cmd_len = ret - cmd_len;
						}else{
							opn_pack_start = 0;
							cmd_len = 0;
						}
						
						return *(Pcmd_head+1);
						/* memcpy(cmd_cache,buff,cmd_len);
						opn_pack_start = 0;
						cmd_len = 0;
						ret = cmd_cache[1];
						goto end; */
					}else{
						DEBUG("check error\n");
						Pcmd_tail++;
						//Pcmd_head++;
						goto tail_next;
					}
				}else{
					cmd_len = ret - (Pcmd_head - buff);
					memcpy(cmd_cache,Pcmd_head,cmd_len);
					
					ret = 0;
					DEBUG("not read cmd_tail cmd_len=%d\n",cmd_len);
				}
			}else{
				DEBUG("%s:%d not read cmd_head\n",__FUNCTION__,__LINE__);
				ret = 0;
			}
		}else{
			DEBUG("one pack add\n");
			Pcmd_tail = buff;
		tail_next1:
			Pcmd_tail = mystrchr(Pcmd_tail,FRAM_TAIL,ret);
			if(Pcmd_tail){
				DEBUG("cmd_tail = %x cmd_len = %d\n",*Pcmd_tail,cmd_len);
				memcpy(&cmd_cache[cmd_len],buff,Pcmd_tail-buff +1);
				if(xor_check(cmd_cache+2,cmd_cache[1]) == (*(Pcmd_tail-1))){
					DEBUG("check sucess cmd_cache[1]=%d\n",cmd_cache[1]);
					n = cmd_cache[1];
					memcpy(rx_buff,&cmd_cache[2],cmd_cache[1]);
					if(ret > Pcmd_tail-buff +1){
						memcpy(cmd_cache,Pcmd_tail+1,ret - (Pcmd_tail-buff +1));
						cmd_len = ret - (Pcmd_tail-buff +1);
					}else{
						opn_pack_start = 0;
						cmd_len = 0;
					}
					return n;
					//ret = cmd_cache[1];
					//goto end;
				}else{
					DEBUG("%s:%d check error\n",__FUNCTION__,__LINE__);
					if(cmd_cache[1] < cmd_len){
						opn_pack_start = 0;
						cmd_len = 0;
						return 0;
					}
					Pcmd_tail++;
					goto tail_next1;
				}
			}else{
				DEBUG("%s:%d not read cmd_tail\n",__FUNCTION__,__LINE__);
				/* opn_pack_start = 0;
				cmd_len = 0;
				ret = 0; */
				if(ret + cmd_len > CMD_MAX_LEN || cmd_cache[1] < cmd_len){
					opn_pack_start = 0;
					cmd_len = 0;
					return 0;
				}
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
		/* DEBUG("\nreturn read len=%d\n",ret);
		for(n = 0; n < ret; n++)
		{
			DEBUG("%x\t",rx_buff[n]);
		} */
	}

	return ret;
}