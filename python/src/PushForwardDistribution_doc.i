%feature("docstring") OT::PushForwardDistribution
R"RAW(PushForward distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Its probability density function is the density of :math:`Y = f(X)`
where :math:`X` is an :math:`n`-dimensional absolutely continuous random
vector and :math:`f : \Rset^n \rightarrow \Rset^p` is a deterministic
function.

If :math:`f` is a diffeomorphism between the support of :math:`X` and its
image, the change of variables formula for :math:`p = n` gives a density
with respect to the Lebesgue measure of :math:`\Rset^p`:

.. math::

    f_Y(y) = \frac{f_X(f^{-1}(y))}{|\det J_f(f^{-1}(y))|}

with :math:`J_f` the Jacobian matrix of :math:`f`.

If :math:`p > n`, the image of :math:`f` is expected to be a
:math:`n`-dimensional subset of :math:`\Rset^p` and :math:`Y` is singular
with respect to the Lebesgue measure of :math:`\Rset^p`. Its density is
defined with respect to the measure induced on the image of :math:`f` by
the Lebesgue measure of the tangent space:

.. math::

    f_Y(y) = \frac{f_X(u)}{\sqrt{\det(J_f(u)^T J_f(u))}},
    \quad y = f(u)

In the square case :math:`p = n`, the preimages :math:`u = f^{-1}(y)`
are sought numerically by a damped Newton solver with step control, which
finds all of them: it is comprehensive in dimension one through a
decomposition of the support of :math:`X` into sign-change intervals of the
residual and local minima of its absolute value, and relies on a
low-discrepancy multi-start search in higher dimensions. The density is
then the sum of the contributions of all the preimages, so non-injective
functions are supported:

.. math::

    f_Y(y) = \sum_{u : f(u) = y} \frac{f_X(u)}{|\det J_f(u)|}

In the intrinsic case :math:`p > n`, the preimage :math:`u = f^{-1}(y)`
closest to the mean of :math:`X` is sought numerically with a least
squares solver.

At critical values, where the Jacobian of :math:`f` is singular, the
density is singular and is not evaluated.

The case :math:`p < n` (co-area formula) is not implemented yet.

Parameters
----------
function : :class:`~openturns.Function`, :math:`\Rset^n \rightarrow \Rset^p`
    The function defining the push-forward.
antecedent : :class:`~openturns.Distribution`, absolutely continuous :math:`n`-dimensional
    The distribution of :math:`X`.

See also
--------
openturns.CompositeDistribution

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

Examples
--------
Create the distribution of :math:`Y = 2 X` with :math:`X \sim \mathcal{N}(0, 1)`:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> f = ot.SymbolicFunction(['x'], ['2.0 * x'])
>>> distY = otexp.PushForwardDistribution(f, ot.Normal(0.0, 1.0))
>>> print(distY.computePDF([0.0]))
0.199471...

The distribution of :math:`Y = X^2` with :math:`X \sim \mathcal{N}(0, 1)`:

>>> f = ot.SymbolicFunction(['x'], ['x^2'])
>>> distZ = otexp.PushForwardDistribution(f, ot.Normal(0.0, 1.0))
>>> print(distZ.computePDF([1.0]))
0.2419707...

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
f : :class:`~openturns.Function`, :math:`\Rset^n \rightarrow \Rset^p`
    The function defining the push-forward.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::setFunction
R"RAW(Accessor to the function.

Parameters
----------
f : :class:`~openturns.Function`, :math:`\Rset^n \rightarrow \Rset^p`
    The function defining the push-forward.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::getAntecedent
"Accessor to the antecedent distribution.

Returns
-------
antecedent : :class:`~openturns.Distribution`, :math:`n`-dimensional
    The distribution of :math:`X`."

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::setAntecedent
"Accessor to the antecedent distribution.

Parameters
----------
antecedent : :class:`~openturns.Distribution`, :math:`n`-dimensional
    The distribution of :math:`X`."

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::getSolver
"Solver accessor.

Returns
-------
solver : :class:`~openturns.Solver`
    The solver used to invert the function in PDF computations."

// ---------------------------------------------------------------------

%feature("docstring") OT::PushForwardDistribution::setSolver
"Solver accessor.

Parameters
----------
solver : :class:`~openturns.Solver`
    The solver used to invert the function in PDF computations."