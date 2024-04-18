# Introduction
An assignment given as part of the Intro to ML (fall 2024) course - regarding the hypothesis class of a finite union of disjoint intervals, and the properties of the ERM algorithm for this class.


## Union of Intervals
Let the sample space be $\mathcal{X} = [0, 1]$ and consider a binary classification problem, i.e. $\mathcal{Y} = {0, 1}$. We will try to learn using an hypothesis class that consists of *k* intervals. More explicitly, let $I = \{[l1, u1], . . . , [lk, uk]\}$ be *k* disjoint intervals, such that $0 ≤ l1 ≤ u1 ≤ l2 ≤ u2 ≤ . . . ≤ uk ≤ 1$. For each such *k* disjoint intervals, define the corresponding hypothesis as

$
h_I(x) = 
\begin{cases}
1 & \text{if } x \in [l_1, u_1] \cup \cdots \cup [l_k, u_k] \\
0 & \text{otherwise}
\end{cases}
$

Finally, define Hk as the hypothesis class that consists of all hypotheses that correspond
to k disjoint intervals:

$\mathcal{H}_k = \{hI |I = \{[l1, u1], . . . , [lk, uk]\}, 0 ≤ l1 ≤ u1 ≤ l2 ≤ u2 ≤ . . . ≤ uk ≤ 1\}$

We are given a sample of size *n*: $(x1, y1), . . . , (xn, yn)$. Assume that the points are sorted, so that $0 ≤ x1 < x2 < . . . < xn ≤ 1$.

## intervals.py
It includes a function that implements an ERM algorithm for $\mathcal{H}_k$.
Given a sorted list $xs = [x1, . . . , xn]$, the respective labeling $ys = [y1, . . . , yn]$ and *k*, the
given function `find_best_interval` returns a list of up to *k* intervals and their error count on the given sample. These intervals have the smallest empirical error count possible from all choices of *k* intervals or less.

## intervals_analysis.py
- We write a function that, given a list of intervals *I*, calculates the true error $e_P(h_I)$. Then, for $k = 3, n = 10, 15, 20, . . . , 100$, perform the following
experiment $T = 100$ times:\
(i) Draw a sample of size n and run the ERM algorithm
on it;\
(ii) Calculate the empirical error for the returned hypothesis;\
(iii) Calculate
the true error for the returned hypothesis.\
We plot the empirical and true errors, averaged across the *T* runs, as a function of *n*.
- We draw a sample of size $n = 1500$, find the best ERM hypothesis for $k = 1, 2, . . . , 10$, and plot the empirical and true errors as a function of k.
- Now we use the principle of structural risk minimization (SRM), to search for a *k* that gives a good test error. Let $δ = 0.1$.\
We use the following penalty function: $2\sqrt{\frac{VCdim(\mathcal{H}_k)+ln \frac{2}{\delta}}{n}}$\
We draw a data set of $n = 1500$ samples, run the previous experiment again, but now plot two additional lines as a function of *k*: 1) the penalty for the best
ERM hypothesis and 2) the sum of penalty and empirical error.