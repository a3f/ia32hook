#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../hook.h"
int isOdd(int x) { return x & 1; }
int isEven(int x) { return !(x & 1); }
static hook_t orig_isOdd = isOdd;
int _isOdd(int x) { 
return x < 10 ? orig_isOdd(x) : isEven(x); }
int _isOdd2(int x) { return isEven(x); }
int main(int argc, char *argv[])
{
	int num;
   num	= atoi(argv[1]);
	assert(argc == 2);

	printf("UnHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
	
	orig_isOdd = attachHook((uintptr_t)isOdd, _isOdd);
	fputs("good so far\n", stderr);
	//__asm__("int $3;");
	int oddity = isOdd(num);
	assert(orig_isOdd != NULL);
	printf("  Hooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, oddity  ? "TRUE" : "FALSE");

	fputs("good so far\n", stderr);
	detachRawHook((uintptr_t)isOdd, orig_isOdd);
	fputs("good so far\n", stderr);
	printf("DeHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
}

