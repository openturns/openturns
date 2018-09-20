.. _monte_carlo_simulation:

Monte Carlo simulation
----------------------

Using the probability distribution the probability distribution of a random
vector :math:`\vect{X}`, we seek to evaluate the following probability:

.. math::

    P_f = \Prob{g\left( \vect{X},\vect{d} \right) \leq 0}

Here, :math:`\vect{X}` is a random vector, :math:`\vect{d}` a deterministic
vector, :math:`g(\vect{X},\vect{d})` the function known as *limit state function*
which enables the definition of the event :math:`\cD_f = \{\vect{X} \in \Rset^n \, / \, g(\vect{X},\vect{d}) \le 0\}`.


If we have the set :math:`\left\{ \vect{x}_1,\ldots,\vect{x}_N \right\}` of *N*
independent samples of the random vector :math:`\vect{X}`,
we can estimate :math:`\widehat{P}_f` as follows:

.. math::

    \widehat{P}_f = \frac{1}{N} \sum_{i=1}^N \mathbf{1}_{ \left\{ g(\vect{x}_i,\vect{d}) \leq 0 \right\} }

where :math:`\mathbf{1}_{ \left\{ g(\vect{x}_i,\vect{d}) \leq 0 \right\} }`
describes the indicator function equal to 1 if :math:`g(\vect{x}_i,\vect{d}) \leq 0`
and equal to 0 otherwise; the idea here is in fact to estimate the required
probability by the proportion of cases, among the *N* samples of :math:`\vect{X}`,
for which the event :math:`\cD_f` occurs.

By the law of large numbers, we know that this estimation converges to the
required value :math:`P_f` as the sample size *N* tends to infinity.

The Central Limit Theorem allows to build an asymptotic confidence interval
using the normal limit distribution as follows:

.. math::

    \lim_{N\rightarrow\infty}\Prob{P_f\in[\widehat{P}_{f,\inf},\widehat{P}_{f,\sup}]}=\alpha

with :math:`\widehat{P}_{f,\inf}=\widehat{P}_f - q_{\alpha}\sqrt{\frac{\widehat{P}_f(1-\widehat{P}_f)}{N}}$, $\widehat{P}_{f,\sup}=\widehat{P}_f + q_{\alpha}\sqrt{\frac{\widehat{P}_f(1-\widehat{P}_f)}{N}}`
and :math:`q_\alpha` is the :math:`(1+\alpha)/2`-quantile of the standard normal distribution.

One can also use a convergence indicator that is independent of the confidence
level $\alpha$: the coefficient of variation, which is the ratio between the
asymptotic standard deviation of the estimate and its mean value.
It is a relative measure of dispersion given by:

.. math::

    \textrm{CV}_{\widehat{P}_f}=\sqrt{ \frac{1-\widehat{P}_f}{N \widehat{P}_f}}\simeq\frac{1}{\sqrt{N\widehat{P}_f}}\mbox{ for }\widehat{P}_f\ll 1

It must be emphasized that these results are *asymptotic* and as such needs that :math:`N\gg 1`.
The convergence to the standard normal distribution is dominated by the skewness
of :math:`\mathbf{1}_{ \left\{ g(\vect{x}_i,\vect{d}) \leq 0 \right\} }`
divided by the sample size *N*, it means :math:`\frac{1-2P_f}{N\sqrt{P_f(1-P_f)}}`.
In the usual case :math:`P_f\ll 1`, the distribution is highly skewed and this
term is approximately equal to :math:`\frac{1}{N\sqrt{P_f}}\simeq\textrm{CV}_{\widehat{P}_f}/\sqrt{N}`.
A rule of thumb is that if :math:`\textrm{CV}_{\widehat{P}_f}<0.1`
with :math:`N>10^2`, the asymptotic nature of the Central Limit Theorem is not problematic.

.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    from openturns.viewer import View

    f = ot.SymbolicFunction(['x'], ['17-exp(0.1*(x-1.0))'])
    graph = f.draw(0.0, 12.0)

    dist = ot.Normal([5.0, 15.0], [1.0, 0.25], ot.IdentityMatrix(2))
    N = 1000
    sample = dist.getSample(N)
    sample1 = ot.Sample(0, 2)
    sample2 = ot.Sample(0, 2)
    for X in sample:
        x, y = X
        if f([x])[0] > y:
            sample1.add(X)
        else:
            sample2.add(X)

    cloud = ot.Cloud(sample1)
    cloud.setColor('green')
    cloud.setPointStyle('square')
    graph.add(cloud)

    cloud = ot.Cloud(sample2)
    cloud.setColor('red')
    cloud.setPointStyle('square')
    graph.add(cloud)

    graph.setTitle('Monte Carlo simulation (Pf=0.048, N=1000)')
    graph.setLegends(['domain Df', 'simulations'])
    graph.setLegendPosition('topright')
    View(graph)

The method is also referred to as Direct sampling, Crude Monte Carlo method, Classical Monte Carlo integration.

.. topic:: API:

    - See :class:`~openturns.ProbabilitySimulationAlgorithm`

.. topic:: Examples:

    - See :doc:`/examples/reliability_sensitivity/estimate_probability_monte_carlo`

.. topic:: References:

    - Robert C.P., Casella G. (2004). Monte-Carlo Statistical Methods, Springer, ISBN 0-387-21239-6, 2nd ed.
    - Rubinstein R.Y. (1981). Simulation and The Monte-Carlo methods, John Wiley \& Sons
