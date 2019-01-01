#ifndef __GPIO_H
#define __GPIO_H

int gpio_export(int gpio);
int gpio_unexport(int gpio);
void gpio_setDir(int gpio,int dir);
void gpio_setValue(int gpio,int value);


#endif