.. _maximum_likelihood:

Maximum Likelihood Principle
----------------------------

This method deals with the parametric modeling of a probability
distribution for a random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. The appropriate
probability distribution is found by using a sample of data
:math:`\left\{ \vect{x}_1,\ldots,\vect{x}_N \right\}`. Such an approach
can be described in two steps as follows:

-  Choose a probability distribution (e.g. the Normal distribution, or
   any other distribution available),

-  Find the parameter values :math:`\vect{\theta}` that characterize the
   probability distribution (e.g. the mean and standard deviation for
   the Normal distribution) which best describes the sample
   :math:`\left\{ \vect{x}_1,\ldots,\vect{x}_N \right\}`.

The maximum likelihood method is used for the second step.

This method is restricted to the
case where :math:`n_X = 1` and continuous probability distributions.
Please note therefore that :math:`\vect{X} = X^1 = X` in the following
text. The maximum likelihood estimate (MLE) of :math:`\vect{\theta}` is
defined as the value of :math:`\vect{\theta}` which maximizes the
likelihood function :math:`L\left(X,\vect{\theta}\right)`:

.. math::

   \begin{aligned}
       \hat{\vect{\theta}} = \textrm{argmax}\ L\left(X,\vect{\theta} \right)
     \end{aligned}

Given that :math:`\left\{x_1,\ldots,x_N \right\}` is a sample of
independent identically distributed (i.i.d) observations,
:math:`L\left(x_1,\ldots, x_N, \vect{\theta} \right)` represents the
probability of observing such a sample assuming that they are taken from
a probability distribution with parameters :math:`\vect{\theta}`. In
concrete terms, the likelihood
:math:`L\left(x_1,\ldots, x_N, \vect{\theta}\right)` is calculated as
follows:

.. math:: L\left(x_1,\ldots, x_N, \vect{\theta} \right) = \prod_{j=1}^{N} f_X\left(x_j;\vect{\theta} \right)

if the distribution is continuous, with density
:math:`f_X\left(x;\vect{\theta}\right)`.

For example, if we suppose that :math:`X` is a Gaussian distribution
with parameters :math:`\vect{\theta}= \{ \mu,\sigma \}` (i.e. the mean
and standard deviation),

.. math::

   \begin{aligned}
       L\left(x_1,\ldots, x_N, \vect{\theta}\right) &=& \prod_{j=1}^{N} \frac{1}{\sigma \sqrt{2\pi}} \exp \left[ -\frac{1}{2} \left( \frac{x_j-\mu}{\sigma}  \right)^2  \right] \\
       &=& \frac{1}{\sigma^N (2\pi)^{N/2}} \exp \left[ -\frac{1}{2\sigma^2} \sum_{j=1}^N \left( x_j-\mu \right)^2  \right]
     \end{aligned}

The following figure graphically illustrates the maximum likelihood
method, in the particular case of a Gaussian probability distribution.

.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    from openturns.viewer import View

    distribution = ot.Normal(4.0, 1.0)
    graph = distribution.drawPDF()

    fig = plt.figure(figsize=(6, 4))
    axis = fig.add_subplot(111)
    axis.set_xlim(auto=True)

    N=6
    # coordinates of points
    x = [v[0] for v in distribution.getSample(N)]
    y = [distribution.computePDF([v]) for v in x]

    # draw lines
    for dot_x, dot_y in zip(x, y):
        plt.plot([dot_x, 1.0], [dot_y, dot_y], 'b--', linewidth=1.5)
        plt.plot([dot_x, dot_x], [0.0, dot_y], 'b--', linewidth=1.5)

    # draw labels
    for i in range(N):
        plt.text(x[i]-0.1, -0.015, 'x'+str(i+1))
        plt.text(0.0, y[i]-0.01, 'f_X(x'+str(i+1)+')')

    View(graph, figure=fig, axes=[axis], add_legend=True)

In general, in order to maximize the likelihood function classical
optimization algorithms (e.g. gradient type) can be used. The Gaussian
distribution case is an exception to this, as the maximum likelihood
estimators are obtained analytically:

.. math::

   \begin{aligned}
       \widehat{\mu}  = \frac{1}{N} \sum_{i=1}^N x_i,\ \widehat{\sigma^2} = \frac{1}{N} \sum_{i=1}^N \left( x_i - \widehat{\mu} \right)^2
     \end{aligned}

.. topic:: API:

    - See :class:`~openturns.MaximumLikelihoodFactory`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/maximumlikelihood_estimator`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
