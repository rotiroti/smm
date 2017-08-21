# Makefile
.PHONY: clean

# -------------------------------------------
# Insert here the dirpath of libsmm.a
# ------------------------------------------- 
SMMLIBDIR = $(HOME)/smm

# -------------------------------------------
# When MODE is SMM, it compiles for smm project
# and links to libsmm.a, otherwise it compiles
# for executing the test simulation using native
# Linux syscalls.
# -------------------------------------------
MODE = SMM
#MODE = 

# Compiler
CC = gcc

# SMM_VERBOSE defines that program tracing is enabled
VERBOSE = -D SMM_VERBOSE

# SMM defines that the test is for smm (not for Linux syscalls)
ifeq ($(MODE),SMM)
  SMMLIB = -L$(SMMLIBDIR) -lsmm
  SMMLIBFILE = $(SMMLIBDIR)/libsmm.a
else
  SMMLIB = -L. -lsmmsim
  SMMLIBFILE = libsmmsim.a
endif

# Additional flags for gcc compiler
MYFLAGS = $(SMMDEF) $(VERBOSE)

# Standard flags for gcc compiler
CFLAGS = -O -Wall -Wundef -Wpointer-arith -Wconversion -Wsign-compare \
         -Wwrite-strings -Wshadow -Wstrict-prototypes -Wmissing-prototypes \
         -Wmissing-declarations
CFLAGS += $(MYFLAGS)

EXE = smm_test
OBJS = smm_test.o

all: $(EXE)

$(EXE): $(OBJS) $(SMMLIBFILE)
	$(CC) -o $@ $(OBJS) $(SMMLIB)

smm_test.o: smm_test.c smm_test.h smm_lib.h
	$(CC) -c $(CFLAGS) $< 

smm_libsim.o: smm_libsim.c smm_libsim.h
	$(CC) -c $(CFLAGS) $< 

libsmmsim.a: smm_libsim.o
	ar rcs $@ $^

clean:
	rm -f *.o *.a $(EXE) core *~
