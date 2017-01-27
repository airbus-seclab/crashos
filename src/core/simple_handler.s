/*
 * simple_handler.s : simple .s to implement a first handler (PF_Handler)
 * Not used in the future
 *  Created on: 17 mai 2016
 *      Author: anais
 */
.text
.extern PF_handler
.globl idt_switch
.type  idt_switch,"function"

idt_switch:
 /* 14 #PF : Page Fault Exception          Y */
	.align	16
	push	$14
	pusha
	mov		%esp, %eax
   	call	PF_handler
   	popa
	add		$8, %esp
   	iret
