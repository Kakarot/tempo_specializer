## Tempo, Copyright INRIA (C) 2002.  All rights reserved.
##
## Tempo, copyright (C) Irisa 95
##
## file/version: $Id: rtcg.mk,v 1.13 2000/07/08 13:59:11 chang Exp $
## author(s):    F. Noel P. Boinot
## date:         $Date: 2000/07/08 13:59:11 $
##
SYSTEM_SH = uname -s -r | sed -e 's/ /-/g' -e 's/\..*//' | tr -d \\012
SYSTEM = $(shell $(SYSTEM_SH))$(SYSTEM_SH:sh)

# NB: Again, these have to be kept in sync w/ several other
# scripts/makefiles, but its basically ganked from what the sml/nj
# build system does.
OS	:= $(strip $(shell uname -s))
OS_REV	:= $(strip $(shell uname -r | sed -e 's/ /-/g' -e 's/\..*//'))

# Check to see if we have the fully expanded x86 chip type.
ARCH 	:= $(strip $(shell uname -m))
ifneq (,$(findstring i.86,$(ARCH)))
	ARCH := x86
endif # x86

SYSTEM := $(strip $(shell $(TEMPOHOME)/bin/arch-n-opsys))

#include /udd/pe/tempo/work/makefile.defs

ifeq ($(OS),SunOS)
	ifeq ($(OS_REV), 4)
		FLAGS= -DSUNOS4
	endif
	ifeq ($(OS_REV), 5)
		FLAGS= -DSUNOS5
	endif
endif # SunOS
ifeq ($(OS), Linux)
	FLAGS= -DLINUX2
endif #Linux

RTCG_DIR    = $(TEMPOHOME)/rtcg

##
## Templates compilation
## ---------------------
## - BASE_NAME        : base name for the program
## - ARCH_DP_BASE_NAME: base name for architecture dependant files
## - BASE_TEMP        : base name for template files
## - ARCH_DP_BASE_TEMP: base name for architecture dependant template file
## - CC               : compiler to use to compile templates
## - CFLAGS           : compiler flags
## - ANALYSE          : a command that performs further analysis on templates
##                    : if needed (and removes the generated file otherwise)
##
TEMP_C = $(BASE_TEMP).c
TEMP_O = $(ARCH_DP_BASE_TEMP).o
TCC    = $(RTCG_DIR)/tcc/$(SYSTEM)/tcc

object_templates:
	$(COMPILE.c) $(TEMP_C) -o $(TEMP_O)
	$(TCC) $(BASE_TEMP) $(ARCH_DP_BASE_TEMP)
	$(ANALYSE)

##
## Specilizer production
## ---------------------
## - OUT_NAME         : name for the final result
## - ARCH_DP_BASE_TEMP: base name for templates files
## - BASE_RTS         : base name for specilizer files
## - ARCH_DP_BASE_RTS : base name for architecture dep specilizer files
## - CC               : compiler to compile specializer
## - CFLAGS           : compiler flags
##
RTS_C        = $(BASE_RTS).c
RTS_O        = $(ARCH_DP_BASE_RTS).o
RTS_OBJS_DIR = $(RTCG_DIR)/rts/$(SYSTEM)

RTS_LFLAGS   = -L$(RTS_OBJS_DIR) -lrts
CFLAGS 	     = -DTEMPLATE_OBJECT_FNAME=\"$(TEMP_O)\"

specializer:
	$(COMPILE.c) $(CFLAGS) $(FLAGS) $(RTS_C) -o $(RTS_O)
	ld -r -o $(OUT_NAME) $(RTS_O) $(TEMP_O) $(RTS_LFLAGS)
