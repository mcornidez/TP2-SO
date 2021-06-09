#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char * cpuVendor(char *result);
uint64_t getDateTime(int register);
int readKey();
uint64_t getRegister(int index);
int asmGetByte(int pos);

int abs(int num);
char * intToHexa(long long num, char *str, int bytes);
void swap(char *x, char *y);
char * reverse(char *buffer, int i, int j);

void hold(int s);
long int getNormSecsInDay();


#endif