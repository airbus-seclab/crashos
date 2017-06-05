# This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
# Copyright Airbus Group
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
