
#include "stdint-gcc.h"

#ifndef vu32
#	define vu32 volatile uint32_t
#endif

void LoopCopyDataInit(void) {
	extern char _sidata asm("_sidata");
	extern char _sdata asm("_sdata");
	extern char _edata asm("_edata");
	
	vu32* src = (vu32*) &_sidata;
	vu32* dst = (vu32*) &_sdata;
	
	vu32 len = (&_edata - &_sdata) / 4;
	
	for(vu32 i=0; i < len; i++)
		dst[i] = src[i];
}

void LoopFillZerobss(void) {
	extern char _sbss asm("_sbss");
	extern char _ebss asm("_ebss");
	
	vu32* dst = (vu32*) &_sbss;
	vu32 len = (&_ebss - &_sbss) / 4;
	
	for(vu32 i=0; i < len; i++)
		dst[i] = 0;
}

void __gcc_data_init(void) {
	LoopFillZerobss();
	LoopCopyDataInit();
}
