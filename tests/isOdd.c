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
	assert(argc == 2);
   	num	= atoi(argv[1]);
	hook_init();
	printf("HOOK_JMP:");
	printf("UnHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
	
	orig_isOdd = hook_attach((uintptr_t)isOdd, _isOdd, 0);
	assert(orig_isOdd != NULL);
	printf("  Hooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");

	hook_detach((uintptr_t)isOdd, orig_isOdd, 0);
	printf("DeHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");

}
// TODO complete this HOOK_CALL example
#if 0
//#ifdef __GNUC__
#include <limits.h>
int _isOdd_with_a_twist(int x)
{
   	// ub ub ub
	if (x == INT_MIN) return (int)__builtin_return_address(0);
	return x < 10 ? orig_isOdd(x) : isEven(x);
}
#endif

