/* Terminology:
 *
 * fish = function to patch
 * hook = patch to apply
 * coolbox = overwritten bytes and stack fixer
 * scales = memory security flags
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <windows.h>
#include "hook.h"
#include "ollydisasm/disasm.h"

static union {
	uintptr_t addr;
	void *ptr;
	hook_t fun;
}conv;
#define PTR(addr_) (conv.addr=addr_, conv.ptr)
#define ADDR(ptr_) (conv.ptr=ptr_, conv.addr)
#define DATA(fun_) (conv.fun=fun_, conv.ptr)
static unsigned long CleanBiteOff(const uint8_t *ptr, size_t amount); // how many bytes should i overwrite to fit a jump and not leave garbage behind 
int attachHook(uintptr_t fish, hook_t hook)
{
	#define JMP_SIZE 5
	unsigned long safeSize = CleanBiteOff(PTR(fish), JMP_SIZE);
	uint8_t *overwrite = malloc(safeSize);

	overwrite[0] = 0xE9;//call e8 -- jmp e9
	for (size_t i = JMP_SIZE; i < safeSize; i++)
		overwrite[i] = 0x90;

	DWORD oldScales, newScales;
	
	uint8_t* coolbox /* asm("%ecx") */; // dat could save 9 bytes  on gcc
	#define XCHG_SIZE 13
	#define FOOTER_SIZE 2*JMP_SIZE+XCHG_SIZE
	if ((coolbox = VirtualAlloc(NULL, safeSize + FOOTER_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)) == NULL)
		return HOOK_VIRTUALALLOC_COOLBOX;
	ptrdiff_t relative = (uintptr_t)coolbox + safeSize - fish;
	
	memcpy(&overwrite[1], &relative, JMP_SIZE-1);
	memcpy(coolbox, PTR(fish), safeSize);
	relative = -relative;
	/*
	push ecx
	mov ecx, %coolbox
	xchg dword [esp+4], ecx
	xchg dword[esp], ecx

	 {0x51,0xB9,0,0,0,0,0x87,0x4C,0xE4,0x04,0x87,0x0C,0xE4}
	*/
	#define FOUR_BYTES 0,0,0,0 //placeholder
	uint8_t coolboxfooter[FOOTER_SIZE] = {0xE9,FOUR_BYTES,0x51,0xB9,FOUR_BYTES,0x87,0x4C,0xE4,0x04,0x87,0x0C,0xE4, 0xE9/*,FOUR_BYTES*/};
	memcpy(&coolboxfooter[1],&relative , JMP_SIZE-1);
	memcpy(&coolboxfooter[JMP_SIZE+2], &coolbox, JMP_SIZE-1);
	relative = ADDR(DATA(hook)) - (ADDR(coolbox) + safeSize + FOOTER_SIZE);
	memcpy(&coolboxfooter[JMP_SIZE+XCHG_SIZE+1], &relative, JMP_SIZE-1);
	memcpy(coolbox+safeSize, coolboxfooter, FOOTER_SIZE);

	if (VirtualProtect(PTR(fish), safeSize, PAGE_READWRITE, &oldScales) == 0)
		return HOOK_PROTECTION_DISABLE;

	memcpy(PTR(fish), overwrite, safeSize);
	
	if (VirtualProtect(PTR(fish), safeSize, oldScales, &newScales) == 0)
		return HOOK_PROTECTION_ENABLE;

	return HOOK_ALL_OK;
	
}
int detachRawHook(uintptr_t fish_)
{
	void *fish = PTR(fish_);
	uint8_t *instruction = fish;
	if (*instruction != 0xE9)
		return -1;
	uint32_t coolbox;
	memcpy(&coolbox, instruction + 1, 4);
	instruction+= JMP_SIZE;
	int nops = 0;
	while (*(instruction++) == 0x90) nops++;
	int safeSize = 5 + nops;

	DWORD oldScales, newScales; 
	if (VirtualProtect(fish, safeSize, PAGE_READWRITE, &oldScales) == 0)
		return HOOK_PROTECTION_DISABLE;
	memcpy(fish, PTR(coolbox - safeSize + fish_), safeSize);
	if (VirtualProtect(fish, safeSize, oldScales, &newScales) == 0)
		return HOOK_PROTECTION_ENABLE;
	VirtualFree(&coolbox, safeSize + FOOTER_SIZE, MEM_RELEASE);

	return 0;
}
static unsigned long CleanBiteOff(const uint8_t *ptr, size_t amount)
{
	static t_disasm disasm;
	unsigned long size = 0;
	do {size += Disasm(ptr+size, 16, 0x00401AAC, &disasm, DISASM_SIZE);} while(size < amount);
	return size;
}

