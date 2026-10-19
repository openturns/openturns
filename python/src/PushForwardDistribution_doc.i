%feature("docstring") OT::PushForwardDistribution
R"RAW(PushForward distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Its probability density function is the density of :math:`\outputRV = f(\inputRV)`
where :math:`\inputRV` is an :math:`\inputDim`-dimensional absolutely continuous random
vector and :math:`f: \Rset^\inputDim \rightarrow \Rset^\outputDim` is a deterministic
function.

If :math:`f` is a diffeomorphism between the support of :math:`\inputRV` and its
image, the change of variables formula for :math:`\outputDim = \inputDim` gives a density
with respect to the Lebesgue measure of :math:`\Rset^\outputDim`:

.. math::

    f_{\outputRV}(\outputReal) = \frac{f_{\inputRV}(f^{-1}(\outputReal))}{|\det J_f(f^{-1}(\outputReal))|}

with :math:`J_f` the Jacobian matrix of :math:`f`.

If :math:`\outputDim > \inputDim`, the image of :math:`f` is expected to be an
:math:`\inputDim`-dimensional subset of :math:`\Rset^\outputDim` and :math:`\outputRV` is singular
with respect to the Lebesgue measure of :math:`\Rset^\outputDim`. Its density is
defined with respect to the measure induced on the image of :math:`f` by
the Lebesgue measure of the tangent space:

.. math::

    f_{\outputRV}(\outputReal) = \frac{f_{\inputRV}(\inputReal)}{\sqrt{\det(J_f(\inputReal)^T J_f(\inputReal))}},
    \quad \outputReal = f(\inputReal)

In the square case :math:`\outputDim = \inputDim`, the preimages :math:`\inputReal = f^{-1}(\outputReal)`
are sought numerically by a damped Newton solver with step control, which
finds all of them: it attempts to be comprehensive in dimension one through a
decomposition of the support of :math:`\inputRV` into sign-change intervals of the
residual and local minima of its absolute value, and relies on a
low-discrepancy multi-start search in higher dimensions. The density is
then the sum of the contributions of all the preimages, so non-injective
functions are supported:

.. math::

    f_{\outputRV}(\outputReal) = \sum_{\inputReal : f(\inputReal) = \outputReal} \frac{f_{\inputRV}(\inputReal)}{|\det J_f(\inputReal)|}

In the intrinsic case :math:`\outputDim > \inputDim`, the preimages :math:`\inputReal = f^{-1}(\outputReal)`
are sought numerically with a multi-start least squares solver over the
finite search box plus the mean of :math:`\inputRV`, deduplicated by
distance, and their contributions are summed.

At critical values, where the Jacobian of :math:`f` is singular, the
density diverges where the antecedent carries mass and `computePDF`
returns `+inf`.

In the co-area case :math:`\outputDim < \inputDim`, the preimages of
:math:`\outputReal` are not isolated points but the
:math:`(\inputDim - \outputDim)`-dimensional fibre
:math:`\{\inputReal : f(\inputReal) = \outputReal\}`, and the density is the
integral of the antecedent density along it, divided by the
:math:`\outputDim`-dimensional Jacobian of :math:`f`:

.. math::

    f_{\outputRV}(\outputReal) = \int_{f^{-1}(\outputReal)} \frac{f_{\inputRV}(\inputReal)}{\sqrt{\det(J_f(\inputReal)^T J_f(\inputReal))}}
    \, \mathrm{d} H^{\inputDim - \outputDim}(\inputReal)

where :math:`J_f` is the :math:`\inputDim \times \outputDim` Jacobian matrix of
:math:`f` and :math:`H^{\inputDim - \outputDim}` is the Hausdorff measure of
dimension :math:`\inputDim - \outputDim` on the fibre. This case is implemented
for an affine function :math:`f(\inputReal) = \mat{A}\inputReal + \vect{c}` with a one
dimensional fibre, i.e. :math:`\outputDim = \inputDim - 1`. The fibre is then a
straight line through the point closest to the mean of :math:`\inputRV`, directed
by a unit vector of the kernel of :math:`\mat{A}`: with such a parameterization the
Hausdorff element along the fibre is exactly the parameter element, and the
density reduces to a one dimensional integral, evaluated by an adaptive
Gauss-Kronrod rule (see the ``PushForwardDistribution-CoAreaMaximumError`` and
``PushForwardDistribution-CoAreaMaximumSubIntervals`` keys). Whether :math:`f` is
affine is checked on the faces and on the corners of the search box, which bound
the region where the fibre may be integrated, and again at each quadrature node
along the fibre: a curved fibre raises an exception instead of returning a wrong
density. The integration is carried out
over the intersection of the fibre with the search box of the distribution,
extended while the integrand is not negligible there: the neglected mass at the
ends of the fibre is then at most of the order of
``Distribution-DefaultQuantileEpsilon`` for an antecedent with an unbounded
support, and exactly zero for an antecedent with a bounded one.

Parameters
----------
function : :class:`~openturns.Function`, :math:`\Rset^\inputDim \rightarrow \Rset^\outputDim`
    The function defining the push-forward.
antecedent : :class:`~openturns.Distribution`, absolutely continuous :math:`\inputDim`-dimensional
    The distribution of :math:`\inputRV`.

See also
--------
:class:`~openturns.CompositeDistribution`

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``PushForwardDistribution-SolverResidualTolerance`` (``Scalar``, default: ``1.0e-5``): tolerance on the
  residual norm used to accept a point as a preimage of the searched point and to configure the default solver.
- ``PushForwardDistribution-SampleSize`` (``UnsignedInteger``, default: ``10000``): number of Monte-Carlo
  realizations used to estimate the range, the mean and the covariance.
- ``PushForwardDistribution-PreimageSearchSampleSize`` (``UnsignedInteger``, default: ``256``): number of
  points used to decompose the search interval and to seed the damped Newton solver.
- ``PushForwardDistribution-NewtonMaximumIterations`` (``UnsignedInteger``, default: ``50``): maximum number
  of iterations of the damped Newton solver.
- ``PushForwardDistribution-NewtonStepReduction`` (``Scalar``, default: ``0.5``): reduction factor applied
  to the step when it fails to decrease the residual norm.
- ``PushForwardDistribution-CoAreaResidualTolerance`` (``Scalar``, default: ``1.0e-8``): relative tolerance
  on the residual norm of the co-area formula, used to check that the fibre of the evaluated point is a
  straight line and hence that the function is affine.
- ``PushForwardDistribution-CoAreaMaximumError`` (``Scalar``, default: ``1.0e-14``): requested relative error
  of the integration of the density along the fibre, scaled by the magnitude of the integrand so that the
  relative accuracy does not degrade for small densities.
- ``PushForwardDistribution-CoAreaMaximumSubIntervals`` (``UnsignedInteger``, default: ``100``): maximum
  number of sub-intervals of the adaptive bisection used to integrate the density along the fibre.

Examples
--------
Create the distribution of :math:`\outputRV = 2 \inputRV` with :math:`\inputRV \sim \cN(0, 1)`:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> f = ot.SymbolicFunction(['x'], ['2.0 * x'])
>>> distY = otexp.PushForwardDistribution(f, ot.Normal(0.0, 1.0))
>>> print(distY.computePDF([0.0]))
0.199471...

The distribution of :math:`\outputRV = \inputRV^2` with :math:`\inputRV \sim \cN(0, 1)`:

>>> f = ot.SymbolicFunction(['x'], ['x^2'])
>>> distZ = otexp.PushForwardDistribution(f, ot.Normal(0.0, 1.0))
>>> print(distZ.computePDF([1.0]))
0.2419707...

Create the distribution of :math:`\outputRV = (\inputRV_0^2, \inputRV_1^2)`
with :math:`\inputRV \sim \cN(0, \vect{I}_2)`:

>>> f = ot.SymbolicFunction(['x0', 'x1'], ['x0^2', 'x1^2'])
>>> dist2 = otexp.PushForwardDistribution(f, ot.Normal(2))
>>> print(dist2.computePDF([1.0, 1.0]))
0.0585498...

Create the distribution of :math:`\outputRV = (\inputRV_0, \inputRV_1)`
with :math:`\inputRV \sim \cN(0, \vect{I}_3)`, whose density is the bivariate
marginal obtained by integrating along the fibre:

>>> f = ot.SymbolicFunction(['x0', 'x1', 'x2'], ['x0', 'x1'])
>>> distCoArea = otexp.PushForwardDistribution(f, ot.Normal(3))
>>> print(distCoArea.computePDF([0.0, 0.0]))
0.159154943...

Create the distribution of a random point following a normal distribution
along the diagonal of :math:`\Rset^2`:

>>> f = ot.SymbolicFunction(['x'], ['x', 'x'])
>>> distD = otexp.PushForwardDistribution(f, ot.Normal(0.0, 1.0))

Draw a sample:

>>> sample = distY.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::getFunction
R"RAW(Accessor to the function.

Returns
-------
f : :class:`~openturns.Function`, :math:`\Rset^\inputDim \rightarrow \Rset^\outputDim`
    The function defining the push-forward.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::setFunction
R"RAW(Accessor to the function.

Parameters
----------
f : :class:`~openturns.Function`, :math:`\Rset^\inputDim \rightarrow \Rset^\outputDim`
    The function defining the push-forward.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::getAntecedent
R"RAW(Accessor to the antecedent distribution.

Returns
-------
antecedent : :class:`~openturns.Distribution`, :math:`\inputDim`-dimensional
    The distribution of :math:`\inputRV`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::setAntecedent
R"RAW(Accessor to the antecedent distribution.

Parameters
----------
antecedent : :class:`~openturns.Distribution`, :math:`\inputDim`-dimensional
    The distribution of :math:`\inputRV`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::getSolver
R"RAW(Solver accessor.

Returns
-------
solver : :class:`~openturns.Solver`
    The solver used to invert the function in PDF computations.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::setSolver
R"RAW(Solver accessor.

Parameters
----------
solver : :class:`~openturns.Solver`
    The solver used to invert the function in PDF computations.)RAW"
