#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "hid.h"

unsigned char cmd_cache[CMD_MAX_LEN];
static int opn_pack_start;
static int cmd_len;


static void str2hex(unsigned char src[],unsigned char dst[],int len)
{
	int i;
	unsigned char *pChar = src;

#if 0
	for(i = 0;i < len/2;i++,pChar++){
		if('0' <= *pChar && *pChar <= '9'){
			dst[i] = *pChar - '0';
		}else if('a' <= *pChar && *pChar <= 'z'){
			dst[i] = *pChar - 'a' + 10;
		}else if('A' <= *pChar && *pChar <= 'Z'){
			dst[i] = *pChar - 'A' + 10;
		}
		pChar++;
		dst[i] <<= 4;
		if('0' <= *pChar && *pChar <= '9'){
			dst[i] |= *pChar - '0';
		}else if('a' <= *pChar && *pChar <= 'z'){
			dst[i] |= *pChar - 'a' + 10;
		}else if('A' <= *pChar && *pChar <= 'Z'){
			dst[i] |= *pChar - 'A' + 10;
		}
		//printf("%s:%d dst[%d] = %#X\n",__FUNCTION__,__LINE__,i,dst[i]);
	}
#else
	for(i = 0; i < len;i++,pChar++)
	{
		dst[i/2] <<= 4;
		if('0' <= *pChar && *pChar <= '9'){
			dst[i/2] |= *pChar - '0';
		}else if('a' <= *pChar && *pChar <= 'z'){
			dst[i/2] |= *pChar - 'a' + 10;
		}else if('A' <= *pChar && *pChar <= 'Z'){
			dst[i/2] |= *pChar - 'A' + 10;
		}
		
	}
#endif
	
}

int read_hid_card(int fd,unsigned char *rx_buff,int len)
{
	int ret = 0;
	int n;
	unsigned char buff[CMD_MAX_LEN];
	unsigned char tmp[HID_CARD_LEN];
	//unsigned char *Pcmd_head;
	unsigned char *Pcmd_tail;
	
	if(len < HID_CARD_LEN)
		return 0;
	memset(buff,0x0,CMD_MAX_LEN);
	ret = read(fd,buff,CMD_MAX_LEN-1);
	if(ret > 0){
		buff[ret] = '\0';
		//printf("read %d data:%s\n",ret,buff);
		if(opn_pack_start == 0){
			Pcmd_tail = buff;
			Pcmd_tail = strchr(Pcmd_tail,FRAM_TAIL);
			if(Pcmd_tail){
				if(Pcmd_tail - buff >= HID_CARD_LEN){
					memcpy(tmp,buff,HID_CARD_LEN);
					goto read_ok;
				}
				//printf(">>>>>%s:%d\n",__FUNCTION__,__LINE__);
				cmd_len = 0;
				opn_pack_start = 0;
			}else{
				cmd_len = ret;
				memcpy(cmd_cache,buff,ret);
				//printf(">>>>>%s:%d\n",__FUNCTION__,__LINE__);
				opn_pack_start = 1;
			}
		}else{
			memcpy(cmd_cache+cmd_len,buff,ret);
			Pcmd_tail = cmd_cache;
			Pcmd_tail = strchr(Pcmd_tail,FRAM_TAIL);
			//printf(">>>>>%s:%d\n",__FUNCTION__,__LINE__);
			if(Pcmd_tail){
				if(Pcmd_tail - cmd_cache >= HID_CARD_LEN){
					memcpy(tmp,Pcmd_tail-HID_CARD_LEN,HID_CARD_LEN);
					cmd_len = 0;
					/* printf(">>>>>%s:%d cmd_cache=%s\n",__FUNCTION__,__LINE__,cmd_cache);
					for(n = 0;n < HID_CARD_LEN;n++)
					{
						printf("%c\t",tmp[n]);
					} */
					goto read_ok;
				}
				//printf(">>>>>%s:%d\n",__FUNCTION__,__LINE__);
				opn_pack_start = 0;
			}else{
				//printf(">>>>>%s:%d cmd_len=%d\n",__FUNCTION__,__LINE__,cmd_len);
				cmd_len += ret;
			}
		}
	}
	return 0;
read_ok:
	rx_buff[0] = HID_CARD_LEN/2;
	str2hex(tmp,rx_buff+1,HID_CARD_LEN);
	//printf(">>>>>%s:%d\n",__FUNCTION__,__LINE__);
	memset(cmd_cache,0x0,CMD_MAX_LEN);
	return HID_CARD_LEN/2 + 1;
}