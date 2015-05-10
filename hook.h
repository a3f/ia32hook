#ifndef HOOK_H
#define HOOK_H
#include <stdint.h>

typedef int (*hook_t)();
hook_t hook_attach(uintptr_t fish, hook_t hook);
int hook_detach(uintptr_t fish, hook_t);
const char* hook_error(void*);
enum HookFailedOn {HOOK_ALL_OK = 0
		  , HOOK_VIRTUALALLOC_COOLBOX
		  ,HOOK_PROTECTION_DISABLE
		  ,HOOK_PROTECTION_ENABLE
		  ,HOOK_COOLBOX_EXECUTABLE
		  };
static union {
	uintptr_t addr;
	char *byte;
	void *ptr;
	void** ptr2ptr;
	hook_t fun;
}conv;

#endif

