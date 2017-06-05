// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#include <core/types.h>
#include <core/segmentation.h>
#include <core/page.h>
#include <core/interrupts.h>
#include <core/print.h>
#include <core/start.h>



isr_t interrupt_handlers[256];
extern mem_info_t mem_info;

/**
 * Handler of the Double Fault Exception
 * 8  #DF : Double Fault Exception (Y)
 *
 * @param ctx fault context
 */
void __attribute__((regparm(1))) DF_handler(context_t* ctx) {
	if (ctx == NULL) {
		printf_log("DF_handler : Null Parameter.\n");
		return;
	}
	printf_log("DF_handler : Double Fault eip 0x%x\n", ctx->eip);
	panic();
	return;
}

/**
 * Handler of the Global Protection Exception
 * 13 #GP : General Protection Exception (Y)
 *
 * @param ctx fault context
 */
void __attribute__((regparm(1))) GP_handler(context_t* ctx) {
	if (ctx == NULL) {
		printf_log("GP_handler : Null Parameter.\n");
		return;
	}
	if (ctx->error_code != 0) {
		printf_log("GP_handler : Segmentation fault at 0x%x (error code : 0x%x)\n", ctx->eip, ctx->error_code);
		panic();
	} else {
		printf_log("GP_handler : General Protection fault at 0x%x (error code : 0x%x)\n", ctx->eip, ctx->error_code);
		panic();
	}
	return;
}

/**
 * Handler of the Page Fault exception
 * 14 #PF : Page Fault Exception (Y)
 * @param ctx fault context
 */
/*  PF Error Code Format :
 *  31                                   4    3    2    1    0
 *  |----------------------------------|----|----|----|----|----|
 *   __________________________________ ____ ____ ____ ____ ____
 *  |             Reserved             |I/D |RSVD|U/S |W/R |P   |
 *  |__________________________________|____|____|____|____|____|
 */
void __attribute__((regparm(1))) PF_handler(context_t* ctx) {
	uint32_t cr2;
	int p, w_r, u_s, rsvd, i_d;

	if (ctx == NULL) {
		printf_log("PF_handler : Null Parameter.\n");
		return;
	}
	/* CR2 (=fault_address) */
	get_cr2(cr2);
	printf_log("#PF (cr2 0x%x eip 0x%x)\n", cr2, ctx->eip);

	/* Error code interpretation */
	p    = (ctx->error_code   ) & 1;
	w_r  = (ctx->error_code>>1) & 1;
	u_s  = (ctx->error_code>>2) & 1;
	rsvd = (ctx->error_code>>3) & 1;
	i_d  = (ctx->error_code>>4) & 1;

	if(p == 0) {
		allocate_missing_page(cr2, &mem_info);
		//printf_log("Warning : Page fault handled (Page allocated : p=0)\n");
		return;
	} else {
		printf_log("Page-level protection violation (p=1)\n");
	}
	if(w_r == 0) {
		printf_log("Read access not allowed (w_r=0)\n");
	} else {
		printf_log("Write access not allowed (w_r=1)\n");
	}

	if(u_s == 0) {
		printf_log("Execution in supervisor mode (u_s=0)\n");
	} else {
		printf_log("Execution in user mode (u_s=1)\n");
	}

	if (rsvd == 1) {
		printf_log("Reserved bit violation (set to 1) in PGD (rsvd=1)\n");
	}
	if (i_d == 1) {
		printf_log("Instruction fetch failed (i_d=1)\n");
	}
	panic();
}

/**
 * Default handler of x instruction
 * @param ctx context in the stack when the interrupt occurs
 */
void __attribute__((regparm(1))) default_handler(context_t* ctx) {
	printf_log("Default Handler : IT/EXCP %d at eip 0x%x \n", ctx->IT_number, ctx->eip);
	panic();
}

void init_handler_array() {
	int i;
	for(i=0; i<256; i++){
		interrupt_handlers[i] = default_handler;
	}
	interrupt_handlers[8]  = (void *) DF_handler;
	interrupt_handlers[13] = (void *) GP_handler;
	interrupt_handlers[14] = (void *) PF_handler;
}
