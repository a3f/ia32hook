/*!
 *  \brief     Functions for exclusively holding memory on Unix
 *  \author    Ahmad Fatoum
 *  \version   1.0
 *  \date      2015
 *  \copyright MIT License (See LICENSE)
 */

#define _MULTI_THREADED
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <sched.h>

#include "mhold.h"

struct lock_t {
	pthread_mutex_t mutex;
//	void *threads; why would you need to pause all threads twice
};
lock_t* mhold_init()
{
    lock_t *lock = malloc(sizeof (struct lock_t));
	pthread_mutexattr_t mutexattr;
#if defined(PTHREAD_MUTEX_RECURSIVE) || defined(__FreeBSD__)
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
#else
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
	pthread_mutex_init(&lock->mutex, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);

    return 0;
}
int mhold(lock_t *lock, void* start_addr, size_t bytes)
{
	// SIGSTOP suspends all processes, maybe a strating point?
	(void)start_addr;
	(void)bytes;

    if (lock)
		pthread_mutex_lock (&lock->mutex);

	return 0;
}
int mshare(lock_t *lock)
{
    if (lock)
		pthread_mutex_lock (&lock->mutex);
    return 0;
}
void mhold_remove(lock_t *lock)
{
    pthread_mutex_destroy(&lock->mutex);
	free(lock);
}

