/* C Program: the C interface of our code, containing the 
implementation of the symnmf algorithm's different steps. */
#include "symnmf.h"

double find_sq_euc_dist(double* point1, double* point2, int d) /* finds squared euclidian distance between two points of dimension d */
{
    /* Iterates over coordinates of point1 and point 2, calculates the square of their difference and adds to sum*/
    double total = 0;
    double squared_diff = 0;
    int i = 0;

    for (i = 0; i < d; i++)
    {
        squared_diff = pow(point1[i] - point2[i], 2);
        total += squared_diff;
    }

    return total;
}

double find_exp(double* point1, double* point2, int d) /* finds the exp function as described in algorithm: e^( - sq_euc_dist / 2) */
{
    double sq_euc_dist = 0;
    double res = 0;
    
    sq_euc_dist = find_sq_euc_dist(point1, point2, d);
    res = exp((-0.5) * sq_euc_dist);

    return res;
}

int mat_mult(PMATRIX A, PMATRIX B, PMATRIX* pres) /* */
{
    int status = -1;
    int n, k, m = 0;
    int i, j, x = 0;
    PMATRIX res = NULL;

    n = A->rows;
    k = A->cols;
    m = B->cols; 

    /* Allocate a zero-ed matrix nXm */
    status = create_matrix(n, m, &res);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }
    
    /* Fill the values */
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            for (x = 0; x < k; x++)
                res->coords[i][j] += A->coords[i][x] * B->coords[x][j];  /* Perform the matrix multiplication */ 

    /* Transfer ownership */
    *pres = res;
    res = NULL;

    status = 0;

lblCleanup:
    free_matrix(res);
    return status;
}

int subtract_matrices(PMATRIX A, PMATRIX B, PMATRIX* pres)
{
    int status = -1;
    int i, j = 0;
    PMATRIX res = NULL;
    
    /* Allocate a zero-ed matrix nXm */
    status = create_matrix(A->rows, A->cols, &res);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }
    
    /* Fill the values */
    for (i = 0; i < A->rows; i++)
        for (j = 0; j < A->cols; j++)
            res->coords[i][j] = A->coords[i][j] - B->coords[i][j];  /* Perform the matrix subtraction */ 

    /* Transfer ownership */
    *pres = res;
    res = NULL;

    status = 0;

lblCleanup:
    free_matrix(res);
    return status;
}

void pow_diag(PMATRIX M, double a)
{
    int n = 0;
    int i = 0;

    n = M->rows;

    for (i = 0; i < n; i++)
        M->coords[i][i] = pow(M->coords[i][i], a);
}

double calculate_cell(double numerator, double denominator, double H_ij, double beta)
{
    double result = 0;
    result = H_ij * (1 - beta + ((beta)*(numerator/denominator)));
    return result;
}

int squared_frob_norm(PMATRIX A, PMATRIX B, double* presult)
{
    int status = -1;
    int i, j = 0;
    double result = 0;
    PMATRIX sub = NULL;
    
    /* Allocate a zero-ed matrix nXm */
    status = subtract_matrices(A, B, &sub);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }
    
    /* Calculate the final result */
    for (i = 0; i < sub->rows; i++)
        for (j = 0; j < sub->cols; j++)
            result += pow(sub->coords[i][j], 2);

    /* Transfer result */
    *presult = result;

    status = 0;

lblCleanup:
    free_matrix(sub);
    return status;
}

int sym(PMATRIX initial, PMATRIX* psim)
{
    int status = -1;
    int i, j = 0;
    int n, d = 0;
    PMATRIX sim = NULL;

    n = initial->rows;
    d = initial->cols;

    /* Allocate a zero-ed matrix nXn */
    status = create_matrix(n, n, &sim);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }
    
    /* Fill the values */
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i == j)
                sim->coords[i][j] = 0;
            else
                sim->coords[i][j] = find_exp(initial->coords[i], initial->coords[j], d); 
        }
    }

    /* Transfer ownership */
    *psim = sim;
    sim = NULL;

    status = 0;

lblCleanup:
    free_matrix(sim);
    return status;
}

int ddg(PMATRIX sim, PMATRIX* pdiagonal)/* A -> D */
{
    int status = -1;
    int i, j = 0;
    int n = 0;
    double sum = 0;
    PMATRIX diagonal = NULL;

    n = sim->rows;

    /* Allocate a zero-ed matrix nXn */
    status = create_matrix(n, n, &diagonal); 
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }
    
    /* Fill the values of diagonal by going over rows of sim */
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            sum += sim->coords[i][j];
        }
        diagonal->coords[i][i] = sum; 
        sum = 0; /* resetting sum for next iteration */  
    }

    /* Transfer ownership */
    *pdiagonal = diagonal;
    diagonal = NULL;

    status = 0;

lblCleanup:
    free_matrix(diagonal);
    return status;
}

int norm(PMATRIX sim, PMATRIX diagonal, PMATRIX* pnormalized)
{
    int status = -1;
    PMATRIX temp = NULL; /* Will store result of D^(-1/2) * A */
    PMATRIX res = NULL; /* Will store end result */

     /* Computing D^(-0.5) */
    (void)pow_diag(diagonal, -0.5); 

    /* Computing D^(-0.5)*A and storing it in temp */
    status = mat_mult(diagonal, sim, &temp);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Computing W */
    status = mat_mult(temp, diagonal, &res);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Transfer ownership */
    *pnormalized = res;
    res = NULL;

    status = 0;

lblCleanup:
    free_matrix(temp);
    free_matrix(res);
    return status;
}

int symnmf(PMATRIX initial_h, PMATRIX normalized, PMATRIX* pupdated_h)
{
    int status = -1;
    int i = 0;
    int convergence = 0; /* initialized to False */
    double delta = 0;
    PMATRIX prev_h = NULL; 
    PMATRIX updated_h = NULL; 

    /* update H until convergence */
    prev_h = initial_h;
    initial_h = NULL; /* will be freed by freeing prev_h in the first iteration */
    i = 0;
    
    while (!convergence && i < MAX_ITER)
    {
        /* perform an update */
        status = perform_iteration(prev_h, normalized, &updated_h, BETA);
        if (status == 1)
        {
            printf("An Error Has Occurred\n");
            goto lblCleanup;
        }

        /* check convergence */
        status = squared_frob_norm(updated_h, prev_h, &delta);
        if (status == 1)
        {
            printf("An Error Has Occurred\n");
            goto lblCleanup;
        }

        if (delta < EPSILON)
            convergence = 1; /* True */

        /* free our current prev - and update it to be the current result (updated_h) */
        free_matrix(prev_h);
        prev_h = updated_h;
        i++;
    }

    /* Transfer ownership */
    *pupdated_h = updated_h;
    updated_h = NULL;

    status = 0;

lblCleanup:
    free_matrix(updated_h);
    return status;
}

int create_matrix(int rows, int cols, PMATRIX* pmatrix)
{
    int status = -1;
    double** coords = NULL;
    PMATRIX matrix = NULL;
    int i = 0;

    /* allocate the matrix */
    matrix = (PMATRIX)HEAPALLOCZ(matrix, 1);
    if (matrix == NULL)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* allocate the coords */
    coords = (double**)HEAPALLOCZ(coords, rows);
    if (coords == NULL)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }
    
    for (i = 0; i < rows; i++)
    {
        coords[i] = (double*)HEAPALLOCZ(coords[i], cols);
        if (coords[i] == NULL)
        {
            printf("An Error Has Occurred\n");
            status = 1;
            goto lblCleanup;
        }
    }
    
    matrix->coords = coords;
    matrix->rows = rows;
    matrix->cols = cols;

    /* Transfer ownership */
    *pmatrix = matrix;
    matrix = NULL;

    status = 0;

lblCleanup:
    free_matrix(matrix);
    return status;
}

int transpose_matrix(PMATRIX matrix, PMATRIX* ptransposed)
{
    int status = -1;
    int i, j = 0;
    PMATRIX transposed = NULL;

    /* Allocate a zero-ed matrix nXk */
    status = create_matrix(matrix->cols, matrix->rows, &transposed);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Fill the values */
    for (i = 0; i < transposed->rows; i++)
        for (j = 0; j < transposed->cols; j++)
            transposed->coords[i][j] = matrix->coords[j][i];

    /* Transfer ownership */
    *ptransposed = transposed;
    transposed = NULL;

    status = 0;

lblCleanup:
    free_matrix(transposed);
    return status;
}

void free_matrix(PMATRIX matrix)
{
    int i = 0;
    if (matrix != NULL)
    {
        /* free all the one-dimensional arrays */
        if (matrix->coords != NULL)
            for (i = 0; i < matrix->rows; i++)
                HEAPFREE(matrix->coords[i]);

        /* free the two-dimensional pointer */
        HEAPFREE(matrix->coords);
        HEAPFREE(matrix);
    }
}

void print_matrix(PMATRIX matrix)
{
    int i, j = 0;
    for (i = 0; i < matrix->rows; i++)
    {
        for (j = 0; j < matrix->cols - 1; j++)
            printf("%.4f,", matrix->coords[i][j]);
        printf("%.4f\n", matrix->coords[i][j]);
    }
}

int parse_file(char* file_name, int* n, int* d)
{
    int status = -1;
    int rows = 0;
    int cols = 0;
    FILE* fp = NULL;
    char* line = NULL;
    char* token = NULL;
    size_t len = 0;
    ssize_t read = 0;

    fp = fopen(file_name, "r");
    if (fp == NULL) 
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    while ((read = getline(&line, &len, fp)) != -1) 
    {
        /* Increase the number of rows */
        rows++;

        if (rows > 1)
            continue;
        
        /* Calculate the number of columns - only first row */ 
        token = strtok(line, ",");
        while (token != NULL) {
            token = strtok(NULL, ",");
            cols++;
        }
    }

    /* Transfer results */
    *n = rows;
    *d = cols;

    status = 0;

lblCleanup:
    HEAPFREE(line);
    fclose(fp);
    return status;
}

int read_initial_from_file(char* file_name, PMATRIX matrix)
{
    int status = -1;
    FILE* fp = NULL;
    char* line = NULL;
    char* token = NULL;
    size_t len = 0;
    ssize_t read = 0;
    int i, j = 0;
    double value = 0;

    fp = fopen(file_name, "r");
    if (fp == NULL) 
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Fill the matrix with the data from the file */
    for (i = 0; i < matrix->rows; i++)
    {
        read = getline(&line, &len, fp);
        if (read == -1)
        {
            printf("An Error Has Occurred\n");
            status = 1;
            goto lblCleanup;
        }
        
        token = strtok(line, ",");
        for (j = 0; j < matrix->cols; j++)
        {
            value = atof(token);
            matrix->coords[i][j] = value;
            token = strtok(NULL, ",");
        }
    }

    status = 0;

lblCleanup:
    HEAPFREE(line);
    fclose(fp);
    return status;
}

int perform_iteration(PMATRIX prev, PMATRIX normalized, PMATRIX* pnew, double beta)
{
    int status = -1;
    int i, j = 0;
    int n, k = 0;
    PMATRIX new = NULL;
    PMATRIX numerator_mat = NULL; /* This is WH */
    PMATRIX temp = NULL; /* This is H^T * H */
    PMATRIX denominator_mat = NULL; /* This is H*H^T*H*/
    PMATRIX prev_transposed = NULL; /* This is H^T */

    n = prev->rows; /* Note that the dimensions of prev and new are the same */
    k = prev->cols;

    /* Allocate a zero-ed matrix nXk */
    status = create_matrix(n, k, &new);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Computing numerator matrix */
    status = mat_mult(normalized, prev, &numerator_mat);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Create prev transposed */
    status = transpose_matrix(prev, &prev_transposed);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Computing temp */
    status = mat_mult(prev_transposed, prev, &temp);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }
    
    /* Computing denominator matrix */
    status = mat_mult(prev, temp, &denominator_mat);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Fill the values */
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < k; j++)
        {
            new->coords[i][j] = calculate_cell(
                numerator_mat->coords[i][j], 
                denominator_mat->coords[i][j], 
                prev->coords[i][j], 
                beta);
        }
    }

    /* Transfer ownership */
    *pnew = new;
    new = NULL;

    status = 0;

lblCleanup:
    free_matrix(new);
    free_matrix(numerator_mat);
    free_matrix(prev_transposed);
    free_matrix(temp);
    free_matrix(denominator_mat);
    return status;
}


int main(int argc, char *argv[])
{
    int status = -1;
    char* goal = NULL;
    char* file_name = NULL;
    int n, d = 0;
    PMATRIX initial = NULL;
    PMATRIX sim = NULL;
    PMATRIX diagonal = NULL;
    PMATRIX normalized = NULL;
    PMATRIX result = NULL;
    
    /* Validate arguments */
    if (argc < ARGS_COUNT || argc > ARGS_COUNT)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    goal = argv[ARGS_GOAL];
    file_name = argv[ARGS_FILE_NAME];

    /* Deduce n and d, by reading the file */
    status = parse_file(file_name, &n, &d);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }
    
    /* Allocate a zero-ed matrix nXd */
    status = create_matrix(n, d, &initial);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Read the initial matrix from the file */
    status = read_initial_from_file(file_name, initial);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        status = 1;
        goto lblCleanup;
    }

    /* Perform logic according to goal */
    status = sym(initial, &sim);
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }
    
    if (strcmp(goal, "sym") == 0)
        result = sim;
    else
    {
        status = ddg(sim, &diagonal);
        if (status == 1)
        {
            printf("An Error Has Occurred\n");
            goto lblCleanup;
        }
        if (strcmp(goal, "ddg") == 0)
            result = diagonal;
        else
        {
            status = norm(sim, diagonal, &normalized);
            if (status == 1)
            {
                printf("An Error Has Occurred\n");
                goto lblCleanup;
            }
            if (strcmp(goal, "norm") == 0)
                result = normalized;
        }
    }

    /* Goal wasn't one of the following: {sym, ddg, norm} */
    if (result == NULL)
        printf("An Error Has Occurred\n");
    /* Output the matrix */
    else
        (void)print_matrix(result);

    /* Success */
    status = 0;

lblCleanup:
    free_matrix(initial);
    free_matrix(sim);
    free_matrix(diagonal);
    free_matrix(normalized);
    return status;
}
