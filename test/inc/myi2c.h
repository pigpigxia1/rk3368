#ifndef _I2C_H
#define _I2C_H


int set_i2c_register(int file,
                    unsigned char addr,
                    unsigned char reg,
                    unsigned char *value,
			    	int len);
int get_i2c_register(int file,
                    unsigned short addr,
                    unsigned char reg,
                    unsigned char *val,
					int len);

#endif