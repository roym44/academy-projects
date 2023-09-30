""" Comparing SymNMF to K-means """
import sys
import numpy as np
from sklearn.metrics import silhouette_score
from sklearn.metrics import pairwise_distances
import symnmf_capi

from symnmf import matrix_to_c, initialize_h
from kmeans import Cluster, DataPoint

# Constants 
EPSILON = 0.0001
MAX_ITER = 300
ARGS = {
    "SELF": 0,
    "K": 1,
    "FILE_NAME": 2,
}
    
def get_symnmf_result(k, data_points):
    # Parse the input file
    X = np.loadtxt(file_name, delimiter=',')
    points, n, d = matrix_to_c(X)

    # Get SymNMF result
    wc, hc = initialize_h(points, n, k, d)
    final_h = symnmf_capi.symnmf(wc, hc, n, k)

    # Derive the clusters using final H
    clusters = [Cluster() for i in range(k)]
    for i, p in enumerate(data_points):
        j = np.array(final_h[i]).argmax() # get maximum association score index
        clusters[j].add_point(p)
        # update the cluster which the point belongs to (in kmeans it happens inside assign_to_closest)
        p.cluster = clusters[j]
    
    return clusters

def get_kmeans_result(k, data_points):
    # Initialize k new clusters, such that their centroids are the first k datapoints
    # (i.e. add each of these k points to their cluster).
    clusters = []
    for i in range(k):
        c = Cluster(data_points[i])
        data_points[i].cluster = c
        clusters.append(c)

    i = 0
    convergence = False # initiated outside the loop to ensure we execute it at least once
    while (not convergence) and (i < MAX_ITER):
        # assign every point to the closest cluster
        for point in data_points:
            point.assign_to_closest(clusters)

        # update centroids
        convergence = True # let's assume we finished
        for cluster in clusters:
            delta = cluster.update_centroid()
            if delta >= EPSILON:
                convergence = False
        i += 1

    return clusters


def get_labels(n, clusters):
    # Create a label array of length n where labels[i] = j, such that datapoint i in the original 2d datapoint array is in the j_th index of clusters.
    labels = np.zeros(n)
    for j in range(len(clusters)): # for every cluster (by index)
        for point in clusters[j].points: # for every point in each cluster
            labels[point.initial_index] = j
    return labels


# takes the initial points matrix (X), and the clusters and retruns the silhouette score 
def get_silhouette_score(n, clusters, X: np.ndarray):
    dist_matrix = pairwise_distances(X, metric='euclidean') # distance matrix
    labels = get_labels(n, clusters)
    return silhouette_score(dist_matrix, labels, metric='precomputed')

if __name__ == "__main__":
    # Read arguments
    if (len(sys.argv) < len(ARGS) or len(sys.argv) > len(ARGS)):
        print(f"An Error Has Occurred")
        sys.exit()
    k = int(sys.argv[ARGS["K"]])
    file_name = sys.argv[ARGS["FILE_NAME"]]

    # Read raw data from input file and convert to data points list 
    data_points = []
    initial_index = 0
    with open(file_name) as f:
        for line in f:
            coords = [float(x) for x in line.split(',')]
            data_points.append(DataPoint(coords, initial_index))
            initial_index += 1
    n = len(data_points)

    # Also read the points from the file to an nparray (X: initial matrix)
    X = np.loadtxt(file_name, delimiter=',')
    
    # Get results and compare
    symnmf_clusters = get_symnmf_result(k, data_points)
    score_symnmnf = get_silhouette_score(n, symnmf_clusters, X)
    print(f"nmf: {score_symnmnf:.4f}")

    kmeans_clusters = get_kmeans_result(k, data_points)
    score_k_means = get_silhouette_score(n, kmeans_clusters, X)
    print(f"kmeans: {score_k_means:.4f}")
    