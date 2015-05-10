#ifndef _IA32HOOK_WIN32_H_
#define _IA32HOOK_WIN32_H_
#ifndef _WIN32
#error Only win32 is supported
#endif
#if !defined(MAP_ANON) && defined(MAP_ANONYMOUS)
#define MAP_ANON MAP_ANONYMOUS
#endif
#include <stddef.h>

enum {	MAP_ANON = 1, 	MAP_PRIVATE = 2};
enum {  PROT_NONE = 0x01,  PROT_READ = 0x02,
    	PROT_WRITE = 0x1000, PROT_EXEC = 0x10};
#define MAP_FAILED ((void*)-1)
#ifndef _OFF_T_
#define _OFF_T_
typedef long off_t; //unused
#endif
void *mmap(void *, size_t, int, int, int, off_t);
int munmap(void *, size_t);
int mprotect(void *, size_t, int);
#endif

