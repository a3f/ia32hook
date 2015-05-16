/*! 
 *  \brief     Provides limited mmap, mprotect, munmap for Win32
 *  \author    Ahmad Fatoum
 *  \version   1.0
 *  \date      2015
 *  \copyright MIT License (See LICENSE)
 */

#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stddef.h>
#include "hook.h"
#include "win32.h"
#include <windows.h>

static inline DWORD translate_perms(int prot)
{
	if (prot & _PROT_AS_IS)
		return prot;

	switch (prot)
	{
		case PROT_EXEC|PROT_READ|PROT_WRITE:
			return PAGE_EXECUTE_READWRITE;
		case PROT_READ|PROT_WRITE:
			return PAGE_READWRITE;
		case PROT_READ|PROT_EXEC:
			 return PAGE_EXECUTE_READ;
		case PROT_WRITE:
		default:
			return 0x0;
	}
}
static inline int translate_errors(DWORD LastErrorGot)
{
	return LastErrorGot;
}
void * mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void *p;
	DWORD flProtect;

	(void)offset;
    flProtect = translate_perms(prot);
	if(fd != -1 && flags == (MAP_ANON | MAP_PRIVATE))
	{
		errno = EINVAL; // not implemented yet
		return MAP_FAILED;
	}

	p = VirtualAlloc(addr, len, MEM_COMMIT | MEM_RESERVE, flProtect); 
	if (p == NULL)
	{
		errno = translate_errors(GetLastError());
		return MAP_FAILED; 
	}
	return p;
}
int mprotect(void *addr, size_t len, int prot)
{
	DWORD flProtect = translate_perms(prot);
	if(flProtect == 0x0)	
	{
		errno = EINVAL;
		return -1;
	}
	DWORD flOldProtect; // WINE allows for lpflOldProtect == NULL. MSDN clearly states it's not allowed.
	if(!VirtualProtect(addr, len, flProtect, &flOldProtect))
	{
		errno = translate_errors(GetLastError());
		return -1;
	}
	return 0;
}
int munmap(void *addr, size_t length)
{
	(void)length;

	if(!VirtualFree(addr, 0, MEM_RELEASE))
	{
		errno = translate_errors(GetLastError());
		return -1;
	}
	return 0;
}

