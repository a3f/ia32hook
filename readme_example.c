#include <stdio.h>

int print(int mode, const char* s) { (void) mode; return puts(s); }
int print_hook(int (*orig)(), int, const char*);

int main(void)
{
}

// if you like it ugly V can be written as HOOK(orig, const char *s)
int print_hook(int (*orig)(), int mode, const char *s)
{
  switch(mode)
  {
    case 0:
      return orig(mode, s);
    case 1:
      return -1;
    default:
      return printf("[%d]: %s", mode, s);
  }
}
#define ia32hook_orig(hook) ({void* ptr = (void*)hook-4; (__typeof__(hook))ptr;})
int print_hook(int mode, const char*s)
{
  switch(mode)
  {
    case 0:
      return ia32hook_orig(print_hook);
    case 1:
      return -1;
    default:
      return printf("[%d]: %s", mode, s);
  }

}

