%feature("docstring") OT::MatrixFisherFactory
R"RAW(Matrix Fisher factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Given a sample of rotation matrices :math:`\mathbf{R}_1,\dots,\mathbf{R}_N`,
estimates the Matrix Fisher parameters using the method of moments:

1. Compute the mean matrix :math:`\mathbf{M} = \frac{1}{N}\sum_{i=1}^N
   \mathbf{R}_i`.

2. Compute the singular values :math:`s_1 \ge s_2 \ge s_3` of
   :math:`\mathbf{M}` along with the singular vectors
   :math:`\mathbf{U},\mathbf{V}`.

3. The concentration matrix is built as
   :math:`\mathbf{F} = \mathbf{U}\,\mathrm{diag}(f_1,f_2,f_3)\mathbf{V}^T`
   where :math:`f_i = 3s_i` when :math:`s_i < 0.5` and
   :math:`f_i = 1/(2(1-s_i))` otherwise.

Notes
-----
The following :class:`openturns.ResourceMap` keys are used:

- ``MatrixFisherFactory-AbsolutePrecision``: absolute precision of the
  estimator.
- ``MatrixFisher-OrthogonalityThreshold``: relative tolerance for the
  validation of the orthogonality of the sampled matrices.

See also
--------
openturns.DistributionFactory, MatrixFisher

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> factory = otexp.MatrixFisherFactory()
>>> F = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 0.5, 0.0], [0.0, 0.0, 0.1]])
>>> estimated = factory.build(otexp.MatrixFisher(F).getSample(1000))
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisherFactory::buildAsMatrixFisher
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsMatrixFisher()

    buildAsMatrixFisher(*sample*)

    buildAsMatrixFisher(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on :math:`SO(3)`: each row stores the flattened
    :math:`3\times 3` rotation matrix in row order. The dimension must be 9
    and the sample size at least 3.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.MatrixFisher`:
    :math:`(f_{00},f_{01},\dots,f_{22})` as the row-major flattened
    :math:`3\times 3` parameter matrix :math:`\mathbf{F}`.

Returns
-------
dist : :class:`~openturns.experimental.MatrixFisher`
    The estimated distribution as a MatrixFisher.

    In the first usage, the default MatrixFisher distribution is built.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MatrixFisherFactory::build
R"RAW(Estimate the distribution.

**Available usages**:

    build()

    build(*sample*)

    build(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on :math:`SO(3)` as flattened :math:`3\times 3` rotation matrices.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.MatrixFisher`.

Returns
-------
dist : :class:`~openturns.Distribution`
    The estimated distribution.

    In the first usage, the default MatrixFisher distribution is built.)RAW"