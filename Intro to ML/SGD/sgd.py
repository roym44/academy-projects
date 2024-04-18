import numpy as np
import numpy.random
import scipy
from matplotlib import pyplot as plt
from sklearn.datasets import fetch_openml
import sklearn.preprocessing

def helper():
    mnist = fetch_openml('mnist_784', as_frame=False)
    data = mnist['data']
    labels = mnist['target']

    neg, pos = "0", "8"
    train_idx = numpy.random.RandomState(0).permutation(np.where((labels[:60000] == neg) | (labels[:60000] == pos))[0])
    test_idx = numpy.random.RandomState(0).permutation(np.where((labels[60000:] == neg) | (labels[60000:] == pos))[0])

    train_data_unscaled = data[train_idx[:6000], :].astype(float)
    train_labels = (labels[train_idx[:6000]] == pos) * 2 - 1

    validation_data_unscaled = data[train_idx[6000:], :].astype(float)
    validation_labels = (labels[train_idx[6000:]] == pos) * 2 - 1

    test_data_unscaled = data[60000 + test_idx, :].astype(float)
    test_labels = (labels[60000 + test_idx] == pos) * 2 - 1

    # Preprocessing
    train_data = sklearn.preprocessing.scale(train_data_unscaled, axis=0, with_std=False)
    validation_data = sklearn.preprocessing.scale(validation_data_unscaled, axis=0, with_std=False)
    test_data = sklearn.preprocessing.scale(test_data_unscaled, axis=0, with_std=False)
    return train_data, train_labels, validation_data, validation_labels, test_data, test_labels



def SGD_hinge(data, labels, C, eta_0, T):
    """
    Implements SGD for hinge loss.
    """
    # initialize weights vector
    num_features = data.shape[1]
    w = np.zeros(num_features)
    # run T gradient updates
    for t in range(1, T + 1):
        # sample a data point uniformly
        i = np.random.randint(data.shape[0])
        xi, yi = data[i], labels[i]
        eta_t = eta_0 / t # compute learning rate for this iteration
        margin = yi * np.dot(w, xi) # compute margin
        # update weights
        w = w * (1 - eta_t)
        if margin < 1:
            w += eta_t * C * yi * xi
    return w


def SGD_log(data, labels, eta_0, T):
    """
    Implements SGD for log loss.
    """
    # initialize weights vector
    num_features = data.shape[1]
    w = np.zeros(num_features)
    norm_values = []  # to store norm values at each iteration
    # run T gradient updates
    for t in range(1, T + 1):
        # sample a data point uniformly
        i = np.random.randint(data.shape[0])
        xi, yi = data[i], labels[i]
        eta_t = eta_0 / t
        exp = scipy.special.softmax([0, -yi * np.dot(w, xi)])[-1]
        gradient = -yi * xi * exp
        # update weights
        w -= eta_t * gradient
        # store norm value
        norm_values.append(np.linalg.norm(w))
    return w, norm_values


def cross_validation(train_data, train_labels, validation_data, validation_labels,
                     C_values, eta_values, T, num_runs=10, best_C=False, log_loss=False):
    """
    Train on the training set and perform cross-validation on the validation set
    """
    accuracies = []
    # finding the best C
    if best_C:
        eta_0 = eta_values[0]
        values = C_values
    # finding the best eta_0
    else:
        C = C_values[0]
        values = eta_values

    # perform cross-validation
    for value in values:
        avg_accuracy = 0
        for _ in range(num_runs):
            # train the classifier using SGD with the current eta_0 on train_data
            if best_C:
                weights = SGD_hinge(train_data, train_labels, value, eta_0, T)
            else:
                if log_loss:
                    weights, _ = SGD_log(train_data, train_labels, value, T)
                else:
                    weights = SGD_hinge(train_data, train_labels, C, value, T)

            # predict using the trained weights on the validation set
            predictions = np.sign(np.dot(validation_data, weights))
            # calculate accuracy
            accuracy = np.mean(predictions == validation_labels)
            avg_accuracy += accuracy
        # average accuracy across runs
        avg_accuracy /= num_runs
        accuracies.append(avg_accuracy)
    return accuracies

def hinge_best_eta(train_data, train_labels, validation_data, validation_labels, eta_values):
    avg_accuracies_eta = cross_validation(train_data, train_labels, validation_data, validation_labels,
                                          C_values=[1], eta_values=eta_values, T=1000, best_C=False, log_loss=False)
    # Plot the average accuracy on the validation set as a function of eta_0
    plt.figure()
    plt.semilogx(eta_values, avg_accuracies_eta)
    plt.title('Average Accuracy on Validation Set vs. eta_0')
    plt.xlabel('eta_0')
    plt.ylabel('Average Accuracy')
    plt.grid(True)
    plt.show()

    best_eta_0_index = np.argmax(avg_accuracies_eta)
    print(f"beat_accuracy = {avg_accuracies_eta[best_eta_0_index]}")
    best_eta_0 = eta_values[best_eta_0_index]
    print(f"best_eta_0 = {best_eta_0}")
    return best_eta_0


def hinge_best_C(train_data, train_labels, validation_data, validation_labels, C_values, eta_0):
    avg_accuracies_C = cross_validation(train_data, train_labels, validation_data, validation_labels,
                                        C_values=C_values, eta_values=[eta_0], T=1000, best_C=True, log_loss=False)
    # Plot the average accuracy on the validation set as a function of C
    plt.figure()
    plt.semilogx(C_values, avg_accuracies_C, marker='o')
    plt.title('Average Accuracy on Validation Set vs. C')
    plt.xlabel('C')
    plt.ylabel('Average Accuracy')
    plt.grid(True)
    plt.show()

    best_C_index = np.argmax(avg_accuracies_C)
    print(f"beat_accuracy = {avg_accuracies_C[best_C_index]}")
    best_C = C_values[best_C_index]
    print(f"best_C = {best_C}")
    return best_C


def log_best_eta(train_data, train_labels, validation_data, validation_labels, eta_values):
    avg_accuracies_eta = cross_validation(train_data, train_labels, validation_data, validation_labels,
                                          C_values=[None], eta_values=eta_values, T=1000, best_C=False, log_loss=True)
    # Plot the average accuracy on the validation set as a function of eta_0
    plt.figure()
    plt.semilogx(eta_values, avg_accuracies_eta)
    plt.title('Average Accuracy on Validation Set vs. eta_0')
    plt.xlabel('eta_0')
    plt.ylabel('Average Accuracy')
    plt.grid(True)
    plt.show()

    best_eta_0_index = np.argmax(avg_accuracies_eta)
    print(f"beat_accuracy = {avg_accuracies_eta[best_eta_0_index]}")
    best_eta_0 = eta_values[best_eta_0_index]
    print(f"best_eta_0 = {best_eta_0}")
    return best_eta_0


def hinge_loss_experiment():
    print(f"--- SGD for Hinge Loss ---")
    train_data, train_labels, validation_data, validation_labels, test_data, test_labels = helper()
    print(f"fetched data for training and validation")
    # Q1 - (a)
    eta_values_options = [np.logspace(-5, 5, num=11), np.logspace(-1, 1, num=150), np.arange(0.1, 1.0, 0.01)]
    for eta_values in eta_values_options:
        best_eta_0 = hinge_best_eta(train_data, train_labels, validation_data, validation_labels, eta_values)
    # Q1 - (b)
    C_values = np.logspace(-5, 5, num=11)
    best_C = hinge_best_C(train_data, train_labels, validation_data, validation_labels, C_values, best_eta_0)
    # Q1 - (c)
    best_w = SGD_hinge(train_data, train_labels, best_C, best_eta_0, 20000)
    plt.figure()
    plt.imshow(np.reshape(best_w, (28, 28)), interpolation='nearest', cmap='gray')
    plt.title('Weights Vector as Image')
    plt.axis('off')
    plt.show()
    # Q1 - (d)
    predictions_test = np.sign(np.dot(test_data, best_w))
    test_accuracy = np.mean(predictions_test == test_labels)
    print("Test accuracy of the best classifier:", test_accuracy)

def log_loss_experiment():
    print(f"--- SGD for Log Loss ---")
    train_data, train_labels, validation_data, validation_labels, test_data, test_labels = helper()
    print(f"fetched data for training and validation")
    # Q2 - (a)
    eta_values_options = []
    best_eta_0 = log_best_eta(train_data, train_labels, validation_data, validation_labels, np.logspace(-5, 5, num=11))
    # Q2 - (b)
    best_w, norm_values = SGD_log(train_data, train_labels, best_eta_0, 20000)
    plt.figure()
    plt.imshow(np.reshape(best_w, (28, 28)), interpolation='nearest', cmap='gray')
    plt.title('Weights Vector as Image')
    plt.axis('off')
    plt.show()
    predictions_test = np.sign(np.dot(test_data, best_w))
    test_accuracy = np.mean(predictions_test == test_labels)
    print("Test accuracy of the best classifier:", test_accuracy)
    # Q2 - (c)
    # Plot the norm of w as a function of the iteration
    plt.figure()
    plt.plot(range(1, 20001), norm_values)
    plt.title('Norm of w as a Function of Iteration')
    plt.xlabel('Iteration')
    plt.ylabel('Norm of w')
    plt.grid(True)
    plt.show()


if __name__ == '__main__':
    hinge_loss_experiment()
    log_loss_experiment()
