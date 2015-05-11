/*! 
 *  \brief     Source file for ia32hook. You are supposed to link with this.
 *  \author    Ahmad Fatoum
 *  \version   1.0
 *  \date      2014
 *  \copyright MIT License (See LICENSE)
 */

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
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "hook.h"
#include "ollydisasm/disasm.h"

#undef GET_PAGE
#ifdef _WIN32
#include "win32.h"
#define GET_PAGE PTR //VirtualAlloc seems to do this internally
#else
#ifndef MAP_ANON
#define MAP_ANON MAP_ANONYMOUS
#endif
#include <sys/mman.h>
#include <unistd.h>
#define GET_PAGE(addr) ((void*)((uintptr_t)(addr) & ~(getpagesize() - 1)))
#endif

#undef mempcpy
#define mempcpy(dst, src, len) ((char*)memcpy((dst), (src), (len)) + (len))

#undef PTR
#undef ADDR
#undef DATA
#define PTR(addr_) (conv.addr=addr_, conv.ptr)
#define ADDR(ptr_) (conv.ptr=ptr_, conv.addr)
#define DATA(fun_) (conv.fun=fun_, conv.ptr)

#undef JMP_SIZE
#define JMP_SIZE 5
static unsigned long CleanBiteOff(const unsigned char *ptr, size_t amount); // how many bytes should I overwrite to fit a jump and not leave garbage behind 

static union {
	uintptr_t addr;
	char *byte;
	void *ptr;
	void** ptr2ptr;
	hook_t fun;
}conv;

static int hook_errno = HOOK_EUNKNOWN;
hook_t hook_attach(uintptr_t fish_, hook_t hook)
{
	void *fish = PTR(fish_);
	unsigned long safeSize = CleanBiteOff(fish, JMP_SIZE);
	uint8_t *overwrite = malloc(safeSize);

	overwrite[0] = 0xE9;//call e8 -- jmp e9
	for (size_t i = JMP_SIZE; i < safeSize; i++)
		overwrite[i] = 0x90;

	uint8_t* coolbox = mmap(NULL, safeSize + JMP_SIZE, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0); 
	if (coolbox  == MAP_FAILED)
	{
		hook_errno = HOOK_ECOOLBOX_ALLOC;
		return NULL; 
	}

	ptrdiff_t hook_rel = ADDR(DATA(hook)) - fish_ - JMP_SIZE;
	ptrdiff_t orig_rel = fish_ - ADDR(coolbox) - JMP_SIZE;	
	(void)memcpy(&overwrite[1], &hook_rel, JMP_SIZE-1);

	(void)mempcpy(mempcpy(mempcpy(coolbox, 
			fish, safeSize),
			"\xE9",	   1),
			&orig_rel, JMP_SIZE-1);

	if (mprotect(GET_PAGE(fish_), safeSize, PROT_READ | PROT_EXEC | PROT_WRITE) == -1) 
	{
		hook_errno = HOOK_EFISH_PROTOFF;
		return NULL;
	}
	
//README: atomic memcpy? process should be suspended while patching imo
	(void)memcpy(fish, overwrite, safeSize);
	
	if (mprotect(GET_PAGE(fish_), safeSize, PROT_READ | PROT_EXEC) == -1) 
	{
		hook_errno = HOOK_EFISH_PROTON;
		return NULL;
	}

	return (conv.ptr=coolbox, conv.fun);
}
int hook_detach(uintptr_t fish_, hook_t coolbox)
{
	void *fish = PTR(fish_);
	uint8_t *instruction = fish;
	if (*instruction != 0xE9)
		return -1;
	instruction+= JMP_SIZE;

	int safeSize = JMP_SIZE;
	while (*(instruction++) == 0x90) safeSize++; // count nops

	if (mprotect(GET_PAGE(fish_), safeSize, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) 
	{
		hook_errno = HOOK_EFISH_PROTOFF;
		return -1;
	}

	(void)memcpy(fish, DATA(coolbox), safeSize);

	if (mprotect(GET_PAGE(fish_), safeSize, PROT_READ | PROT_EXEC) == -1) 
	{
		hook_errno = HOOK_EFISH_PROTOFF;
		return -1;
	}

	if (munmap(DATA(coolbox), safeSize + JMP_SIZE) != 0)
	{
		hook_errno = HOOK_ECOOLBOX_DEALLOC;
		return -1;
	}
	return 0;
}
int hook_error(char *buf, size_t buf_len)
{
	(void) buf;
	(void) buf_len;
	return hook_errno;
}

static inline unsigned long CleanBiteOff(const unsigned char *ptr, size_t amount)
{
	static t_disasm disasm;
	unsigned long size = 0;
	do {size += Disasm(ptr+size, 16, 0x00401AAC, &disasm, DISASM_SIZE);} while(size < amount);
	return size;
}

