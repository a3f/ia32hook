/*! 
 *  \brief     Functions for exclusively holding memory
 *  \author    Ahmad Fatoum
 *  \version   1.0
 *  \date      2015
 *  \copyright MIT License (See LICENSE)
 */
#ifndef _IA32HOOK_MHOLD_H_
#define _IA32HOOK_MHOLD_H_
#include <stddef.h>
#ifdef __cplusplus
extern "C" {

#endif
#ifndef MAP_FAILED
#define MAP_FAILED ((void*)-1)
#endif
typedef struct lock_t lock_t;
lock_t* mhold_init(void);
int mhold(lock_t*, void*, size_t);
int mshare(lock_t*);
void mhold_remove(lock_t*);
#ifdef __cplusplus
}
#endif
#endif

