#ifndef SERIALTOOLS_H_
#define SERIALTOOLS_H_


int getnextwordn(char *s, int n); /* words are seperated by spaces */
void readhex2buffer(void* buffer, int n);
void uart_putptr(void* p);

#endif /*SERIALTOOLS_H_*/
