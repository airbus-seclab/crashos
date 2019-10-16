# This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
# Copyright Airbus Group
#!/usr/bin/make -f

##
## Paths
##
SRC_DIR    := src
BIN_DIR    := build
TOOL_DIR   := tools
KERNEL     := $(BIN_DIR)/test.bin
LDSCRIPT   := $(BIN_DIR)/linker.lds


##
## Flags
##
MAKEFLAGS  := --no-print-directory

CC_MODEL   := small
#CCVER      := 4.8
#CC         := $(shell which gcc-$(CCVER))
#CPP        := $(shell which cpp-$(CCVER))
CC         := $(shell which gcc)
CPP        := $(shell which cpp)
LD         := $(shell which ld)
RM         := $(shell which rm)
MKDIR      := $(shell which mkdir)
DOXYGEN    := $(shell which doxygen)
MAKE       := $(shell which make)
CP         := $(shell which cp)
SED        := $(shell which sed)
FIND       := $(shell which find)
INSTOOL    := $(TOOL_DIR)/installer_vmware.sh

CFLG_WRN   := -Wall -W
CFLG_KRN   := -pipe -nostdlib -nostdinc -ffreestanding -fms-extensions
CFLG_FP    := -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3 -mno-sse4.1 \
              -mno-sse4.2 -mno-sse4 -mno-avx -mno-avx2 -mno-aes -mno-pclmul \
              -mno-fsgsbase -mno-rdrnd -mno-f16c -mno-fma -mno-sse4a \
              -mno-fma4 -mno-xop -mno-lwp -mno-3dnow -mno-popcnt \
              -mno-abm -mno-bmi -mno-bmi2 -mno-lzcnt -mno-tbm

CFLG_32    := -m32
CCLIB_32   := $(shell $(CC) -m32 -print-libgcc-file-name)
LDFLG_32   := -melf_i386

CFLAGS     := $(CFLG_WRN) $(CFLG_KRN)
LDFLAGS    := --warn-common --no-check-sections -n
INCLUDE    := -I$(SRC_DIR) -I$(SRC_DIR)/core

CCLIB         := $(CCLIB_32)
EXTRA_CFLAGS  := $(CFLG_32)
EXTRA_LDFLAGS := $(LDFLG_32)



##
## Rules
##
define inst
$(INSTOOL) $<
echo "    CP    $<"
endef

define compile
echo "    CC    $<"
$(CC) $(INCLUDE) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<
endef

define assemble
echo "    AS    $<"
$(CPP) $< $(CFLAGS) $(EXTRA_CFLAGS) -o $<.s
$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<.s 
$(RM) $<.s
endef

define aggregate
echo "    LD    $@"
$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) -r -o $@ $^
endef

define link
echo "    LD    $@"
$(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) $(EXTRA2_LDFLAGS) -T $(LDSCRIPT) $^ -o $@ $(CCLIB)
endef

%.o: %.c
	@$(compile)
%.o: %.s
	@$(assemble)



###
### TARGETS
###

### Create test objects
ifneq ($(tbuild),)

tdir = $(dir $(tbuild))
include  $(tdir)/Makefile
tdep = $(addprefix $(tdir), $(obj))

$(tbuild): $(tdep)
	@$(aggregate)

### Create kernel
else
TESTS := stable_tests/tests_Prvlg_Instr/test_06       \
         stable_tests/tests_REP_OUTS/test_01          \
         stable_tests/tests_REP_OUTS/test_02          \
         stable_tests/tests_REP_OUTS/test_03          \
         stable_tests/tests_REP_OUTS/test_03b         \
         stable_tests/tests_REP_OUTS/test_04          \
         stable_tests/tests_REP_OUTS/test_05          \
         stable_tests/tests_REP_OUTS/test_07          \
         stable_tests/tests_REP_OUTS/test_07b         \
         stable_tests/tests_REP_OUTS/test_07c         \
         stable_tests/tests_REP_OUTS/test_08          \
         stable_tests/tests_REP_OUTS/test_15_odd_nbr  \
         stable_tests/tests_REP_OUTS/test_15_even_nbr \
 

 

CORE :=	core/entry.o				\
		core/start.o				\
		core/setup.o				\
		core/utils.o				\
		core/string.o				\
		core/video.o				\
		core/serial_driver.o		\
		core/print.o				\
		core/init.o					\
		core/panic.o				\
		core/interrupt_handler.o	\
		core/interrupt_wrapper.o	\
		core/page.o                 \
        core/keyboard.o


## append path
KRN_OBJ	:= $(addprefix $(SRC_DIR)/, $(CORE))
TST_OBJ := $(addsuffix /build.o, $(addprefix $(SRC_DIR)/, $(TESTS)))

.PHONY: clean $(TST_OBJ)

all: $(TST_OBJ) $(KERNEL)

$(TST_OBJ):
	@$(MAKE) tbuild=$@

$(KERNEL): $(KRN_OBJ) $(TST_OBJ)
	@$(link)

install: $(KERNEL)
	@$(inst)

clean:
	@$(RM) -f $(KERNEL)
	@$(MAKE) -C docs clean
	@$(FIND) . -name \*.[od] | xargs $(RM) -f
		
doc: documentation

documentation:
	@$(MAKE) -C docs
endif


