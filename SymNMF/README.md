# SymNMF
Final project for SP course - implementation of the SymNMF algorithm.
We implement a clustering algorithm that is based on symmetric Non-negative Matrix Factorization (symNMF).
We further apply it to several datasets and compare to Kmeans algorithm.

## MISC
VSCode extension to debug Python+C:
https://marketplace.visualstudio.com/items?itemName=benjamin-simmonds.pythoncpp-debug

Cloning the project:
git clone git@github.com:roymayan/SymNMF.git

# building the extension
python3 setup.py build_ext --inplace

# running the python program
python3 symnmf.py 2 symnmf input_1.txt
(symnmf, "k", "goal", "input file")

# compiling the C program using Makefile
make

# compiling and running C (no debugging)
gcc -ansi -Wall -Wextra -Werror -pedantic-errors symnmf.c -lm -o symnmf
./symnmf sym input_1.txt
(symnmf, "goal", "input file")

# running the analysis program
python3 analysis.py input_k5_d7.txt

2GqIvGp4

## Leak checking
First we copile the program:
```
gcc -g ./symnmf.c -lm -o symnmf
```
Then we run valgrind:
```
valgrind --leak-check=full ./symnmf sym ./input_3.txt
```

## Zipping
```
zip -r 322315250_206483554_project.zip 322315250_206483554_project
```

