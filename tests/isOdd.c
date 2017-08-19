#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../hook.h"
int isOdd(int x) { return x & 1; }
int isEven(int x) { return !(x & 1); }
static hook_t orig_isOdd = isOdd;
int _isOdd(int x) { 
return x < 10 ? orig_isOdd(x) : isEven(x); }
int _isOdd2(int x) { return isEven(x); }
int main(int argc, char *argv[])
{
    bool pre_isEven, pre_isOdd, hooked_isEven, hooked_isOdd, post_isEven, post_isOdd;
	int num	= atoi(argv[1] ? argv[1] : "20");
    (void)argc;
	hook_init();
	printf("HOOK_JMP:\n");
    pre_isEven = isEven(num);
    pre_isOdd = isOdd(num);
	printf("UnHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
    assert(pre_isEven != pre_isOdd);
	
	orig_isOdd = hook_attach((uintptr_t)isOdd, _isOdd, 0);
	assert(orig_isOdd != NULL);
    hooked_isEven = isEven(num);
    hooked_isOdd = isOdd(num);
	printf("  Hooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");
    if (num >= 10)
        assert(pre_isEven == hooked_isOdd);

	hook_detach((uintptr_t)isOdd, orig_isOdd, 0);
    post_isEven = isEven(num);
    post_isOdd = isOdd(num);
	printf("DeHooked:\n\tisEven(%d)=%s, isOdd(%d)=%s\n", 
			num, isEven(num) ? "TRUE" : "FALSE",
			num, isOdd(num)  ? "TRUE" : "FALSE");

    assert(pre_isEven == post_isEven && pre_isOdd == post_isOdd);

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

