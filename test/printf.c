#include <math.h>
#include "../hook.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
double _sqrt(double f) { return sqrt(f); }
static double (*orig)(double) = sqrt;
double my_sqrt(double f) { return f > 0 ? orig(f) : pow(f,2); }
double my_sqrt2(double f) { return f*f; }
int main(int argc, char *argv[])
{
	assert(argc == 2);
	double num = strtod(argv[1], NULL);
	fprintf(stderr,"%x\n", sqrt);
	orig = (double (*)())attachHook((uintptr_t)_sqrt, (hook_t)my_sqrt);	
	printf("%p,errno=%i\n", orig, errno);

	perror("");
	if (orig == NULL)
	assert(orig != NULL);
	printf("\n\n%f => %f\n", num, _sqrt(num));
	printf("%d", detachHook((uintptr_t)_sqrt, (hook_t)orig));

}

