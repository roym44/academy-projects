import numpy as np
import matplotlib.pyplot as plt

def get_hoeffding_bound(epsilon, n):
    return 2 * np.exp(-2 * n * epsilon ** 2)

def plot_hoeffding_bound(epsilons, empirical_probabilities, hoeffding_bounds):
    plt.plot(epsilons, empirical_probabilities, label='Empirical Probability')
    plt.plot(epsilons, hoeffding_bounds, linestyle='--', label='Hoeffding Bound')
    plt.xlabel('Epsilon')
    plt.ylabel('Probability')
    plt.title('Empirical Probability and Hoeffding Bound vs Epsilon')
    plt.legend()
    plt.show()

def visualize_hoeffding_bound(N=200000, n=20):
    # generate an N x n matrix of samples from Bernoulli(1/2)
    matrix = np.random.choice([0, 1], size=(N, n), p=[0.5, 0.5])

    # calculate the empirical mean for each row
    empirical_means = np.mean(matrix, axis=1)

    # calculate the empirical probability for each epsilon
    epsilons = np.linspace(0, 1, 50)  # Values of ϵ
    empirical_probabilities = [np.mean(np.abs(empirical_means - 0.5) > epsilon) for epsilon in epsilons]

    # calculate the Hoeffding bound for each epsilon
    hoeffding_bounds = [get_hoeffding_bound(epsilon, n) for epsilon in epsilons]

    # plot the results
    plot_hoeffding_bound(epsilons, empirical_probabilities, hoeffding_bounds)

if __name__ == '__main__':
    visualize_hoeffding_bound()
