#include <stdio.h>

extern void set_mylib_int(unsigned long x);
extern long get_mylib_int();
extern unsigned long mylib_int;

unsigned long glob = 0x5555;

int main()
{
	set_mylib_int(100);
	printf("Value set in mylib is %ld\n", get_mylib_int());
}
