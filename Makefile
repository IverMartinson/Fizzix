COMPILER=gcc
FLAGS_ALL=-g -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter
FLAGS_EXAMPLE=-Lbuilds/ -lfizzix -Wl,-rpath=builds/ -lm
FLAGS_LIB=-fPIC -shared -lc -lm -lSDL2

main.bin: libfizzix.so # LIBNAMEHERE.so
	$(COMPILER) $(FLAGS_ALL) src/launch\ program/main.c -o builds/main.bin $(FLAGS_EXAMPLE) 

libfizzix.so:
	$(COMPILER) $(FLAGS_ALL) src/main/main.c -o builds/libfizzix.so $(FLAGS_LIB) 

# LIBNAMEHERE.so:
# 	cp src/libraries/libLIBNAMEHERE.so builds/

clean:
	rm builds/*
