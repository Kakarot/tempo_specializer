# Copyright (C) 1996-97 INRIA - Rennes
#
# file/version: $Id: ctcg.mk,v 1.23 2000/07/08 13:35:45 chang Exp $
# author(s):    Renaud Marlet
# date:         $Date: 2000/07/08 13:35:45 $

#
# The following parameters must be provided:
#  - EV_C   : name of the EV fragments source file     ("ev.C" C file)
#  - CTS_C  : name of the action source file           ("cts.C" C++ file)
#  - SCXT_C : name of the specializer initializer file ("sctx.c" C file)
#  - CTS    : name of the specializer executable       ("cts" binary file)
#

#
# The following looks strange but it sets 'SYSTEM' and 'LIBMALLOC' correctly
# for both make and gnu-make (on SunOS, Solaris and Linux).
# I.e., $(shell...) returns an empty string with make; so does $(SYSTEM_SH:sh)
# with gun-make.
#
SYSTEM_SH = $(TEMPOHOME)/bin/arch-n-opsys
SYSTEM = $(SYSTEM_SH:sh)$(shell $(SYSTEM_SH))

#
# Use bsdmalloc library on Solaris because Solaris' malloc is buggy
# Same trick as above for the 'SYSTEM' variable to make it work on
# both make and gnu-make
#
SOLARIS_SH = uname -s -r | sed -e 's/ /-/g' -e 's/\..*//' | tr -d \\012
SOLARIS_TEST = $(SOLARIS_SH:sh)$(shell $(SOLARIS_SH))
LIBMALLOC_SH = if test $(SOLARIS_TEST) = SunOS-5; then echo -lbsdmalloc; fi
LIBMALLOC = $(LIBMALLOC_SH:sh)$(shell $(LIBMALLOC_SH))

# In order to use egcs instead of gcc, update variables CC
# and CXX, and update the Makefile (see comments in this file)

CC = gcc
CCC = gcc
CFLAGS = -O2
CCCFLAGS = -O2

CFLAGS += -I$(CTCG)
CCCFLAGS += -I$(CTCG)

CTCG = $(TEMPOHOME)/ctcg
CTCG_DIR = $(CTCG)/$(SYSTEM)
CTCG_LIBS = -lctcg $(LIBMALLOC)

SCTX_H = $(SCTX_C:.c=.h)

OBJECTS = $(EV_O) $(CTS_O) $(SCTX_O)

$(CTS): $(EV_O) $(CTS_O) $(SCTX_O)
	$(LINK.C)  -L$(CTCG_DIR) $(OBJECTS) $(CTCG_LIBS) -o $(CTS) $(LDLIBS)

$(EV_O): $(EV_C)
	$(COMPILE.c) -I$(CTCG) $(EV_C) -o $@

$(CTS_O): $(CTS_C)
	$(COMPILE.C) -I$(CTCG) $(CTS_C) -o $@

$(SCTX_O): $(SCTX_C) $(SCTX_H)
	@grep -l "^#include.*\.sctx\.h" $(SCTX_C) > /dev/null || \
	  (echo "*** A \`\` #include \"`basename $(SCTX_H)`\" '' statement must appear in `basename $(SCTX_C)`"; \
	   exit 1)
	$(COMPILE.c) $(SCTX_C) -o $@
