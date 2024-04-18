import numpy as np
import matplotlib.pyplot as plt
import intervals


class Assignment2(object):

    def sample_from_D(self, m):
        """Sample m data samples from D.
        Input: m - an integer, the size of the data sample.

        Returns: np.ndarray of shape (m,2) :
                A two-dimensional array of size m that contains the pairs where drawn from the distribution P.
        """
        x_samples = np.random.uniform(0, 1, m) # sample x uniformly from [0, 1]
        x_samples.sort()
        # sample y given x according to P[y|x]
        y_samples = np.array([np.random.choice([0, 1], p=[self.p_not_y_given_x(x), self.p_y_given_x(x)])
                              for x in x_samples])
        return np.column_stack((x_samples, y_samples))


    def experiment_m_range_erm(self, m_first, m_last, step, k, T):
        """Runs the ERM algorithm.
        Calculates the empirical error and the true error.
        Plots the average empirical and true errors.
        Input: m_first - an integer, the smallest size of the data sample in the range.
               m_last - an integer, the largest size of the data sample in the range.
               step - an integer, the difference between the size of m in each loop.
               k - an integer, the maximum number of intervals.
               T - an integer, the number of times the experiment is performed.

        Returns: np.ndarray of shape (n_steps,2).
            A two-dimensional array that contains the average empirical error
            and the average true error for each m in the range accordingly.
        """
        n_steps = (m_last - m_first) // step + 1  # number of steps
        results = np.zeros((n_steps, 2))
        print(f"running ERM with k={k}, for {n_steps} steps")
        for i, m in enumerate(range(m_first, m_last + 1, step)):
            print(f"running on {m} samples")
            empirical_errors = []
            true_errors = []
            for _ in range(T):
                sample = self.sample_from_D(m) # generate a sample of size m
                emp_error, true_error = self.get_errors(sample, k, m)
                empirical_errors.append(emp_error)
                true_errors.append(true_error)

            # Compute average empirical and true errors
            avg_empirical_error = np.mean(empirical_errors)
            avg_true_error = np.mean(true_errors)
            results[i] = [avg_empirical_error, avg_true_error]

        # plot the average empirical and true errors
        plt.plot(range(m_first, m_last + 1, step), results[:, 0], label='Empirical Error')
        plt.plot(range(m_first, m_last + 1, step), results[:, 1], label='True Error')
        plt.xlabel('m')
        plt.ylabel('Error')
        plt.title('Average Error as a Function of m')
        print("Average Error as a Function of m")
        plt.legend()
        plt.show()

        return results

    def experiment_k_range_erm(self, m, k_first, k_last, step):
        """Finds the best hypothesis for k= 1,2,...,10.
        Plots the empirical and true errors as a function of k.
        Input: m - an integer, the size of the data sample.
               k_first - an integer, the maximum number of intervals in the first experiment.
               m_last - an integer, the maximum number of intervals in the last experiment.
               step - an integer, the difference between the size of k in each experiment.

        Returns: The best k value (an integer) according to the ERM algorithm.
        """
        k_values = list(range(k_first, k_last + 1, step))
        empirical_errors = []
        true_errors = []

        print(f"running ERM with m={m}, with k from {k_first} to {k_last} with step {step}")
        sample = self.sample_from_D(m)  # generate a sample of size m
        for k in k_values:
            print(f"running on k={k}")
            emp_error, true_error = self.get_errors(sample, k, m)
            empirical_errors.append(emp_error)
            true_errors.append(true_error)

        # Plot the empirical and true errors as a function of k
        plt.plot(k_values, empirical_errors, label='Empirical Error')
        plt.plot(k_values, true_errors, label='True Error')
        plt.xlabel('k')
        plt.ylabel('Error')
        plt.title('Empirical and True Errors as a function of k')
        plt.legend()
        plt.show()

        # Find the best k value based on the minimum true error
        best_k_index = np.argmin(true_errors)
        best_k = k_values[best_k_index]
        print(f"The best k value is {best_k} with true error of {true_errors[best_k_index]}")

        return best_k

    def experiment_k_range_srm(self, m, k_first, k_last, step):
        """Run the experiment in (c).
        Plots additionally the penalty for the best ERM hypothesis.
        and the sum of penalty and empirical error.
        Input: m - an integer, the size of the data sample.
               k_first - an integer, the maximum number of intervals in the first experiment.
               m_last - an integer, the maximum number of intervals in the last experiment.
               step - an integer, the difference between the size of k in each experiment.

        Returns: The best k value (an integer) according to the SRM algorithm.
        """
        delta = 0.1
        empirical_errors = []
        true_errors = []
        penalty_errors = []
        sum_errors = []
        k_values = list(range(k_first, k_last + 1, step))

        print(f"running ERM considering SRM penalty where m={m}, with k from {k_first} to {k_last} with step {step}")
        sample = self.sample_from_D(m)  # generate a sample of size m
        for k in k_values:
            print(f"running on k={k}")
            emp_error, true_error = self.get_errors(sample, k, m)
            empirical_errors.append(emp_error)
            true_errors.append(true_error)
            # SRM
            penalty = 2 * np.sqrt((2 * k + np.log(2 / delta)) / m)  # SRM penalty, VCdim(H) = 2k
            penalty_errors.append(penalty)
            sum_errors.append(emp_error + penalty)

        # Plot the empirical error, penalty, and sum of penalty and empirical error as functions of k
        plt.plot(k_values, empirical_errors, label='Empirical Error')
        plt.plot(k_values, true_errors, label='True Error')
        plt.plot(k_values, penalty_errors, label='Penalty')
        plt.plot(k_values, sum_errors, label='Penalty + Empirical Error')
        plt.xlabel('k')
        plt.ylabel('Error')
        plt.title('Errors, Penalty, and Penalty + Empirical as functions of k')
        plt.legend()
        plt.show()

        # Find the best k value based on the minimum sum of penalty and empirical error
        best_k_index = np.argmin(sum_errors)
        best_k = k_values[best_k_index]
        print(f"The best k value is {best_k} with true error of {sum_errors[best_k_index]}")

        return best_k

    def cross_validation(self, m):
        """Finds a k that gives a good test error.
        Input: m - an integer, the size of the data sample.

        Returns: The best k value (an integer) found by the cross validation algorithm.
        """
        holdout_ratio = 0.2
        num_holdout = int(m * holdout_ratio)
        training_set = self.sample_from_D(m - num_holdout)
        holdout_validation_set = self.sample_from_D(num_holdout)

        errors = []
        hypothesis = []
        print(f"running cross validation with m={m}")
        for k in range(1, 11):
            print(f"running on k={k}")
            best_intervals, _ = intervals.find_best_interval(training_set[:, 0], training_set[:, 1], k)
            hypothesis.append(best_intervals)
            emp_error = self.calculate_empirical_error(best_intervals, holdout_validation_set)
            errors.append(emp_error)

        best_k = np.argmin(errors) + 1
        print(f"The best k value is {best_k} with true error of {errors[best_k - 1]}")
        print(f"The best hypothesis is {hypothesis[best_k - 1]}")
        return best_k

    #################################
    # Place for additional methods
    #################################

    def p_y_given_x(self, x):
        """
        The conditional probability P[y=1|x].
        :param x: A float in the range [0, 1].
        :return: the probability P[y=1|x].
        """
        if x <= 0.2 or (0.4 <= x <= 0.6) or x >= 0.8:
            return 0.8
        return 0.1
    def p_not_y_given_x(self, x):
        """
        The conditional probability P[y=0|x].
        :param x: A float in the range [0, 1].
        :return: the probability P[y=0|x].
        """
        return 1 - self.p_y_given_x(x)

    def get_intersection(self, interval1, interval2):
        """
        Get the intersection of two intervals.
        :param interval1: A tuple of two floats, the first is the left bound, the second is the right bound.
        :param interval2: A tuple of two floats, the first is the left bound, the second is the right bound.
        :return: the length of the intersection.
        """
        return min(interval1[1], interval2[1]) - max(interval1[0], interval2[0])

    def complement_intervals(self, intervals):
        """
        Get the complement of the intervals.
        :param intervals: A list of tuples, each tuple contains the left and right bounds of an interval.
        :return: the complement of the intervals.
        """
        complement_intervals = []
        if intervals[0][0] > 0: # add the space before the first interval
            complement_intervals.append([0, intervals[0][0]])
        for i in range(len(intervals) - 1): # add the space between intervals
            complement_intervals.append([intervals[i][1], intervals[i + 1][0]])
        if intervals[-1][1] < 1: # add the space after the last interval
            complement_intervals.append([intervals[-1][1], 1])
        return complement_intervals

    def calculate_true_error(self, best_intervals):
        """
        Calculate the true error of the best hypothesis.
        :param best_intervals: A list of tuples, each tuple contains the left and right bounds of an interval.
        :return: the true error.
        """
        total_error = 0
        # positive intervals for the optimal hypothesis Y assigned to the probability we need to pay for classifying 0
        positive_intervals = [([0, 0.2], 0.2), ([0.4, 0.6], 0.2), ([0.8, 1], 0.2), ([0.2, 0.4], 0.9), ([0.6, 0.8], 0.9)]
        # iterate over positive intervals (h(x)=1)
        for true_interval, miss_prob in positive_intervals:
            for best_interval in best_intervals:
                intersection = self.get_intersection(true_interval, best_interval)
                if intersection > 0:
                    # we pay the length of the intersection multiplied by P(y=0|x)
                    total_error += intersection * miss_prob

        # complement of the best intervals
        best_intervals_complement = self.complement_intervals(best_intervals)
        # negative intervals for the optimal hypothesis Y assigned to the probability we need to pay for classifying 1
        negative_intervals = [([0, 0.2], 0.8), ([0.4, 0.6], 0.8), ([0.8, 1], 0.8), ([0.2, 0.4], 0.1), ([0.6, 0.8], 0.1)]
        # iterate over negative intervals (h(x)=0)
        for negative_interval, miss_prob in negative_intervals:
            for best_interval_comp in best_intervals_complement:
                intersection = self.get_intersection(negative_interval, best_interval_comp)
                if intersection > 0:
                    # we pay the length of the intersection multiplied by P(y=1|x)
                    total_error += intersection * miss_prob

        return total_error

    def calculate_empirical_error(self, best_intervals, test_set):
        """
        Calculate the empirical error of the best hypothesis.
        :param best_intervals: A list of tuples, each tuple contains the left and right bounds of an interval.
        :param test_set: A two-dimensional array of size m that contains the pairs where drawn from the distribution P.
        :return: the empirical error.
        """
        mislabeled_count = 0
        for x, y in test_set:
            # check if x falls into any of the intervals
            in_interval = any(l <= x <= u for l, u in best_intervals)
            # update total error if the classification is incorrect
            if (in_interval and y == 0) or (not in_interval and y == 1):
                mislabeled_count += 1
        return mislabeled_count / len(test_set)

    def get_errors(self, sample, k, m):
        """
        Get the empirical and true errors of the best hypothesis retrieved by running find_best_interval.
        :param sample: A two-dimensional array of size m that contains the pairs where drawn from the distribution P.
        :param k: An integer, the maximum number of intervals.
        :param m: An integer, the size of the data sample.
        :return: A tuple of two floats, the empirical error and the true error.
        """
        best_intervals, empirical_error = intervals.find_best_interval(sample[:, 0], sample[:, 1], k)
        true_error = self.calculate_true_error(best_intervals)
        return empirical_error / m, true_error


if __name__ == '__main__':
    ass = Assignment2()
    # (b)
    ass.experiment_m_range_erm(10, 100, 5, 3, 100)
    # (c)
    ass.experiment_k_range_erm(1500, 1, 10, 1)
    # (d)
    ass.experiment_k_range_srm(1500, 1, 10, 1)
    # (e)
    ass.cross_validation(1500)
