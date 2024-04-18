# Introduction
An assignment given as part of the Intro to ML (fall 2024) course - visualizing the Hoeffding bound and studying the performance of the k-NN algorithm.


## Visualizing the Hoeffding bound
- We use **numpy** to generate an $N \times n$ matrix of samples from *Bernoulli(1/2)*, and calculate for each row *i* the empricial mean: $\bar X_i=\frac{1}{n} \sum_{j=1}^{n}X_{i,j}$ where $N=200000$ and $n=20$.
- We take 50 values of $\epsilon \in [0,1]$ and calculate the empirical probability that $[\bar X_i-\frac{1}{2}]>\epsilon$. We plot the empirical probability as a function of $\epsilon$.
- We add to the plot the Hoeffding bound of that probability as a function of $\epsilon$.


## k-NN
We study the performance of the Nearest
Neighbor (NN) algorithm on the MNIST dataset. The MNIST dataset consists of images of
handwritten digits, along with their labels. Each image has 28 × 28 pixels, where each pixel
is in gray-scale, and can get an integer value from 0 to 255. Each label is a digit between 0
and 9. The dataset has 70,000 images. Although each image is square, we treat it as a vector
of size 784.


In order to classify a new data point, the k-NN algorithm finds the k nearest neighbors of that point
in the dataset and classifies according to the majority label

- We implement a function that accepts as input: (i) a set of train images; (ii) a vector of labels,
corresponding to the images; (iii) a query image; and (iv) a number *k*. The function
implements the *k*-NN algorithm to return a prediction of the query image, given the train
images and labels. The function uses the *k* nearest neighbors, using the Euclidean
L2 metric. In case of a tie between the *k* labels of neighbors, it will choose an arbitrary
option.
- We plot the prediction accuracy as a function of *k*, for $k = 1, . . . , 100$ and $n = 1000$.
- Using $k = 1$, we run the algorithm on an increasing number of training images and plot the
prediction accuracy as a function of $n = 100, 200, . . . , 5000$.








