## ia32hook - A simple function hooking mechanism for Intel 32-Bit
ia32hook is a tiny library for [hooking][] procedures in the same address space. The hooked function's calling convention needs to be the same as the hook. So for hooking `WINAPI` functions one would need to specify `__stdcall`.

ia32hook is written in the C99 subset of C++11 and is compilable as either. It supports hooking of IA32 (x86 32 bit, i386) code only. It has been tested on Windows 8, OS X Yosemite and Debian 7 but should work on any other Windows/UNIX system too. If your hook isn't within the address space of the function you want to hook, <s>[lade][]</s> A [DLL injector][] can inject your code into the foreign process.<!--note to self: lade's link here when uploaded--> 

The hooking is done by patching the first few instructions with a `jmp` to the hook. the overwritten bytes are copied to an executable buffer and their address is returned by `hook_attach`. `hook_detach` reinstates the bytes and frees the buffer. Suspension of the process does **not** occur. The initial commit code-caved an additional argument push to stack with a pointer to the buffer, but the current static-duration-pointer-way seems neater.

## Example I
	 // redirect the sqrt_ funtion to sqrt_hook and conditionally call the original
	 #include <stdio.h>
	 #include <math.h>	
	 #include "hook.h"
	
	 double sqrt_(double f) {return sqrt(f);} // hooking libc doesn't completely work on all systems yet
	 static hook_t sqrt_orig = sqrt_;
	 double sqrt_hook(double f) {return f >= 0 ? sqrt_orig(f) : pow(f,2);}
	   
	
	 int main(void)
	 {
	     printf("UnHooked:_sqrt(9) = %g, _sqrt(-9) = %g,\n         sqrt_hook(9) = %g, sqrt_hook(-9) = %g\n\n", 
						  _sqrt(9),		 _sqrt(-9),		  		   sqrt_hook(9),	  sqrt_hook(-9 );
	
	     sqrt_orig = hook_attach((uintptr_t)_sqrt, sqrt_hook);
	     printf("  Hooked:_sqrt(9) = %g, _sqrt(-9) = %g,\n         sqrt_hook(9) = %g, sqrt_hook(-9) = %g\n\n", 
						  _sqrt(9),		 _sqrt(-9),		  		   sqrt_hook(9),	  sqrt_hook(-9 );
	
	     hook_detach(sqrt_orig);	
	     printf("DeHooked:_sqrt(9) = %g, _sqrt(-9) = %g,\n         sqrt_hook(9) = %g, sqrt_hook(-9) = %g\n\n", 
						  _sqrt(9),		 _sqrt(-9),		  		   sqrt_hook(9),	  sqrt_hook(-9 );
	
	   return 0;
	 }  

## Example II
	// implement a cls command (clear on unix) to clear the chat channel
	// in the MMORPG Tibia (function signatures and addresses are from v8.60)
	// Use a DLL injector to get this into Tibia's address space

	#include <windows.h>
	#include <string.h>
	#include "hook.h"

	typedef int (*PlayerTalkChannel)(int is7, int channel_id, const char *text);
	typedef int (*ClientClearChannel)(void);

	static const uintptr_t PlayerTalkChannelAddr 	= 0x00407C50;
	static PlayerTalkChannel player_talkchannel;
	static ClientClearChannel client_clearchannel = (ClientClearChannel)0x005481B0;

	
	int PlayerTalkChannelHook (int is7, int channel_id, const char *text)
	{
		if(strcmp(text, "cls") != 0)
			return player_talkchannel(is7, channel_id, text);

		client_clearchannel();
		return 0;
	}
	BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID static_or_terminate)
	{
		(void) static_or_terminate;
		(void) hModule;

		if(dwReason == DLL_PROCESS_ATTACH)
			player_talkchannel = hook_attach(PlayerTalkChannelAddr, PlayerTalkChannelHook);

		return TRUE;
	}
	
## Known bugs
Rehooking an already hooked function currently leads to undefined behavior. Hooking a function shorter than 5 bytes leads to undefined behavior. Hooking a function which uses relative addressing in the instructions containing the first 5 bytes is undefined behaviour. Because most (all?) compilers maintain a frame pointe pointer by default, the patched bytes will be the prologue `push ebp;mov ebp, esp;` and a stack push/sub. So the last one shouldn't be too common. 

Eventually, the ollydbg disassembler will be replaced by a non copy-left engine and these points will be adressed.

## License
The disassembler is	written by [Oleh Yuschuk][] and distributed under the terms of the [GNU GPL 2.0][]. The rest of the code is under the [MIT/X11 license][]. Effectively this means though that the thing as a whole is GPL'd.

[lade]: https://github.com/a3f/lade
[hooking]: https://en.wikipedia.org/wiki/Hooking
[DLL injector]: https://en.wikipedia.org/wiki/DLL_injection
[Oleh Yuschuk]: http://www.ollydbg.de/
[GNU GPL 2.0]: https://www.gnu.org/licenses/gpl-2.0.html
[MIT/X11 license]: http://github.com/a3f/ia32hook/LICENSE

