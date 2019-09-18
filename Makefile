home = `pwd`

cc = gcc
cflags = -g

dirs = preprocessor

# includepath = $(foreach dir, $(dirs), -I $(dir) )
includedir = $(home)/include/
sourcedir = $(home)/src/



libopenfd:
	@echo "target unimplemented"

tests:
	$(cc) $(cflags) -I $(includedir) $(sourcedir)/preprocessor/downsample.c $(sourcedir)/3rdparty/qdbmp.c test.c -o bin/test

		
