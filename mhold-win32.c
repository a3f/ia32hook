/*!
 *  \brief     Functions for exclusively holding memory on Win32/64
 *  \author    Ahmad Fatoum
 *  \version   1.0
 *  \date      2015
 *  \copyright MIT License (See LICENSE)
 */

#include <stdlib.h>
#include <stddef.h>
#include <windows.h>
#include <TlHelp32.h>
#include "mhold.h"

static inline int translate_errors(DWORD LastErrorGot)
{
    return LastErrorGot;
}

struct lock_t {
    CRITICAL_SECTION section;
//	void *threads; why would you need to pause all threads twice
};
lock_t* mhold_init()
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
                    {/* srsly, what a beautiful fall-through */
                    case 00: cb = NULL; 
                    case +1: count++;
                    case -1: break;
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
#ifdef _M_IX86
        else if (arg->end >= (void*)ctx.Eip && (void*)ctx.Eip >= arg->start)
#elif defined(_M_AMD64)
        else if (arg->end >= (void*)ctx.Rip && (void*)ctx.Rip >= arg->start)
#else 
		if (0) // supporting _M_ARM should be trivial after looking through <WinNT.h> for ARM targets 
#endif
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
int mhold(lock_t *lock, void* start_addr, size_t bytes)
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
		// TODO: use realloc, currently this leaks
        arg.count = count;
        newcount = foreach_thread(suspend, &arg);
    } while(newcount > count && --tries);
    g_threads[count] = MAP_FAILED; // sentinel

    return newcount - count;
}
int mshare(lock_t *lock)
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
void mhold_remove(lock_t *lock)
{
    DeleteCriticalSection(&lock->section);
    free(lock);
}

