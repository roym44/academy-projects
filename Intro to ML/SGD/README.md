# Introduction
An assignment given as part of the Intro to ML (fall 2024) course - optimazing different loss functions using SGD (stochastic gradient descent).

We continue working with the MNIST data set. The helper function contains the code to load the training,
validation and test sets for the digits 0 and 8 from the MNIST data, with the labels $−1/+1$.

## SGD for Hinge loss
We optimize the Hinge loss with *L2*-regularization $(ℓ(w, x, y) = C · max\{0, 1 − y⟨w, x⟩\} + 0.5∥w∥^2)$, using SGD. Namely, we initialize $w_1 = 0$, and at each iteration $t = 1, . . .$ we sample $i$
uniformly; and if $y_iw_t · x_i < 1$, we update:\
$w_{t+1} = (1 − η_t)w_t + η_tCy_ix_i$
and $w_{t+1} = (1 − η_t)w_t$ otherwise, where $η_t = η_0/t$, and $η_0$ is a constant.\
We implement an SGD function that accepts the samples and their labels, $C$, $η_0$ and $T$, and runs
$T$ gradient updates as specified above.

- We train the classifier on the training set, then use cross-validation on the validation set to find the best $η_0$, assuming $T = 1000$ and $C = 1$. For each possible $η_0$ (for example, on the log scale $η_0 = 10^{−5}, 10^{−4}, . . . , 10^4, 10^5$), we assess the performance of $η_0$ by averaging the accuracy on the validation set across 10 runs. We plot the average accuracy on the validation set, as a function of $η_0$.
- Now we cross-validate on the validation set to find the best $C$ given the best $η_0$ we found above. For each possible $C$, we average the accuracy on the validation set across 10 runs, and then plot the average accuracy on the validation set, as a function of $C$.
- Using the best $C, η_0$ we train the classifier, but for $T = 20000$.
We show the resulting **w** as an image.


## SGD for log-loss
Here we optimize the log loss defined as follows:\
$ℓ_{log} (w, x, y) = log(1 + e^{−yw·x})$

After deriving the gradient update for this case, we can implement the appropriate SGD function.
- We train the classifier on the training set using SGD, then use cross-validation on the validation set to find the best $η_0$, assuming $T = 1000$. For each possible $η_0$, we assess the performance of $η_0$ by averaging the accuracy on the validation set across 10 runs. We plot the average accuracy on the validation set, as a function of $η_0$.
- Using the best $η_0$ we train the classifier, but for $T = 20000$. We show the resulting **w** as an image.
- Now we train the classifier for $T = 20000$ iterations, and plot the norm of **w** as a function of the iteration. We can see how the norm changes as SGD progresses.








