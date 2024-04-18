import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import fetch_openml

def get_data():
    # load the MNIST dataset
    mnist = fetch_openml('mnist_784', as_frame=False)
    data = mnist['data']
    labels = mnist['target']

    # define the train and test sets
    idx = np.random.RandomState(0).choice(70000, 11000)
    train = data[idx[:10000], :].astype(int)
    train_labels = labels[idx[:10000]]
    test = data[idx[10000:], :].astype(int)
    test_labels = labels[idx[10000:]]

    return train, train_labels, test, test_labels

# (a) - a function that implements the KNN algorithm
def knn(train_images, train_labels, query_image, k):
    # calculate distances between the query image and all train images (treating each row as a separate vector)
    distances = np.linalg.norm(train_images - query_image, axis=1)

    # get labels of k-nearest neighbors
    nearest_indices = np.argsort(distances)[:k]
    nearest_labels = train_labels[nearest_indices].astype(int)

    # count occurrences of each label
    label_counts = np.bincount(nearest_labels)

    # choose label with the maximum count and return as string
    return str(np.argmax(label_counts))


# calculate accuracy of KNN algorithm for a given k and first n train images
def get_accuracy(train_images, train_labels, test_images, test_labels, n, k):
    correct = 0
    # get the first n images from the train set
    train_images = train_images[:n]
    train_labels = train_labels[:n]
    for i, test_image in enumerate(test_images):
        predicted_label = knn(train_images, train_labels, test_image, k)
        if predicted_label == test_labels[i]:
            correct += 1
    return correct / len(test_labels)

def plot_k(train_images, train_labels, test_images, test_labels):
    accuracies = []
    for k in range(1, 101):
        if k % 10 == 0:
            print(f"k={k}")
        accuracies.append(get_accuracy(train_images, train_labels, test_images, test_labels, n=1000, k=k))
    plt.plot(range(1, 101), accuracies)
    plt.xlabel("k")
    plt.ylabel("accuracy")
    plt.title("Prediction accuracy as a function of k")
    print("Prediction accuracy as a function of k")
    plt.show()

def plot_n(train_images, train_labels, test_images, test_labels):
    accuracies = []
    for n in range(100, 5001, 100):
        if n % 1000 == 0:
            print(f"n={n}")
        accuracies.append(get_accuracy(train_images, train_labels, test_images, test_labels, n=n, k=1))
    plt.plot(range(100, 5001, 100), accuracies)
    plt.xlabel("n")
    plt.ylabel("accuracy")
    plt.title("Prediction accuracy as a function of n")
    print("Prediction accuracy as a function of n")
    plt.show()


if __name__ == '__main__':
    train_images, train_labels, test_images, test_labels = get_data()

    # (b) - calculate accuracy for n=1000 and k=10
    accuracy = get_accuracy(train_images, train_labels, test_images, test_labels, n=1000, k=10)
    print(f"(b) Accuracy = {accuracy * 100}%")

    # (c) - plot the prediction accuracy as a function of k for k=1,...,100 and n=1000
    print("(c) getting results...")
    plot_k(train_images, train_labels, test_images, test_labels)

    # (d) - plot the prediction accuracy as a function of n for n=100,200,...,5000
    print("(d) getting results...")
    plot_n(train_images, train_labels, test_images, test_labels)
