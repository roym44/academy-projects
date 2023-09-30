""" Python Program: The Python interface of our code.
Contains all of the cmd argument interface, reading the data, 
H initialization, using our C extension and outputting the results. """
import sys
import math
import numpy as np
import symnmf_capi

# Constants
ARGS = {
    "SELF": 0,
    "K": 1,
    "GOAL": 2,
    "FILE_NAME": 3,
}

def print_result(result):
    """
    Prints the result matrix in the appropriate way
    """
    for line in result:
        format_coords = [f"{coord:.4f}" for coord in line]
        print(",".join(format_coords))

def matrix_to_c(matrix):
    """
    Converts the np array matrix to a list of lists for C interface
    """
    n = matrix.shape[0]
    # We consider the edge case where the points are one-dimensional (d=1)
    d = matrix.shape[1] if len(matrix.shape) != 1 else 1
    points = matrix.tolist()
    if d == 1:
        points = [[p] for p in points]
    return points, n, d

def initialize_h(points, n, k, d):
    """
    Initializes H and returns it together with W matrix (as list of lists for C interface)
    """
    np.random.seed(0)
    W = symnmf_capi.norm(points, n, d)
    wnp = np.array(W)
    m = wnp.mean()
    H = np.random.uniform(0, 2 * math.sqrt(m / k), (n, k))
    wc, n, n = matrix_to_c(wnp)
    hc, n, k = matrix_to_c(H)
    return wc, hc

if __name__ == "__main__":
    # Read arguments
    if (len(sys.argv) < len(ARGS) or len(sys.argv) > len(ARGS)):
        print(f"An Error Has Occurred")
        sys.exit()
    k = int(sys.argv[ARGS["K"]])
    goal = sys.argv[ARGS["GOAL"]]
    file_name = sys.argv[ARGS["FILE_NAME"]]

    # Parse the input file
    X = np.loadtxt(file_name, delimiter=',')
    points, n, d = matrix_to_c(X)

    # Interface with C extension
    if goal == "sym":
        result = symnmf_capi.sym(points, n, d)
    if goal == "ddg":
        result = symnmf_capi.ddg(points, n, d)
    if goal == "norm":
        result = symnmf_capi.norm(points, n, d)
    if goal == "symnmf":
        wc, hc = initialize_h(points, n, k, d)
        result = symnmf_capi.symnmf(wc, hc, n, k)

    # Output result matrix
    print_result(result)
