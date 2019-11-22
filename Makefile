home = $(shell pwd)
os = $(shell uname)


ifeq ($(os), Darwin)
	cc = clang
endif

ifeq ($(os), Linux)
		cc = gcc
endif


cflags = -g -Wall -fPIC

dirs = preprocessor

E = @echo

# includepath = $(foreach dir, $(dirs), -I $(dir) )
includedir = $(home)/include/
sourcedir = $(home)/src/
3rdparty_path := $(home)/src/3rdparty

%.c: 
	@echo "CC	" $<

libopenfd:
	@echo "building libopenfd"
	$(cc) $(cflags) -c -I./include/ -I$(home)/src/ -I$(home)/src/3rdparty/qdbmp/ -I$(home)/src/3rdparty/darknet/include/ -I /usr/include/python3.6/ $(home)/src/*.c  $(home)/src/3rdparty/qdbmp/qdbmp.c -L/usr/lib/ -L/usr/lib/x86_64-linux-gnu/ -lpython3.6m  
	ar -cr libopenfd.a *.o
	$(cc) -shared *.o -o libopenfd.so $(3rdparty_path)/darknet/libdarknet.a -lm -lpthread
	rm -rf *.o

_maint:
	$(cc) $(cflags) -Wl,-rpath -Wl,./ test.c src/3rdparty/qdbmp/qdbmp.c -I include/ -L./ -lopenfd -L/usr/lib/ -L/usr/lib/x86_64-linux-gnu/ -lpython3.6m -o maintest -ldarknet


maint:
	$(cc) $(cflags) -Wl,-rpath -Wl,./ detector.c test.c -I include/ -I$(home)/src/3rdparty/darknet/include/ -L./ -o openfd_test -ldarknet -lpthread

clean:
	rm -rf libopenfd.a
	rm -rf libopenfd.so

tests:
	@echo "building for $(os)"
	$(cc) $(cflags) -I $(includedir) $(sourcedir)/preprocessor/downsample.c $(sourcedir)/3rdparty/qdbmp.c test.c -o bin/test

newt:
	gcc -Wl,-rpath -Wl,./ newtest.c -o newtest -Iinclude/ -L./ -lopenfd  -L/usr/lib/ -L/usr/lib/x86_64-linux-gnu/ -lpython3.6m -lpthread -lm
	


# -Wl,-rpath -Wl,$(3rdparty_path)/darknet/ 
