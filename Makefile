# Makefile per Simple Memory Manager (smm) 

AR = ar
CC = gcc
CFLAGS = -pedantic -Wall -Wextra -ggdb3 -O0
INC = inc
SRC = src
LIB = lib/libsmm.a
OBJECTS = obj/utility.o obj/smm.o obj/smmMon.o obj/smmMem.o obj/smmd.o
BINARIES = bin/smmMon bin/smmd
INSTALL = install 
SMM_DIR = $(HOME)/smm


all: $(BINARIES) 

obj/utility.o: $(SRC)/utility.c $(INC)/utility.h
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INC)/

obj/smm.o: $(SRC)/smm.c $(INC)/utility.h $(INC)/smm.h
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INC)/

obj/smmMon.o: $(SRC)/smmMon.c $(INC)/utility.h $(INC)/smm.h
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INC)/

obj/smmMem.o: $(SRC)/smmMem.c $(INC)/utility.h $(INC)/smmMem.h
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INC)/

obj/smmd.o: $(SRC)/smmd.c $(INC)/utility.h $(INC)/smmMem.h
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INC)/

lib/libsmm.a: obj/smm.o obj/utility.o
	$(AR) rcs $@ $^

bin/smmMon: obj/smmMon.o obj/utility.o lib/libsmm.a 
	$(CC) $(CFLAGS) $+ -Llib -lsmm -o $@

bin/smmd: obj/smmd.o obj/utility.o obj/smmMem.o 
	$(CC) $(CFLAGS) $+ -o $@

.PHONY: install

install:
	@$(INSTALL) -m755 $(BINARIES) $(SMM_DIR)
	@$(INSTALL) -m644 $(LIB) $(SMM_DIR)
.PHONY: clean

clean:
	rm -fr $(OBJECTS)
	rm -fr $(BINARIES)
	rm -fr $(LIB)
	rm -fr *~ 
	rm -fr core

.PHONY: uninstall
uninstall:
	rm -fr $(SMM_DIR)/smmMon	
	rm -fr $(SMM_DIR)/smmd
	rm -fr $(SMM_DIR)/libsmm.a
