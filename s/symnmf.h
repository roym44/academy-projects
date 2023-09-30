/* C Header file: defines all the functions prototypes that
is being used in symnmfmodule.c and implemented at symnmf.c */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* MACROS */
#define BETA (0.5)
#define EPSILON (0.0001)
#define MAX_ITER (300)

/* Allocates a zero-ed buffer of n elements from pointer p on the heap, casts the return value to the pointer's type */
#define HEAPALLOCZ(p, n) calloc((n), sizeof(*p))

/* Safely frees a buffer allocated on the heap */
#define HEAPFREE(p)					\
{									\
	if (NULL != (p))			    \
	{								\
		(void)free(p);	            \
		(p) = NULL;					\
	}								\
}

/* TYPEDEFS */
typedef struct _MATRIX
{
    double** coords;
	int rows;
	int cols;
} MATRIX;
typedef MATRIX* PMATRIX;

typedef enum _ARGS
{
	ARGS_SELF = 0,
	ARGS_GOAL,
    ARGS_FILE_NAME,

	/* Must be last */ 
	ARGS_COUNT
} ARGS;

/* MATH HELPER FUNCTIONS */
double find_sq_euc_dist(double* point1, double* point2, int d); /* finds squared euclidian distance between two points */
double find_exp(double* point1, double* point2, int d); /* finds the exp function as described in algorithm: e^( - sq_euc_dist / 2) */
int mat_mult(PMATRIX A, PMATRIX B, PMATRIX* pres); /* matrix multiplication function - Receives A: n*k, B: k*m. Returns A*B: n*m */
int subtract_matrices(PMATRIX A, PMATRIX B, PMATRIX* pres); /* Assumes that matrices are of same dimensions */
void pow_diag(PMATRIX M, double a); /* receives a diagonal matrix M and changes it in-place to be M^a */
double calculate_cell(double numerator, double denominator, double H_ij, double beta);
int squared_frob_norm(PMATRIX A, PMATRIX B, double* result); /* calculates squared frobenius norm */

/* SYMNMF FUNCTIONS */
int sym(PMATRIX initial, PMATRIX* psim); /* X -> A */
int ddg(PMATRIX sim, PMATRIX* pdiagonal); /* A -> D */
int norm(PMATRIX sim, PMATRIX diagonal, PMATRIX* pnormalized); /* D -> W */
int symnmf(PMATRIX initial_h, PMATRIX normalized, PMATRIX* pupdated_h); /* H_0,W -> H_final */

/* MATRIX FUNCTIONS */
int create_matrix(int rows, int cols, PMATRIX* pmatrix); /* creates a new empty zero-ed matrix with dimensions rows X cols */
int transpose_matrix(PMATRIX matrix, PMATRIX* ptransposed); /* gets a matrix and returns its transpose */
void free_matrix(PMATRIX matrix); /* frees the memory for a matrix */
void print_matrix(PMATRIX matrix); /* prints a matrix */
int parse_file(char* file_name, int* n, int* d);
int read_initial_from_file(char* file_name, PMATRIX matrix);
int perform_iteration(PMATRIX prev, PMATRIX normalized, PMATRIX* pnew, double beta); /* receives H(i) (prev), W (norm) and beta, returns H(i+1) (new). H: nXk */
