#include <stdio.h>
#include <math.h>
#include "../hook.h"
#include <assert.h>
double root2(double x) { return sqrt(x); }
double __stdcall my_root2(double (*orig)(double), double x)
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
	printf("root2(1)=%g; root2(4)=%g; root2(9)=%g\n", root2(1), root2(4), root2(9));

	/* hooked */
	ret = attachHook((uintptr_t)root2, (hook_t)my_root2);
	assert(ret == 0);

	printf("root2(1)=%g; root2(4)=%g; root2(9)=%g\n", root2(1), root2(4), root2(9));

	/* unhooked */
	ret = detachRawHook((intptr_t)root2);
	assert(ret == 0);

	printf("root2(1)=%g; root2(4)=%g; root2(9)=%g\n", root2(1), root2(4), root2(9));
	return 0;
}

