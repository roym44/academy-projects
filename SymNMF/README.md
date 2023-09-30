# Introduction
Final project for SP course - implementation of the SymNMF algorithm.
We implement a clustering algorithm that is based on symmetric Non-negative Matrix Factorization (symNMF).
We further compare it to K-means algorithm.

## SymNMF 
Given a set of $N$ points $X = x_1, x_2, ...,x_N \in R^d$ the algorithm is:
1. Form the similarity matrix $A$ from $X$
2. Compute the Diagonal Degree matrix $D$
3. Compute the normalized similarity matrix $W$
4. Find $H_{n \times k}$ that solves: $\min\limits_{H \geq 0}||W-HH^T||^2_F $

Where $k$ is a parameter denoting the required number of clusters, and $||\space||_F^2$ is the squared [Frobenius norm](https://en.wikipedia.org/wiki/Matrix_norm#Frobenius_norm).

## K-means
Given a set of $N$ datapoints $X = x_1, x_2, ...,x_N \in R^d$, the goal is to group the data into $K$ clusters, each datapoint is assigned to exactly one cluster and the number of clusters $K$ is such that $1< K< N$. Each cluster $k$ is represented by it’s centroid which is the mean $\mu_k \in R^d$ of the
cluster’s members. The algorithm is:
1. Initialize centroids as first $k$ datapoints: $\mu_k=x_k, \forall k\in K$
2. **repeat**
3. Assign every $x_i$ to the closest cluster $k$: $arg \min\limits_{k}d(x_i,\mu_k), \forall k. 1\leq k \leq K$
4. Update the centroids: $\mu_k = \frac{1}{|k|} \sum_{x_i \in k}x_i$
5. **until** convergence: ($\Delta\mu_k<\epsilon$) *OR* (*iteration_number = iter*)

Where $d(p,q)$ is the Euclidean Distance,
and $\Delta\mu_k$ is the Euclidean Distance between the updated centroid to the previous one (this should be checked for every centroid).

# Usage
## Running python (using Python/C API)
```
make build-python
python3 symnmf.py 2 symnmf input.txt
(symnmf, "k", "goal", "input file")
```
## Running C directly
```
make build-c
./symnmf sym input.txt
(symnmf, "goal", "input file")
```
### Running the analysis
```
python3 analysis.py input_.txt
```
### leak checking
```
valgrind --leak-check=full ./symnmf sym ./input.txt
```
