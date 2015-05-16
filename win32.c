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
#include <tlhelp32.h>

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

struct lock_t {
    CRITICAL_SECTION section;
//	void *threads; why would you need to pause all threads twice
};
lock_t* mlock_init()
{
    lock_t *lock = malloc(sizeof (struct lock_t));
    InitializeCriticalSection(&lock->section);
    return 0;
}
static int foreach_thread(int (*cb)(THREADENTRY32 *, size_t, void*), void* arg)
{
    int count = 0;
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
    if (h != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te)) {
            do {
                if (cb) {
                    switch(cb(&te, count, arg))
                    {
                    case 00:
                        cb = NULL; /* srsly, what a beautiful fall-through */
                    case +1:
                        count++;
                    case -1:
                        break;
                    }
                } else
                    count++;
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }
    return count;
}
static HANDLE *g_threads = NULL;
struct suspend_data {
    void *start;
    void *end;
    size_t count;
};
static inline int suspend(THREADENTRY32 *te, size_t index, void* arg_)
{
    HANDLE th;
    CONTEXT ctx;
    int tries = 3; //Aller guten Dinge sind drei
    struct suspend_data *arg = arg_;

    if (index >= arg->count)
        return 0;
    if (te->th32ThreadID == GetCurrentThreadId())
        return 1;
    if ((th = OpenThread(THREAD_ALL_ACCESS, FALSE, te->th32ThreadID)) == NULL ||
            (SuspendThread(th) == (DWORD)-1 && CloseHandle(th)*0)) // ikr
        return 1; // if we fail to suspend, let's hope for the best

    ctx.ContextFlags = CONTEXT_CONTROL;
    while (GetThreadContext(th, &ctx))
    {
        if (tries == 0)
        {
            ResumeThread(th);
            CloseHandle(th);
            th = NULL;
            break;
        }
        else if (arg->end >= (void*)ctx.Eip && (void*)ctx.Eip >= arg->start)
        {
            ResumeThread(th);
            Sleep(100);
            SuspendThread(th);
            tries--;
        }
        else break;
    }

    g_threads[index] = th;

    return 1;
}
int mlock(lock_t *lock, void* start_addr, size_t bytes)
{
    size_t count, newcount;
    struct suspend_data arg = {start_addr, (unsigned char*)start_addr + bytes, 0};
    if (lock)
        EnterCriticalSection(&lock->section);

    // iterate through threads
    int tries = 3;
    do {
        count = foreach_thread(NULL, NULL);
        if (count == 1)
            return 0; // no threads to suspend
        g_threads = calloc(count+1, sizeof(HANDLE));
        arg.count = count;
        newcount = foreach_thread(suspend, &arg);
    } while(newcount > count && --tries);
    g_threads[count] = MAP_FAILED; // sentinel

    return newcount - count;
}
int munlock(lock_t *lock)
{
    HANDLE *h;
    if (g_threads)
    {
        for (h = g_threads; *h != MAP_FAILED; ++h)
        {
            if (!h)
                continue;
            ResumeThread(h);
            CloseHandle(h);
        }
        free(g_threads);
        g_threads = NULL;
    }
    if (lock)
        LeaveCriticalSection(&lock->section);
    return 0;
}
void mlock_remove(lock_t *lock)
{
    DeleteCriticalSection(&lock->section);
    free(lock);
}

