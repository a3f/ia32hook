#include <stdio.h>

int print(int mode, const char *s) { (void)mode; return puts(s); }
int print_hook1(int mode, const char *s);

int main(void)
{
  int (*print_without_mode)() = ia32hook_bind(print, "%d %*s", 0);
  print_without_mode("Hello World!");

  ia32hook_t hook;
  ia32hook_init(hook, print);
  
  print(1, "Hello World!");

  ia32hook_add(hook, print_hook1);
  print(1, "Hello World!");
 
  ia32hook_remove(hook);
  print(1, "Hello World!");


}

int print_hook1(int (*orig)(), int mode, const char *s)
{
  if (mode == 0)
    return orig(mode, s);
  else if (mode > 0)
    return printf("%s: %s", __func__, s);
  else
    return -1;
}

int print_hook2(int mode, const char *s)
{
  if (mode == 0)
    return ia32hook_orig(mode, s);
  else if (mode > 0)
    return printf("%s: %s", __func__, s);
  else
    return -1;
}

