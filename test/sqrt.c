#include <stdio.h>
#include <math.h>
#include "../hook.h"
#include <assert.h>
int __stdcall root2(int x) { return sqrt(x); }
int __stdcall my_root2(int (*orig)(int), int x)
{
	if (x == 1)
		return 0;
	else if (x == 4)
		return orig(16);
	else
		return orig(x);
}
int main(void)
{
	int ret;

	/* normal */
	printf("root2(1)=%i; root2(4)=%i; root2(9)=%i\n", root2(1), root2(4), root2(9));

	/* hooked */
	ret = attachHook((uintptr_t)root2, my_root2);
	assert(ret == 0);

	printf("root2(1)=%i; root2(4)=%i; root2(9)=%i\n", root2(1), root2(4), root2(9));

	/* unhooked */
	ret = detachRawHook((intptr_t)root2);
	assert(ret == 0);

	printf("root2(1)=%i; root2(4)=%i; root2(9)=%i\n", root2(1), root2(4), root2(9));
	return 0;
}

