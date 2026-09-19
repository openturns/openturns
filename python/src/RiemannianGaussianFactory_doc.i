%feature("docstring") OT::RiemannianGaussianFactory
R"RAW(Riemannian Gaussian factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Given a sample of symmetric positive definite matrices
:math:`\mathbf{X}_1,\dots,\mathbf{X}_N` of dimension :math:`n(n+1)/2`,
estimates the Riemannian Gaussian parameters:

1. The mean is initialized with the arithmetic mean of the sample and
   projected onto the set of symmetric positive definite matrices.

2. The covariance :math:`\boldsymbol{\Sigma}` is estimated as the empirical
   covariance of the log-Euclidean tangent vectors
   :math:`\mathrm{vec}(\log\mathbf{X}_i - \log\overline{\mathbf{X}})`.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- ``RiemannianGaussianFactory-StepSize``: step size used by the estimator.
- ``RiemannianGaussianFactory-Tolerance``: tolerance of the estimator.
- ``RiemannianGaussianFactory-MaximumIteration``: maximum number of
  iterations of the estimator.
- ``RiemannianGaussian-PositiveDefiniteThreshold``: relative tolerance for
  the validation of the positive definiteness of the mean and covariance
  matrices.

See also
--------
openturns.DistributionFactory, RiemannianGaussian

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> factory = otexp.RiemannianGaussianFactory()
>>> mean = ot.SymmetricMatrix([[1.0, 0.0], [0.0, 1.0]])
>>> sigma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
>>> distribution = otexp.RiemannianGaussian(mean, sigma)
>>> estimated = factory.build(distribution.getSample(500))
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiemannianGaussianFactory::buildAsRiemannianGaussian
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsRiemannianGaussian()

    buildAsRiemannianGaussian(*sample*)

    buildAsRiemannianGaussian(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on the manifold of symmetric positive definite matrices. Each row
    stores the upper-triangle flattened :math:`n\times n` matrix
    :math:`(\mathbf{X}_{00},\dots,\mathbf{X}_{0,n-1},\mathbf{X}_{11},\dots,
    \mathbf{X}_{n-1,n-1})`, so the sample dimension must be
    :math:`n(n+1)/2`. The sample size must be at least 3.
param : sequence of float
    The parameters of the
    :class:`~openturns.experimental.RiemannianGaussian`:
    the upper-triangle flattened mean matrix followed by the row-major
    flattened covariance matrix.

Returns
-------
dist : :class:`~openturns.experimental.RiemannianGaussian`
    The estimated distribution as a RiemannianGaussian.

    In the first usage, the default RiemannianGaussian distribution is
    built.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RiemannianGaussianFactory::build
R"RAW(Estimate the distribution.

**Available usages**:

    build()

    build(*sample*)

    build(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on the manifold of symmetric positive definite matrices.
param : sequence of float
    The parameters of the
    :class:`~openturns.experimental.RiemannianGaussian`.

Returns
-------
dist : :class:`~openturns.Distribution`
    The estimated distribution.

    In the first usage, the default RiemannianGaussian distribution is
    built.)RAW"