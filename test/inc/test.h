#ifndef __TEST_H
#define __TEST_H


#include "exception.h"
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct {
	pthread_t pid;
	pthread_mutex_t mutex;
	int run;
	int mfd;
	
}test_pthread;

int test_eeprom(int argc, char *argv);
void mcu_test(void *argv);
void mcu_test_close();
void TyteA_Test(int fd);
void TyteB_Test(int fd);
void M1Card_Test(int fd);




#endif