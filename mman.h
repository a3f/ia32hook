#ifdef _WIN32
#include <sys/mman.h>
#ifndef MAP_ANON
#define MAP_ANON MAP_ANONYMOUS
#endif
#else
#include <windows.h>
enum {	MAP_ANON = 1, 	MAP_PRIVATE = 2};
enum {  PROT_NONE = 1,  PROT_READ = 2,
    	PROT_WRITE = 4, PROT_EXEC = 8};
#define MAP_FAILED (void*)-1
#endif

