%feature("docstring") OT::BinghamFactory
R"RAW(Bingham factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Given a sample :math:`\mathbf{x}_1,\dots,\mathbf{x}_N` on the sphere
:math:`\mathcal{S}^{n-1}`, estimates the Bingham distribution parameters using
a moment matching algorithm:

1. Compute the scatter matrix
   :math:`\mathbf{S} = \frac{1}{N}\sum_{i=1}^N \mathbf{x}_i\mathbf{x}_i^T`.

2. Diagonalize :math:`\mathbf{S}`. The eigenvectors give the orientation
   matrix :math:`\boldsymbol{\Gamma}` and the eigenvalues
   :math:`\lambda_1 \ge \dots \ge \lambda_n` give the starting concentration
   parameters through the approximation
   :math:`\zeta_i \approx \frac{n}{2}(\lambda_i - 1/n)`.

3. Shift the concentrations so that :math:`\min_i\zeta_i = 0`.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- ``BinghamFactory-AbsolutePrecision``: absolute precision of the estimator.
- ``Bingham-MaximumIteration``: maximum number of iterations.
- ``Bingham-OrthogonalityThreshold``: relative tolerance for the validation
  of the orthogonality of the basis matrix.

See also
--------
openturns.DistributionFactory, Bingham

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> factory = otexp.BinghamFactory()
>>> gamma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
>>> distribution = otexp.Bingham([0.5, 0.2, 0.0], gamma)
>>> estimated = factory.build(distribution.getSample(1000))
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BinghamFactory::buildAsBingham
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsBingham()

    buildAsBingham(*sample*)

    buildAsBingham(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on the sphere :math:`\mathcal{S}^{n-1}`. The dimension must be at
    least 2 and the sample size at least :math:`n+1`.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.Bingham`:
    :math:`(\zeta_1,\dots,\zeta_n,\gamma_{00},\gamma_{01},\dots,\gamma_{n-1,n-1})`
    as the concentration parameters followed by the row-major flattened
    orthogonal matrix.

Returns
-------
dist : :class:`~openturns.experimental.Bingham`
    The estimated distribution as a Bingham.

    In the first usage, the default Bingham distribution is built.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::BinghamFactory::build
R"RAW(Estimate the distribution.

**Available usages**:

    build()

    build(*sample*)

    build(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on the sphere :math:`\mathcal{S}^{n-1}`.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.Bingham`.

Returns
-------
dist : :class:`~openturns.Distribution`
    The estimated distribution.

    In the first usage, the default Bingham distribution is built.)RAW"