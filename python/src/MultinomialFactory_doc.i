%feature("docstring") OT::MultinomialFactory
R"RAW(Multinomial factory.

Notes
-----
The parameters :math:`N` and :math:`p=(p_1,\dots,p_d)` are estimated from a
sample :math:`(\vect{x}^1, \ldots, \vect{x}^m)` using the following empirical
estimators:

.. math::
    :nowrap:

    \begin{eqnarray*}
      N & = & \max_{1 \leq k \leq m} \sum_{i=1}^d x_i^k\\
      p_i & = & \frac{1}{m N} \sum_{k=1}^{m} x_i^k
    \end{eqnarray*}

The number of trials :math:`N` is estimated by the largest observed total
:math:`\sum_{i=1}^d x_i^k`, and the component probabilities :math:`p_i` by the
empirical frequencies of each component.

Remember that the multinomial distribution of the library is such that
:math:`\sum_{i=1}^d X_i \leq N`.

See also
--------
DistributionFactory, Multinomial)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::MultinomialFactory::buildAsMultinomial
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsMultinomial()

    buildAsMultinomial(*sample*)

    buildAsMultinomial(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
   The parameters of the :class:`~openturns.Multinomial`.

Returns
-------
distribution : :class:`~openturns.Multinomial`
    The estimated distribution as a Multinomial.
    
    In the first usage, the default Multinomial distribution is built.

Examples
--------
>>> import openturns as ot
>>> sample = ot.Sample([[5, 0], [0, 5], [3, 2], [1, 4]])
>>> estimatedDistribution = ot.MultinomialFactory().buildAsMultinomial(sample)
>>> print(estimatedDistribution.getN())
5
>>> print(estimatedDistribution.getP())
[0.45,0.55]"

