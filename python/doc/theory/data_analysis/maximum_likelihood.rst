.. _maximum_likelihood:

Maximum Likelihood Principle
----------------------------

This method deals with the parametric modeling of a probability
distribution for a random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. The appropriate
probability distribution is found by using a sample of data
:math:`\left\{ \vect{x}_1,\ldots,\vect{x}_\sampleSize \right\}`. Such an approach
can be described in two steps as follows:

-  Choose a probability distribution (e.g. the Normal distribution, or
   any other distribution available),

-  Find the parameter values :math:`\vect{\theta}` that characterize the
   probability distribution (e.g. the mean and standard deviation for
   the Normal distribution) which best describes the sample
   :math:`\left\{ \vect{x}_1,\ldots,\vect{x}_\sampleSize \right\}`.

The maximum likelihood method is used for the second step.

This method is restricted to the
case where :math:`n_X = 1` and continuous probability distributions.
Please note therefore that :math:`\vect{X} = X^1 = X` in the following
text. The maximum likelihood estimate (MLE) of :math:`\vect{\theta}` is
defined as the value of :math:`\vect{\theta}` which maximizes the
likelihood function :math:`L\left(X,\vect{\theta}\right)`:

.. math::

   \begin{aligned}
       \widehat{\vect{\theta}} = \textrm{argmax}\ L\left(X,\vect{\theta} \right)
   \end{aligned}

Given that :math:`\left\{x_1,\ldots,x_\sampleSize \right\}` is a sample of
independent identically distributed (i.i.d) observations,
:math:`L\left(x_1,\ldots, x_\sampleSize, \vect{\theta} \right)` represents the
probability of observing such a sample assuming that they are taken from
a probability distribution with parameters :math:`\vect{\theta}`. In
concrete terms, the likelihood
:math:`L\left(x_1,\ldots, x_\sampleSize, \vect{\theta}\right)` is calculated as
follows:

.. math:: L\left(x_1,\ldots, x_\sampleSize, \vect{\theta} \right) = \prod_{j=1}^{\sampleSize} f_X\left(x_j;\vect{\theta} \right)

if the distribution is continuous, with density
:math:`f_X\left(x;\vect{\theta}\right)`.

For example, if we suppose that :math:`X` is a Gaussian distribution
with parameters :math:`\vect{\theta}= \{ \mu,\sigma \}` (i.e. the mean
and standard deviation),

.. math::

   \begin{aligned}
       L\left(x_1,\ldots, x_\sampleSize, \vect{\theta}\right)
       &= \prod_{j=1}^{\sampleSize} \frac{1}{\sigma \sqrt{2\pi}} \exp \left( -\frac{1}{2} \left( \frac{x_j-\mu}{\sigma}  \right)^2  \right) \\
       &= \frac{1}{\sigma^\sampleSize (2\pi)^{\sampleSize/2}} \exp \left( -\frac{1}{2\sigma^2} \sum_{j=1}^\sampleSize \left( x_j-\mu \right)^2  \right)
     \end{aligned}

The following figure graphically illustrates the maximum likelihood
method, in the particular case of a Gaussian probability distribution.

.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    import openturns.viewer as otv

    distribution = ot.Normal(4.0, 1.0)
    graph = distribution.drawPDF()

    fig = plt.figure(figsize=(6, 4))
    axis = fig.add_subplot(111)
    axis.set_xlim(auto=True)

    N = 6
    # coordinates of points
    x = [v[0] for v in distribution.getSample(N)]
    y = [distribution.computePDF([v]) for v in x]

    # draw lines
    for dot_x, dot_y in zip(x, y):
        plt.plot([dot_x, 1.0], [dot_y, dot_y], 'b--', linewidth=1.5)
        plt.plot([dot_x, dot_x], [0.0, dot_y], 'b--', linewidth=1.5)

    # draw labels
    for i in range(N):
        plt.text(x[i] - 0.1, -0.015, f'x{i + 1}')
        plt.text(0.0, y[i] - 0.01, f'$f_X$(x{i + 1})')

    otv.View(graph, figure=fig, axes=[axis], add_legend=True)

In general, in order to maximize the likelihood function classical
optimization algorithms (e.g. gradient type) can be used. The Gaussian
distribution case is an exception to this, as the maximum likelihood
estimators are obtained analytically:

.. math::

   \begin{aligned}
       \widehat{\mu}
       & = \frac{1}{\sampleSize} \sum_{i=1}^\sampleSize x_i, \\
       \widehat{\sigma^2}
       & = \frac{1}{\sampleSize} \sum_{i=1}^\sampleSize \left( x_i - \widehat{\mu} \right)^2
   \end{aligned}

.. topic:: API:

    - See :class:`~openturns.MaximumLikelihoodFactory`

.. topic:: Examples:

    - See :doc:`/auto_data_analysis/distribution_fitting/plot_maximumlikelihood_estimator`

Asymptotic distribution of the maximum likelihood estimator
-----------------------------------------------------------

Under regularity conditions, the maximum likelihood estimator (MLE)
:math:`\widehat{\vect{\theta}}` is asymptotically normal. More precisely,

.. math::

   \sqrt{\sampleSize} \left( \widehat{\vect{\theta}} - \vect{\theta}_0 \right)
   \xrightarrow{d} \mathcal{N}\left( \vect{0}, \mat{I}(\vect{\theta}_0)^{-1} \right)

where :math:`\vect{\theta}_0` is the true parameter value and
:math:`\mat{I}(\vect{\theta}_0)` is the Fisher information matrix.

The Fisher information matrix measures the amount of information that the
data carries about the unknown parameter :math:`\vect{\theta}`. For a
parametric model with log-density :math:`\log f_X(x; \vect{\theta})`, it is
defined as:

.. math::

   \mat{I}(\vect{\theta})_{ij}
   = \mathbb{E}\left[ \frac{\partial \log f_X(X; \vect{\theta})}{\partial \theta_i}
                     \frac{\partial \log f_X(X; \vect{\theta})}{\partial \theta_j} \right]

In practice, the expected Fisher information matrix is often replaced by
the **full-sample Fisher information matrix**, evaluated at the MLE
:math:`\widehat{\vect{\theta}}`:

.. math::

   \widehat{\mat{I}}_{\sampleSize}\left(\widehat{\vect{\theta}}\right)_{ij}
   = \sum_{k=1}^{\sampleSize}
     \frac{\partial \log f_X\left(x_k; \widehat{\vect{\theta}}\right)}{\partial \theta_i}
     \frac{\partial \log f_X\left(x_k; \widehat{\vect{\theta}}\right)}{\partial \theta_j}

The asymptotic covariance matrix of the MLE is then estimated by the
inverse of the full-sample Fisher information matrix:

.. math::

   \widehat{\mat{\Sigma}}_{\widehat{\vect{\theta}}}
   = \widehat{\mat{I}}_{\sampleSize}(\widehat{\vect{\theta}})^{-1}

This result is used by the
:meth:`~openturns.DistributionFactory.buildEstimator` method when the
model is estimated by maximum likelihood and is regular. In that case,
the parameter distribution is a normal distribution:
:math:`\vect{\theta} \sim \mathcal{N}\left( \widehat{\vect{\theta}},
\widehat{\mat{I}}_{\sampleSize}(\widehat{\vect{\theta}})^{-1} \right)`.

.. topic:: API:

    - :meth:`~openturns.DistributionFactory.buildEstimator`

.. topic:: Examples:

    - :doc:`/auto_data_analysis/distribution_fitting/plot_asymptotic_estimators_distribution`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
