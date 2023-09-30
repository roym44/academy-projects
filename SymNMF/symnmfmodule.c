/* Python C API: The C extension which serves python. */
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"


/* FUNCTIONS */
int retrieve_points(PyObject* points, int n, int d, PMATRIX* pmatrix); /* from python list of points (list of list of coords) to C matrix */
PyObject* build_points(PMATRIX matrix); /* converts the C matrix to a pythonic list of points */

static PyObject* sym_wrapper(PyObject* self, PyObject* args)
{
    int status = -1;
    PyObject* value = NULL;
    PyObject* points = NULL; /* This will later be converted to a matrix */
    int n, d = 0;
    PMATRIX initial = NULL;
    PMATRIX sim = NULL;

    /* Python -> C */
    /* Parse the Python arguments into the appropriate data types */
    if (!PyArg_ParseTuple(args, "Oii", &points, &n, &d)) 
    {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    /* Retrieve points: from python to c matrix of type PMATRIX */
    status = retrieve_points(points, n, d, &initial);
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* sym phase: getting the similarity matrix A from initial matrix X */
    status = sym(initial, &sim);
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* C -> Python: This builds the answer back into a python object */
    value = build_points(sim);

lblCleanup:
    free_matrix(initial);
    free_matrix(sim);
    return value;
}

static PyObject* ddg_wrapper(PyObject* self, PyObject* args)
{
    int status = -1;
    PyObject* value = NULL;
    PyObject* points = NULL; /* This will later be converted to a matrix */
    int n, d = 0;
    PMATRIX initial = NULL;
    PMATRIX sim = NULL;
    PMATRIX diagonal = NULL;

    /* Python -> C */
    /* Parse the Python arguments into the appropriate data types */
    if (!PyArg_ParseTuple(args, "Oii", &points, &n, &d)) 
    {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    /* Retrieve points: from python to c matrix of type PMATRIX */
    status = retrieve_points(points, n, d, &initial);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* sym phase: getting the similarity matrix A from initial matrix X */
    status = sym(initial, &sim);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* ddg phase: getting the diagonal matrix D from sim matrix A */
    status = ddg(sim, &diagonal);
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* C -> Python: This builds the answer back into a python object */
    value = build_points(diagonal);

    
lblCleanup:
    free_matrix(initial);
    free_matrix(sim);
    free_matrix(diagonal);
    return value;
}

static PyObject* norm_wrapper(PyObject* self, PyObject* args)
{
    int status = -1;
    PyObject* value = NULL;
    PyObject* points = NULL; /* This will later be converted to a matrix */
    int n, d = 0;
    PMATRIX initial = NULL;
    PMATRIX sim = NULL;
    PMATRIX diagonal = NULL;
    PMATRIX normalized = NULL;

    /* Python -> C */
    /* Parse the Python arguments into the appropriate data types */
    if (!PyArg_ParseTuple(args, "Oii", &points, &n, &d)) 
    {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    /* Retrieve points: from python to c matrix of type PMATRIX */
    status = retrieve_points(points, n, d, &initial);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* sym phase: getting the similarity matrix A from initial matrix X */
    status = sym(initial, &sim);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* ddg phase: getting the diagonal matrix D from sim matrix A */
    status = ddg(sim, &diagonal);
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* norm phase: getting W from D and A */
    status = norm(sim, diagonal, &normalized);
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* C -> Python: This builds the answer back into a python object */
    value = build_points(normalized);

    
lblCleanup:
    free_matrix(initial);
    free_matrix(sim);
    free_matrix(diagonal);
    free_matrix(normalized);
    return value;
}


static PyObject* symnmf_wrapper(PyObject* self, PyObject* args)
{
    int status = -1;
    PyObject* value = NULL;
    PyObject* w_points = NULL; 
    PyObject* h_points = NULL; 
    int n, k = 0;
    PMATRIX normalized = NULL;
    PMATRIX initial_h = NULL;
    PMATRIX updated_h = NULL;

    /* Python -> C */
    /* Parse the Python arguments into the appropriate data types */
    if (!PyArg_ParseTuple(args, "OOii", &w_points, &h_points, &n, &k)) 
    {
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }

    /* Retrieve points: from python to c matrix of type PMATRIX */
    status = retrieve_points(w_points, n, n, &normalized);
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    status = retrieve_points(h_points, n, k, &initial_h);
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    status = symnmf(initial_h, normalized, &updated_h);
    initial_h = NULL;
    if (status == 1)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }
    

    /* C -> Python: This builds the answer back into a python object */
    value = build_points(updated_h);

lblCleanup:
    free_matrix(normalized);
    free_matrix(initial_h);
    free_matrix(updated_h);
    return value;
}

static PyMethodDef symnmfMethods[] = {
    {"sym", (PyCFunction)sym_wrapper, METH_VARARGS, PyDoc_STR("sym: constructing the similarity matrix")}, /* sym() */
    {"ddg", (PyCFunction)ddg_wrapper, METH_VARARGS, PyDoc_STR("ddg: constructing the diagonal degree matrix")}, /* ddg() */
    {"norm", (PyCFunction)norm_wrapper, METH_VARARGS, PyDoc_STR("norm: constructing the normalized matrix")}, /* norm() */
    {"symnmf", (PyCFunction)symnmf_wrapper, METH_VARARGS, PyDoc_STR("symnmf: getting the final H")}, /* symnmf() */
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef symnmfmodule =
{
    PyModuleDef_HEAD_INIT, 
    "symnmf_capi", /* name of module */
    NULL, /* module documentation */
    -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    symnmfMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_symnmf_capi(void)
{
    PyObject *m;
    m = PyModule_Create(&symnmfmodule);
    if (!m)
        return NULL;
    return m;
}

int retrieve_points(PyObject* points, int n, int d, PMATRIX* pmatrix)
{
    int status = -1;
    PMATRIX matrix = NULL;
    PyObject* point = NULL;
    PyObject* coord_object = NULL;
    double coord = 0;
    int i, j = 0;

    /* create a new empty initial matrix */
    status = create_matrix(n, d, &matrix);
    if (status != 0)
    {
        printf("An Error Has Occurred\n");
        goto lblCleanup;
    }

    /* fill the matrix with coords */
    for (i = 0; i < n; i++) 
    {
        point = PyList_GetItem(points, i);
        for (j = 0; j < d; j++) 
        {
            coord_object = PyList_GetItem(point, j);
            coord = PyFloat_AsDouble(coord_object);
            matrix->coords[i][j] = coord;
        }
    }

    /* Transfer ownership */
    *pmatrix = matrix;
    matrix = NULL;

    status = 0;

lblCleanup:
    free_matrix(matrix);
    return status;
}

PyObject* build_points(PMATRIX matrix)
{
    PyObject* python_points = NULL;
    PyObject* python_point = NULL;
    PyObject* python_coord = NULL;
    double c_coord = 0;
    int i, j = 0;

    python_points = PyList_New(matrix->rows); /* Creating a python list of length rows */
    for (i = 0; i < matrix->rows; i++)
    {
        /* Building single python point with d coords */
        python_point = PyList_New(matrix->cols); /* Creating a python list of length cols */
        for (j = 0; j < matrix->cols; j++)
        {
            c_coord = matrix->coords[i][j]; /* Retrieving value to put into python coords */
            python_coord = Py_BuildValue("d", c_coord); /* Converting it to a python double */
            PyList_SetItem(python_point, j, python_coord); /* Setting point[i] = coord */
        }
        PyList_SetItem(python_points, i, python_point); /* Setting points[i] = point */
    }
    return python_points;
}
