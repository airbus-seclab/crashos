.text
.globl panic
.type  panic,"function"

.code32

panic:
	cli
	hlt
	jmp	panic
