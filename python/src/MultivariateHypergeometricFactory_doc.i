%feature("docstring") OT::MultivariateHypergeometricFactory
R"RAW(Multivariate hypergeometric factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Notes
-----
The parameters :math:`N` and :math:`(c_1, \dots, c_d)` are estimated from a
sample :math:`(\vect{x}^1, \ldots, \vect{x}^m)` using empirical estimators.

Each observation of a multivariate hypergeometric distribution sums to the
number of draws :math:`N=\sum_{i=1}^d x_i^k`, so :math:`N` is estimated by this
constant row sum. The proportions of each category

.. math::

    p_i = \frac{1}{m N} \sum_{k=1}^{m} x_i^k

are estimated by the empirical frequencies. Finally, the smallest population
size :math:`c` such that every observed category :math:`i` with :math:`p_i > 0`
rounds to a positive count :math:`c_i = \lfloor c p_i \rceil \geq 1` and such
that :math:`\sum_{i=1}^d c_i \geq N` is selected, while the unobserved
categories are set to :math:`c_i = 0`.

Only integer samples with a constant row sum are accepted.

See also
--------
openturns.DistributionFactory, MultivariateHypergeometric)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateHypergeometricFactory::buildAsMultivariateHypergeometric
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsMultivariateHypergeometric()

    buildAsMultivariateHypergeometric(*sample*)

Parameters
----------
sample : 2-d sequence of float
    Data.

Returns
-------
distribution : :class:`~openturns.experimental.MultivariateHypergeometric`
    The estimated distribution as a MultivariateHypergeometric.

    In the first usage, the default MultivariateHypergeometric distribution is built.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> sample = ot.Sample([[3, 0, 0], [0, 2, 1], [2, 1, 0], [1, 1, 1]])
>>> estimatedDistribution = otexp.MultivariateHypergeometricFactory().buildAsMultivariateHypergeometric(sample)
>>> print(estimatedDistribution.getN())
3
>>> print(estimatedDistribution.getColors())
[3,2,1]"