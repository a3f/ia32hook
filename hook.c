/* Terminology:
 *
 * fish = function to patch
 * hook = patch to apply
 * coolbox = overwritten bytes and stack fixer
 * scales = memory security flags
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stddef.h>
#include "hook.h"
#include "ollydisasm/disasm.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#define GET_PAGE(addr) ((void*)((uintptr_t)(addr) & ~(getpagesize() - 1)))
#endif
#define mempcpy(dst, src, len) ((char*)memcpy((dst), (src), (len)) + (len))
#define PTR(addr_) (conv.addr=addr_, conv.ptr)
#define ADDR(ptr_) (conv.ptr=ptr_, conv.addr)
#define DATA(fun_) (conv.fun=fun_, conv.ptr)

#define JMP_SIZE 5
static unsigned long CleanBiteOff(const unsigned char *ptr, size_t amount); // how many bytes should i overwrite to fit a jump and not leave garbage behind 
hook_t attachHook(uintptr_t fish, hook_t hook)
{
	unsigned long safeSize = CleanBiteOff(PTR(fish), JMP_SIZE);
	uint8_t *overwrite = malloc(safeSize);

	overwrite[0] = 0xE9;//call e8 -- jmp e9
	for (size_t i = JMP_SIZE; i < safeSize; i++)
		overwrite[i] = 0x90;

#ifdef _WIN32
	DWORD oldScales, newScales;
	uint8_t* coolbox = VirtualAlloc(NULL, safeSize + JMP_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); 
	if (coolbox  == NULL)
#else
	uint8_t* coolbox = mmap(NULL, safeSize + JMP_SIZE, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0); 
	if (coolbox  == MAP_FAILED)
#endif
		return NULL;
	ptrdiff_t hook_rel = ADDR(DATA(hook)) - fish - JMP_SIZE;
	ptrdiff_t orig_rel = fish - ADDR(coolbox) - JMP_SIZE;	
	memcpy(&overwrite[1], &hook_rel, JMP_SIZE-1);

	printf("att=%p\n", coolbox);
	mempcpy(mempcpy(mempcpy(coolbox, 
			PTR(fish), safeSize),
			"\xE9",	   1),
			&orig_rel, JMP_SIZE-1);
	printf("att=%p\n", coolbox);
#if _WIN32	
	if (VirtualProtect(PTR(fish), safeSize, PAGE_READWRITE, &oldScales) == 0)
#else
		printf("mprotect");
	fprintf(stderr, "fish:%x,size:%x,page:%x\n", fish, getpagesize(), GET_PAGE(fish));
	if (mprotect(GET_PAGE(fish), safeSize, PROT_READ | PROT_EXEC | PROT_WRITE) == -1) 
#endif
		return NULL;
	printf("didnt fail");
//TODO: atomic memcpy
	printf("att=%p\n", coolbox);
	memcpy(PTR(fish), overwrite, safeSize);
	
	printf("att=%p\n", coolbox);
#if _WIN32
	if (VirtualProtect(PTR(fish), safeSize, oldScales, &newScales) == 0)
#else
	if (mprotect(GET_PAGE(fish), safeSize, PROT_READ | PROT_EXEC) == -1) 
#endif
		return NULL;
	printf("att=%p\n", coolbox);
	return (conv.ptr=coolbox, conv.fun);
	
}
int hook_detach(uintptr_t fish_, hook_t coolbox)
{
	void *fish = PTR(fish_);
	uint8_t *instruction = fish;
	if (*instruction != 0xE9)
		return -1;
	instruction+= JMP_SIZE;
	int nops = 0;
	while (*(instruction++) == 0x90) nops++;
	int safeSize = JMP_SIZE + nops;

#ifdef _WIN32	
	DWORD oldScales, newScales; 
	if (VirtualProtect(fish, safeSize, PAGE_READWRITE, &oldScales) == 0)
#else
	if (mprotect(GET_PAGE(fish), safeSize, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) 
#endif
		return HOOK_PROTECTION_DISABLE;

	memcpy(fish, DATA(coolbox), safeSize);

#ifdef _WIN32
	if (VirtualProtect(fish, safeSize, oldScales, &newScales) == 0)
#else
	if (mprotect(GET_PAGE(fish), safeSize, PROT_READ | PROT_EXEC) == -1) 
#endif
		return HOOK_PROTECTION_ENABLE;
	printf("det=%p\n", DATA(coolbox));
#ifdef _WIN32
	return -!VirtualFree(DATA(coolbox), 0, MEM_RELEASE);
#else
return munmap(DATA(coolbox), safeSize + JMP_SIZE);
#endif
}
const char *hook_error()
{

}
static unsigned long CleanBiteOff(const unsigned char *ptr, size_t amount)
{
	static t_disasm disasm;
	unsigned long size = 0;
	do {size += Disasm(ptr+size, 16, 0x00401AAC, &disasm, DISASM_SIZE);} while(size < amount);
	return size;
}

