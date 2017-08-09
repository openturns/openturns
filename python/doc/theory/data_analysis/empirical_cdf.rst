Empirical cumulative distribution function
------------------------------------------

The empirical cumulative distribution function provides a graphical
representation of the probability distribution of a random vector
without implying any prior assumption concerning the form of this
distribution. It concerns a non-parametric approach which enables the
description of complex behaviour not necessarily detected with
parametric approaches.

Therefore, using general notation, this means that we are looking for an
estimator :math:`\widehat{F}_N` for the cumulative distribution function
:math:`F_{X}` of the random variable
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`:

.. math::

   \begin{aligned}
       \widehat{F}_N \leftrightarrow F_{X}
     \end{aligned}

Let us first consider the uni-dimensional case, and let us denote
:math:`\vect{X} = X^1 = X`. The empirical probability distribution is
the distribution created from a sample of observed values
:math:`\left\{x_1, x_2, \ldots, x_N\right\}`. It corresponds to a
discrete uniform distribution on
:math:`\left\{x_1, x_2, \ldots, x_N\right\}`: where :math:`X'` follows
this distribution,

.. math::

   \begin{aligned}
       \forall \; i \in \left\{1,\ldots, N\right\} ,\ \textrm{Pr}\left(X'=x_i\right) = \frac{1}{N}
     \end{aligned}

The empirical cumulative distribution function :math:`\widehat{F}_N`
with this distribution is constructed as follows:

.. math::

   \begin{aligned}
       F_N(x) = \frac{1}{N} \sum_{i=1}^N \mathbf{1}_{ \left\{ x_i \leq x \right\} }
     \end{aligned}

The empirical cumulative distribution function :math:`F_N(x)` is defined
as the proportion of observations that are less than (or equal to)
:math:`x` and is thus an approximation of the cumulative distribution
function :math:`F_X(x)` which is the probability that an observation is
less than (or equal to) :math:`x`.

.. math::

   \begin{aligned}
       F_X(x) = \textrm{Pr} \left( X \leq x \right)
     \end{aligned}

The diagram below provides an illustration of an ordered sample
:math:`\left\{5,6,10,22,27\right\}`.

.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    from openturns.viewer import View

    sample = [[5.0], [6.0], [10.0], [22.0], [27.0]]
    xmin = 0.0
    xmax = 30.0
    graph = ot.VisualTest.DrawEmpiricalCDF(sample, xmin, xmax)
    graph.setTitle('Empirical CDF')
    View(graph)

The method is similar for the case :math:`n_X>1`. The empirical
probability distribution is a distribution created from a sample
:math:`\left\{\vect{x}_1, \vect{x}_2, \ldots, \vect{x}_N\right\}`. It
corresponds to a discrete uniform distribution on
:math:`\left\{\vect{x}_1, \vect{x}_2, \ldots, \vect{x}_N\right\}`: where
:math:`\vect{X}'` follows this distribution,

.. math::

   \begin{aligned}
       \forall \; i \in \left\{1,\ldots, N\right\} ,\ \textrm{Pr}\left(\vect{X}'=\vect{x}_i\right) = \frac{1}{N}
     \end{aligned}

Thus we have:

.. math::

   \begin{aligned}
       F_N(\vect{x}) = \frac{1}{N} \sum_{i=1}^N \mathbf{1}_{ \left\{ x^1_i \leq x^1,\ldots,x^{n_X}_N \leq x^{n_X} \right\} }
     \end{aligned}

in comparison with the theoretical probability density function :math:`F_X`:

.. math::

   \begin{aligned}
       F_X(x) = \Prob{X^1 \leq x^1,\ldots,X^{n_X} \leq x^{n_X}}
     \end{aligned}

This method is also referred to in the literature as the empirical
distribution function.

.. topic:: API:

    - See :class:`~openturns.UserDefined` for the empirical distribution
    - See :py:func:`~openturns.VisualTest_DrawEmpiricalCDF` to draw an empirical CDF

.. topic:: Examples:

    - See :ref:`examples/data_analysis/visualize_empirical_cdf.ipynb`


.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
