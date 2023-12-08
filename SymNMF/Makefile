# Make script for building and running symnmf
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors

build-python:
	python3 setup.py build_ext --inplace

run-c: build-c
	./symnmf

build-c: symnmf.o symnmf.h
	gcc -o symnmf symnmf.o -lm

symnmf.o: symnmf.c
	gcc -c symnmf.c $(CFLAGS)

clean:
	rm -rf *.o build symnmf_capi* symnmf