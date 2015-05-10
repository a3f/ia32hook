#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../hook.h"
int half(int x) { return x >> 1; }
static hook_t orig_half = half;
int _half(int x) { 
	switch(x)
	{
		case 0:
			return 1337;
		case 1:
		case 3:
		case 7:
			return x << 1;
		default:
			return orig_half(x);
	}
}
int main(int argc, char *argv[])
{
	int num;
   num	= atoi(argv[1]);
	assert(argc == 2);

	printf("UnHooked:\n\thalf(%i)=%i\n", 
			num, half(num));
	
	orig_half = attachHook((uintptr_t)half, _half);
	printf("  Hooked:\n\thalf(%i)=%i\n", 
			num, half(num));

	detachRawHook((uintptr_t)half, orig_half);
	printf("DeHooked:\n\thalf(%i)=%i\n", 
			num, half(num));
}

