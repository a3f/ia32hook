#ifndef HOOK_H
#define HOOK_H
#include <stdint.h>

//void attachHook (void (*fish)(), void(*hook)(), int32_t worms);
typedef __stdcall int (*hook_t)();
int attachHook(uintptr_t fish, hook_t hook);
int detachRawHook(uintptr_t fish);
enum HookFailedOn {HOOK_ALL_OK = 0
		  , HOOK_VIRTUALALLOC_COOLBOX
		  ,HOOK_PROTECTION_DISABLE
		  ,HOOK_PROTECTION_ENABLE
		  ,HOOK_COOLBOX_EXECUTABLE
		  };
//enum HookFailedOn attachHook (uintptr_t fish, uintptr_t hook, int32_t worms);

typedef struct {int action; int retval;}HOOK;
#define CONTINUE (HOOK){.action=0}
#define BREAK(X) (HOOK){.action=1, .retval=X}
#define hook(name, ...) int name(int (*func)(), ...) 
#endif

