%feature("docstring") OT::WrappedNormalFactory
R"RAW(Wrapped Normal factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Given a sample :math:`\mathbf{x}_1,\dots,\mathbf{x}_N` on the torus,
estimates the Wrapped Normal parameters:

1. The mean is estimated by the circular mean:

   .. math::

       \hat\mu_j = \frac{p}{2\pi}
       \mathrm{atan2}\left(\sum_{i=1}^N\sin\left(\frac{2\pi
       x_{i,j}}{p}\right), \sum_{i=1}^N\cos\left(\frac{2\pi
       x_{i,j}}{p}\right)\right)

2. The covariance is estimated as the empirical covariance of the wrapped
   differences :math:`(x_{i,j}-\hat\mu_j) \mod p` mapped onto
   :math:`[-p/2,p/2]`.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- ``WrappedNormal-MaxLatticeTerms``: maximum number of lattice terms
  for exact sum in PDF computation (default: 100000).

See also
--------
openturns.DistributionFactory, WrappedNormal

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> factory = otexp.WrappedNormalFactory()
>>> distribution = otexp.WrappedNormal([0.0], ot.CovarianceMatrix([[0.5]]))
>>> estimated = factory.build(distribution.getSample(1000))
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormalFactory::buildAsWrappedNormal
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsWrappedNormal()

    buildAsWrappedNormal(*sample*)

    buildAsWrappedNormal(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on the torus. The sample size must be at least 3.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.WrappedNormal`:
    :math:`(\mu_1,\dots,\mu_n,\sigma_{00},\sigma_{01},\dots,
    \sigma_{n-1,n-1},p)` as the mean vector, the row-major flattened
    covariance matrix and the period.

Returns
-------
dist : :class:`~openturns.experimental.WrappedNormal`
    The estimated distribution as a WrappedNormal.

    In the first usage, the default WrappedNormal distribution is built.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::WrappedNormalFactory::build
R"RAW(Estimate the distribution.

**Available usages**:

    build()

    build(*sample*)

    build(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on the torus.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.WrappedNormal`.

Returns
-------
dist : :class:`~openturns.Distribution`
    The estimated distribution.

    In the first usage, the default WrappedNormal distribution is built.)RAW"