home = $(shell pwd)
os = $(shell uname)


ifeq ($(os), Darwin)
	cc = clang
endif

ifeq ($(os), Linux)
		cc = gcc
endif


cflags = -g -W -fPIC

dirs = preprocessor

E = @echo

# includepath = $(foreach dir, $(dirs), -I $(dir) )
includedir = $(home)/include/
sourcedir = $(home)/src/



libopenfd:
	$(cc) $(cflags) -c -I./include/ -I$(home)/src/ -I$(home)/src/3rdparty/qdbmp/ -I /usr/include/python3.6/ $(home)/src/*.c  -L/usr/lib/ -L/usr/lib/x86_64-linux-gnu/ -lpython3.6m
	ar -cr libopenfd.a *.o
	$(cc) -shared *.o -o libopenfd.so
	rm -rf *.o

clean:
	rm -rf libopenfd.a
	rm -rf libopenfd.so

tests:
	@echo "building for $(os)"
	$(cc) $(cflags) -I $(includedir) $(sourcedir)/preprocessor/downsample.c $(sourcedir)/3rdparty/qdbmp.c test.c -o bin/test
