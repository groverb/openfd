home = $(shell pwd)
os = $(shell uname)


ifeq ($(os), Darwin)
	cc = clang
endif

ifeq ($(os), Linux)
		cc = gcc
endif


cflags = -g -W

dirs = preprocessor

E = @echo

# includepath = $(foreach dir, $(dirs), -I $(dir) )
includedir = $(home)/include/
sourcedir = $(home)/src/



libopenfd:
	@echo "target unimplemented"

tests:
	@echo "building for $(os)"
	$(cc) $(cflags) -I $(includedir) $(sourcedir)/preprocessor/downsample.c $(sourcedir)/3rdparty/qdbmp.c test.c -o bin/test
		
