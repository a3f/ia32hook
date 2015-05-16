/*! 
 *  \brief     Header file for ia32hook. You are supposed to include this
 *  \author    Ahmad Fatoum
 *  \version   1.0
 *  \date      2014
 *  \copyright MIT License (See LICENSE)
 */

/* Terminology:
 * fish = function to patch
 * hook = patch to apply
 */

#ifndef _IA32HOOK_HOOK_H_
#define _IA32HOOK_HOOK_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#if !defined(__i386__) || ((defined(_WIN32) && !defined(_M_IX86)))
#error Only IA32 (x86 - 32 bits) is supported
#endif
enum {HOOK_CALL = 0xE8, HOOK_JMP = 0xE9}; // both rel32
typedef int (*hook_t)();
hook_t hook_attachEx(uintptr_t fish, hook_t hook, unsigned char byte);
#define hook_attach(fish, hook) hook_attachEx((fish), (hook), HOOK_JMP)
int hook_detach(uintptr_t fish, hook_t);
int hook_error(char *buf, size_t buflen);

enum {HOOK_EUNKNOWN, HOOK_ECOOLBOX_ALLOC, HOOK_ECOOLBOX_PROTON, HOOK_ECOOLBOX_PROTOFF, HOOK_EFISH_PROTON, HOOK_EFISH_PROTOFF, HOOK_ECOOLBOX_DEALLOC};
#ifdef __cplusplus
}
#endif
#endif

