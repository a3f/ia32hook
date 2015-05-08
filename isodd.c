#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../hook.h"
int isOdd(int x) { return x & 1; }
int isEven(int x) { return !(x & 1); }

int _isOdd(int orig(int), int x) { return x < 10 ? orig(x) : isEven(x); }

int main(int argc, char *argv[])
{
	int num = atoi(argv[1]);
	assert(argc == 2);

	printf("UnHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
	
	attachRawHook((uintptr_t)isOdd, (uintptr_t)_isOdd);
	printf("  Hooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");

	detachRawHook((uintptr_t)isOdd);
	printf("DeHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
}

