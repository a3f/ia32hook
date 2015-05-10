#ifndef _IA32HOOK_HOOK_H_
#define _IA32HOOK_HOOK_H_
#include <stdint.h>
#include <stddef.h>
#if UINTPTR_MAX != 0xffffffff || (defined(_WIN32) && !defined(_M_IX86))
#error Only IA32 (x86 - 32 bits) is supported
#endif

typedef int (*hook_t)();
hook_t hook_attach(uintptr_t fish, hook_t hook);
int hook_detach(uintptr_t fish, hook_t);
int hook_error(char *buf, size_t buflen);

enum {HOOK_EUNKNOWN, HOOK_ECOOLBOX_ALLOC, HOOK_ECOOLBOX_PROTON, HOOK_ECOOLBOX_PROTOFF, HOOK_EFISH_PROTON, HOOK_EFISH_PROTOFF, HOOK_ECOOLBOX_DEALLOC};

#endif

