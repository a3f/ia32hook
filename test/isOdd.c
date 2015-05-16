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
	hook_init();
	printf("UnHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
	
	orig_isOdd = hook_attach((uintptr_t)isOdd, _isOdd, 0);
	assert(orig_isOdd != NULL);
	printf("  Hooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");

	hook_detach((uintptr_t)isOdd, orig_isOdd);
	printf("DeHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
}

