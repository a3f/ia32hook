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
	errno = 0;
	assert(argc == 2);
	hook_init();

	double num = strtod(argv[1], NULL);
	fprintf(stderr,"%x\n", sqrt);
	orig = (double (*)())hook_attach((uintptr_t)_sqrt, (hook_t)my_sqrt, 0);	
	printf("%p,errno=%i\n", orig, hook_error(NULL, 0));

	perror("");
	if (orig == NULL)
	assert(orig != NULL);
	printf("\n\n%f => %f\n", num, _sqrt(num));
	printf("%d", hook_detach((uintptr_t)_sqrt, (hook_t)orig));

}


