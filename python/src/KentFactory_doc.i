%feature("docstring") OT::KentFactory
R"RAW(Kent factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Given a sample :math:`\mathbf{x}_1,\dots,\mathbf{x}_n` on the sphere
:math:`\mathcal{S}^2`, estimates the Kent distribution parameters using the
method of moments [kent1982]_:

1. Compute the sample mean :math:`\overline{\mathbf{x}}` and the scatter
   matrix :math:`\mathbf{S} = \frac{1}{n}\sum_{i=1}^n \mathbf{x}_i\mathbf{x}_i^T`.

2. Build an orthogonal matrix :math:`\mathbf{H}` that maps the mean direction
   to the first axis:
   :math:`\mathbf{H}\overline{\mathbf{x}} = (\|\overline{\mathbf{x}}\|, 0, 0)^T`.

3. Rotate the scatter matrix: :math:`\mathbf{B} = \mathbf{H}\mathbf{S}\mathbf{H}^T`.

4. Diagonalize the lower-right :math:`2\times 2` block of :math:`\mathbf{B}` to
   obtain the angle :math:`\psi`:
   :math:`\tan(2\psi) = 2b_{23} / (b_{22} - b_{33})` with :math:`b_{ij}` the
   entries of the :math:`\vec{B}` matrix.

5. The estimated orientation is :math:`\boldsymbol{\Gamma} = \mathbf{H}^T\mathbf{K}^T`
   where :math:`\mathbf{K}` is the rotation about the first axis by :math:`\psi`.

6. The moment statistics are:
   :math:`r_1 = \|\overline{\mathbf{x}}\|` and
   :math:`r_2 = t_{22} - t_{33}` where
   :math:`\mathbf{T} = \boldsymbol{\Gamma}^T\mathbf{S}\boldsymbol{\Gamma}`.

7. The concentration parameters are estimated from the large-kappa approximation:
   :math:`\hat\kappa = \frac{1}{2-2r_1-r_2} + \frac{1}{2-2r_1+r_2}`,
   :math:`\hat\beta = \frac12\left(\frac{1}{2-2r_1-r_2} - \frac{1}{2-2r_1+r_2}\right)`.

8. These estimates are used as the starting point for Newton-Raphson iterations
   in order to solve the system of nonlinear equations of the method of moments.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``KentFactory-AbsolutePrecision``: absolute precision for the Newton-Raphson
  solver.
- ``Kent-MaximumIteration``: maximum number of iterations for the
  Newton-Raphson solver.

See also
--------
openturns.DistributionFactory, Kent

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> factory = otexp.KentFactory()
>>> gamma = ot.IdentityMatrix(3)
>>> sample = otexp.Kent(25.0, 0.5, gamma).getSample(1000)
>>> estimated = factory.build(sample)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::KentFactory::buildAsKent
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsKent()

    buildAsKent(*sample*)

    buildAsKent(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on the sphere :math:`\mathcal{S}^2`. The sample size must be at
    least 3 and the dimension must be 3.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.Kent`:
    :math:`(\kappa, \beta, \gamma_{00}, \gamma_{10}, \gamma_{20}, \gamma_{01},
    \gamma_{11}, \gamma_{21}, \gamma_{02}, \gamma_{12}, \gamma_{22})`.

Returns
-------
dist : :class:`~openturns.experimental.Kent`
    The estimated distribution as a Kent.

    In the first usage, the default Kent distribution is built.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::KentFactory::build
R"RAW(Estimate the distribution.

**Available usages**:

    build()

    build(*sample*)

    build(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data on the sphere :math:`\mathcal{S}^2`.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.Kent`.

Returns
-------
dist : :class:`~openturns.Distribution`
    The estimated distribution.

    In the first usage, the default Kent distribution is built.)RAW"
