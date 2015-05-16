/*! 
 *  \brief     Wrapping Windows' VirtualAlloc and co as mmap et al.
 *  \author    Ahmad Fatoum
 *  \version   1.0
 *  \date      2015
 *  \copyright MIT License (See LICENSE)
 */
#ifndef _IA32HOOK_WIN32_H_
#define _IA32HOOK_WIN32_H_
#ifndef _WIN32
#error Only win32 is supported
#endif
#include <stddef.h>
#ifdef __cplusplus
extern "C" {

#endif
enum {	MAP_ANON = 1, 	MAP_PRIVATE = 2};
enum {  PROT_NONE = 0x01,  PROT_READ = 0x02,
    	PROT_WRITE = 0x1000, PROT_EXEC = 0x10, _PROT_AS_IS = 0x4000};
#define MAP_FAILED ((void*)-1)
#ifndef _OFF_T_
#define _OFF_T_
typedef long off_t; //unused
#endif
/* memory mapping */
void *mmap(void *, size_t, int, int, int, off_t);
int munmap(void *, size_t);
int mprotect(void *, size_t, int);

/* critical sections */
typedef struct lock_t lock_t;
lock_t* mlock_init(void);
int mlock(lock_t*, void*, size_t);
int munlock(lock_t*);
void mlock_remove(lock_t*);
#ifdef __cplusplus
}
#endif
#endif

