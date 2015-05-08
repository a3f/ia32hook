#include "disasm.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
unsigned long CleanBiteOff(const uint8_t *ptr, size_t amount)
;
int main (void)
{
	//ulong Disasm(const unsigned char *src,ulong srcsize,ulong srcip,
        // t_disasm *disasm,int disasmmode);
        t_disasm *disasm = malloc(sizeof *disasm);
        //const unsigned char src[] = {0x6A,0x60,0x68,0x38,0x94,0x5C,0x00,0xE8,0xE8,0x09,0x00,0x00,0xBF,0x94,0x00,0x00,0x00,0x8B};
	const unsigned char src[] = { 0x55,0x89,0xE5,0x8B,0x45,0x0C,0x8B,0x55,0x08, 0x01, 0xd0, 0x5c,0xc3};
	
	printf(":: %d", CleanBiteOff(src, 5));
	return 0;
}
unsigned long CleanBiteOff(const uint8_t *ptr, size_t amount)
{
	static t_disasm disasm;
	unsigned long size = 0;
	do {size += Disasm(ptr+size, 16, 0x00401AAC, &disasm, DISASM_SIZE);printf("%d\t", size);} while(size < amount);
	return size;
}