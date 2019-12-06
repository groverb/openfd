HOME = $(shell pwd)


CC = gcc
CFLAGS = -std=c11 -Wall -fPIC
 
SRC=dknet_bridge.c eval.c fd_core.c fdimage.c fdlist.c py_bridge.c slidingwin.c
OBJ=dknet_bridge.o eval.o fd_core.o fdimage.o fdlist.o py_bridge.o slidingwin.o
SLIB = libopenfd.so
ALIB = libopenfd.a

SRCDIR = $(HOME)/src/
INCLUDEDIR = $(HOME)/include/
3RDPARTYDIR=$(HOME)/src/3rdparty

AR=ar rcs
INCLUDEFLAGS= -I$(INCLUDEDIR) -I$(SRCDIR) -I$(3RDPARTYDIR)/darknet/include/  -I$(3RDPARTYDIR)/qdbmp -I/usr/include/python3.6/ -I/Library/Frameworks/Python.framework/Versions/3.6/include/python3.6m/
LDSEARCHFLAGS = -L$(HOME)/ -L$(3RDPARTYDIR)/darknet/ -L$(3RDPARTYDIR)/qdbmp/ -L/Library/Frameworks/Python.framework/Versions/3.6/lib/  
LDFLAGS= -lqdbmp -ldarknet -lpython3.6m
LDRPATHFLAGS = -Wl,-rpath -Wl,$(HOME) -Wl,-rpath -Wl,$(3RDPARTYDIR)/darknet  -Wl,-rpath -Wl,$(3RDPARTYDIR)/qdbmp/


_OBJ = $(addprefix $(SRCDIR), $(OBJ))
_SRC = $(addprefix $(SRCDIR), $(SRC))

darknet: 
	@cd $(3RDPARTYDIR)/darknet/ && $(MAKE) 

qdbmp: 
	@cd $(3RDPARTYDIR)/qdbmp/ && $(MAKE) 

THIRD_PARTY = darknet qdbmp
THIRD_PARTY_LIBS= libdarknet.so libqdbmp.so libpython3.6m.so


%.o: %.c
	@echo "CC	$@"
	@$(CC) $(CFLAGS) -c $(INCLUDEFLAGS) $< -o $@ 

$(SLIB): darknet qdbmp $(_OBJ) 
	@echo "LINK	$(SLIB) $(THIRD_PARTY_LIBS)" 
	@$(CC) -shared -fPIC -o $@ $(_OBJ) $(LDRPATHFLAGS) $(LDSEARCHFLAGS) $(LDFLAGS)

$(ALIB): $(_OBJ)
	@echo "AR	$@"
	@$(AR) $@ $^


all: $(SLIB) $(ALIB)

clean_3rdparty:	
	@cd $(3RDPARTYDIR)/qdbmp/ && $(MAKE) clean 
	@cd $(3RDPARTYDIR)/darknet/ && $(MAKE) clean 

clean: clean_3rdparty 
	rm -rf $(_OBJ) 
	rm -rf $(SLIB)
	rm -rf $(ALIB)


.DEFAULT_GOAL := all

