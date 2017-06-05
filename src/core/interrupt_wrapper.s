# This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
# Copyright Airbus Group
.file "interrupt_wrapper.s"

/* Code section */
.text

.globl idt_switch
.type  idt_switch,"function"

/* Array defined and init interrupt_handler.c/init_handler_array()*/
.extern interrupt_handlers


generic_handle:
/* Save registers */
	pusha
    push %ss
    push %ds
    push %es
    push %fs
    push %gs
/* Prepare argument context_t* */
	mov %esp, %eax
/* Get the interrupt number */
	mov 52(%esp), %ebx
/* Restore default DS to access interrupt_handler array */
	mov $0x10, %ecx
	movw %cx, %ds // WARNING : suppose that GDTR is correctly configured !!
/* Call the right handler */
	lea  interrupt_handlers, %edi
   	call  *(%edi,%ebx,4)
	pop %gs
	pop %fs
	pop %es
	pop %ds
	pop %ss
   	popa
   	add $8, %esp
   	iret

	.align	16
idt_switch:
/* 0  #DE : Divide Error Exception         N */
	push	$-1	/* fake error code */
	push	$0 	/* Exception number */
	jmp	generic_handle
	.align	16

/* 1  #DB : Debug Exception                N */
	push	$-1
	push	$1
	jmp	generic_handle
	.align	16

/* 2  NMI Interrupt                        N */
	push	$-1
	push	$2
	jmp	generic_handle
	.align	16

/* 3  #BP : Breakpoint Exception           N */
	push	$-1
	push	$3
	jmp	generic_handle
	.align	16

/* 4  #OF : Overflow Exception             N */
	push	$-1
	push	$4
	jmp	generic_handle
	.align	16

/* 5  #BR : Bound Range exceeded exception N */
	push	$-1
	push	$5
	jmp	generic_handle
	.align	16

/* 6  #UD : Invalid Opcode Exception       N */
	push	$-1
	push	$6
	jmp	generic_handle
	.align	16

/* 7  #NM : Device Not Available Exception N */
	push	$-1
	push	$7
	jmp	generic_handle
	.align	16

/* 8  #DF : Double Fault Exception         Y */
	push	$8
	jmp	generic_handle
	.align	16

/* 9  Coprocessor Segment Overrun          N */
	push	$-1
	push	$9
	jmp	generic_handle
	.align	16

/* 10 #TS : Invalid TSS Exception          Y */
	push	$10
	jmp	generic_handle
	.align	16

/* 11 #NP : Segment Not Present            Y */
	push	$11
	jmp	generic_handle
	.align	16

/* 12 #SS : Stack Fault Exception          Y */
	push	$12
	jmp	generic_handle
	.align	16

/* 13 #GP : General Protection Exception   Y */
	push	$13
	jmp	generic_handle
	.align	16

 /* 14 #PF : Page Fault Exception          Y */
	push	$14
	jmp	generic_handle
	.align	16

/*  15 Reserved	for Intel				   N */
	push	$-1
	push	$15
	jmp	generic_handle
	.align	16

 /* 16 #MF : x87 FPU Floating-Point Error  N */
	push	$-1
	push	$16
	jmp	generic_handle
	.align	16

/*  17 #AC : Alignment Check Exception     Y */
	push	$17
	jmp	generic_handle
	.align	16

/*  18 #MC : Machine-Check Exception       N */
	push	$-1
	push	$18
	jmp	generic_handle
	.align	16

/*  19 #XF : SIMD Floating-Point Exception N */
	push	$-1
	push	$19
	jmp	generic_handle
	.align	16
