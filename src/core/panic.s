// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
.text
.globl panic
.type  panic,"function"

.code32

panic:
	cli
	hlt
	jmp	panic
