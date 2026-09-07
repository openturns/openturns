%feature("docstring") OT::MultivariatePolyaFactory
R"RAW(Multivariate Polya factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Notes
-----
The parameters :math:`N`, :math:`\vect{\alpha}` and :math:`c` are estimated
from a sample :math:`(\vect{x}^1, \ldots, \vect{x}^m)` using moment
estimators, see the report section about the multivariate Polya distribution
and its estimation.

Each observation of a multivariate Polya distribution sums to the number of
draws :math:`N=\sum_{i=1}^d x_i^k`, so :math:`N` is estimated by this constant
row sum. The proportions of each category

.. math::

    p_i = \frac{1}{m N} \sum_{k=1}^{m} x_i^k

are estimated by the empirical frequencies, and the overdispersion factor
:math:`f` is estimated from the ratio of the empirical variances to the
binomial variances, each ratio being clamped to the feasible range
:math:`[1, N]`:

.. math::

    f = \frac{1}{m N} \sum_{i = 1}^d \frac{s_i^2}{N p_i (1 - p_i)}

If the sample is not significantly overdispersed, in the sense of an average
dispersion ratio within three standard deviations (:math:`3 \sqrt{2 / (m - 1)}`)
above 1, the degenerate :math:`c = 0` multinomial member of the family is
returned with :math:`\alpha_i = p_i`. Otherwise the ratio parameter
:math:`\theta = (N - f) / (f - 1)` is used to set the concentration parameters
:math:`\alpha_i = \theta p_i` with :math:`c = 1`.

Only integer samples with a constant row sum and strictly positive column sums
are accepted.

See also
--------
openturns.DistributionFactory, MultivariatePolya)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariatePolyaFactory::buildAsMultivariatePolya
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsMultivariatePolya()

    buildAsMultivariatePolya(*sample*)

Parameters
----------
sample : 2-d sequence of float
    Data.

Returns
-------
distribution : :class:`~openturns.experimental.MultivariatePolya`
    The estimated distribution as a MultivariatePolya.

    In the first usage, the default MultivariatePolya distribution is built.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> sample = ot.Sample([[3, 2, 1], [1, 3, 2], [2, 1, 3], [0, 4, 2], [2, 2, 2]])
>>> estimatedDistribution = otexp.MultivariatePolyaFactory().buildAsMultivariatePolya(sample)
>>> print(estimatedDistribution.getN())
6

Draw the PDF of a two-dimensional estimate:

>>> sample2D = ot.Sample([[3, 0], [0, 3], [2, 1], [1, 2], [0, 3]])
>>> estimatedDistribution2D = otexp.MultivariatePolyaFactory().buildAsMultivariatePolya(sample2D)
>>> graph = estimatedDistribution2D.drawPDF()
"