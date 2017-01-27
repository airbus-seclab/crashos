.text
.globl entry
.type  entry,"function"

.code32

entry:
        cli
        movl    $__kernel_start__, %esp
        pushl   $0
        popf
        movl    %ebx, %eax
        call    start

halt:
        hlt
        jmp     halt
