# Make script for building symnmf executable
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors

symnmf: symnmf.o symnmf.h
	gcc -o symnmf symnmf.o -lm

symnmf.o: symnmf.c
	gcc -c symnmf.c $(CFLAGS)