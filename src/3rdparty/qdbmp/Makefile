CC = gcc
CFLAGS = -std=c11 -Wall -fPIC

SRC = qdbmp.c
OBJ = qdbmp.o
DEP = qdbmp.h

SLIB = libqdbmp.so
ALIB = libqdbmp.a

AR=ar rcs

%.o: %.c
	@echo "CC	$@"
	@$(CC) $(CFLAGS) -c $(INCLUDEFLAGS) -o $@ $<

$(SLIB): $(OBJ)
	@echo "LINK	$@"
	@$(CC) -shared -fPIC -o $@ $^ 

$(ALIB): $(OBJ)
	@echo "AR	$@"
	@$(AR) $@ $^


all: $(SLIB) $(ALIB)

clean: 
	rm -rf *.o 
	rm -rf $(SLIB)
	rm -rf $(ALIB)


.DEFAULT_GOAL := all

