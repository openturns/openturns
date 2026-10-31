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
co-area density may diverge. When an implemented nonlinear path detects a
rank deficient Jacobian at a point where the antecedent carries mass,
`computePDF` returns `+inf`. Critical values whose density has a finite
limit are not resolved reliably by the mesh extraction.

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
dimension :math:`\inputDim - \outputDim` on the fibre. The density in this
case is implemented for an affine function
:math:`f(\inputReal) = \mat{A}\inputReal + \vect{c}`, with
a fibre of dimension :math:`\inputDim - \outputDim \geq 1`. The fibre is
parameterized as :math:`\inputReal(\vect{t}) = \vect{x}_0 + \mat{W}\vect{t}`,
where :math:`\vect{x}_0` is the point of the fibre closest to the mean of
:math:`\inputRV` and the columns of :math:`\mat{W}` form an orthonormal basis of
the kernel of :math:`\mat{A}`. With such a parameterization the Hausdorff
element along the fibre is exactly the parameter element
:math:`\mathrm{d}\vect{t}`, and the density reduces to an integral on the
intersection of the fibre with the search box of the distribution.

For a one dimensional fibre, i.e. :math:`\outputDim = \inputDim - 1`, this
intersection is an interval: the density is a one dimensional integral,
evaluated by an adaptive Gauss-Kronrod rule (see the
``PushForwardDistribution-CoAreaMaximumError`` and
``PushForwardDistribution-CoAreaMaximumSubIntervals`` keys), over the interval
extended while the integrand is not negligible there, so that the neglected
mass at its ends is at most of the order of
``Distribution-DefaultQuantileEpsilon`` for an antecedent with an unbounded
support, and exactly zero for an antecedent with a bounded one.

For a fibre of dimension two or more, the intersection is a convex polytope
described by the :math:`2\inputDim` constraints
:math:`\vect{l} \leq \vect{x}_0 + \mat{W}\vect{t} \leq \vect{u}`. Its exact
nested interval description is obtained by Fourier-Motzkin elimination of these
constraints, and the density is evaluated by an iterated Gauss-Kronrod rule on
the resulting levels. The kinks of the nested bounds limit the accuracy of the
adaptive rule: for a fibre of dimension three or more the requested relative
error is floored at the value of the
``PushForwardDistribution-CoAreaErrorFloor`` key, the order of the measured
accuracy floor, whereas the one and two dimensional fibres reached about
:math:`3\times 10^{-16}` on the same smooth test integrand. The
cost grows quickly with the fibre dimension, as each level of the iterated
integration evaluates the next one at every quadrature node, and the whole
search box is integrated, without the one dimensional extension: the neglected
mass is then at most of the order of ``Distribution-DefaultQuantileEpsilon`` for
an antecedent with an unbounded support, and exactly zero for a bounded one.

The affinity of the co-area density is enforced structurally by
:meth:`~openturns.Function.isLinear` when ``computePDF()`` is called: a
function that does not declare its linearity never uses the affine density
path, instead of being probed on a finite set of points where a nonlinear
function could hide its curvature. A custom or Python function must therefore
override ``isLinear()`` to use the exact affine path. The nonlinear co-area
density is implemented for a scalar function
:math:`f: \Rset^2 \rightarrow \Rset`,
:math:`f: \Rset^3 \rightarrow \Rset`, and for a function
:math:`f: \Rset^n \rightarrow \Rset^{n-1}` with :math:`n \geq 3`, i.e.
:math:`\Rset^3 \rightarrow \Rset^2`, :math:`\Rset^4 \rightarrow \Rset^3`
and beyond. The search box is triangulated into :math:`n`-simplices. For a
scalar function, the signs of :math:`f - y` at the vertices locate the crossed
edges, each crossing is polished with a scalar Brent solve on the actual
function, and the density is integrated along the resulting level segments
(:math:`\inputDim = 2`) or over the triangulated level surface
(:math:`\inputDim = 3`). For :math:`f: \Rset^n \rightarrow \Rset^{n-1}`,
each component of :math:`f` is replaced by its P1 interpolation in a simplex;
the intersection of the :math:`n - 1` level hyperplanes is a segment, obtained
from the null space of the barycentric system and clipped to the simplex. Its
two ends are polished on the actual function, and the density is integrated
along the segments. Once the mesh has supplied the topology and one seed per
connected component, the regular one dimensional fibres are refined by a
predictor-corrector continuation: an arc-length ordinary differential equation
with unit tangent is integrated with Runge-Kutta steps, each step being
followed by a Newton projection on the actual level set. This removes most of
the mesh-size bias of the polygonal approximation, while branching and saddle
fibres keep the mesh result as a fallback and singular fibres return an
infinite density. The continuation is seeded from a coarser triangulation and
integrated with the step size of the requested resolution, so that the mesh
resolution mainly controls the topology discovery and the fallback accuracy.
The continuation is controlled by the
``PushForwardDistribution-NonlinearCoAreaContinuation`` key.
The co-area factor is the norm of the gradient for a scalar function,
evaluated at the segment midpoint in :math:`\Rset^2` and at the surface
quadrature points in :math:`\Rset^3`. For
:math:`f: \Rset^n \rightarrow \Rset^{n-1}` it is the :math:`(n - 1)`
dimensional Jacobian, i.e. the norm of the generalized cross product of the
:math:`n - 1` gradient vectors, evaluated at the segment quadrature points.
The nonlinear paths fall back to centered finite differences when the function
provides no analytic gradient. The mesh resolution is controlled by the
``PushForwardDistribution-NonlinearCoAreaDiscretization`` key in
:math:`\Rset^2`, by the
``PushForwardDistribution-NonlinearCoAreaSurfaceDiscretization`` key for the
:math:`\Rset^3` paths, and by the
``PushForwardDistribution-NonlinearCoAreaVolumeDiscretization`` key for
:math:`\Rset^n \rightarrow \Rset^{n-1}` with :math:`n \geq 4`. These
densities are approximations, whereas the affine path remains exact. Other
nonlinear maps with :math:`\outputDim < \inputDim` are accepted by the
constructor, can be sampled, and have their range, mean and covariance
computed, but their density is not implemented and ``computePDF()`` raises
*NotYetImplementedException*.

When the Jacobian is rank deficient, the image of the function has measure zero
in the output space. Membership of the image is tested through the residual of
the full SVD in the output directions that the Jacobian does not reach: a
relative residual above the CDF epsilon of the distribution (by default
``Distribution-DefaultCDFEpsilon``) is treated as outside the image and yields
a zero density, whereas a point on the image yields the singular density,
provided that the affine fibre meets the support of the antecedent. When the
fibre misses that support, the density is zero even though the point belongs
to the algebraic image.

The mean and the covariance are available in closed form for an affine
function:
:math:`\Expect{\mat{A}\inputRV + \vect{c}} = \mat{A}\Expect{\inputRV} + \vect{c}`
and :math:`\Cov{f(\inputRV)} = \mat{A}\Cov{\inputRV}\mat{A}^T`. For a nonlinear
function, they are integrated over the finite search box with an iterated
quadrature rule when its input dimension does not exceed
``PushForwardDistribution-MomentsMaximumDimension``. Above this threshold the
iterated quadrature becomes too expensive and the moments are estimated from a
``PushForwardDistribution-SampleSize`` sample instead. This includes nonlinear
maps with :math:`\outputDim < \inputDim`: their moments are available,
including the cases whose density is not implemented. The range is still
estimated from a sample, as it has no closed form in general.

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

.. list-table::
   :widths: 30 15 15 40
   :header-rows: 1

   * - Key
     - Type
     - Default value
     - Description
   * - ``PushForwardDistribution-SolverResidualTolerance``
     - ``Scalar``
     - ``1.0e-5``
     - Tolerance on the residual norm used to accept a point as a preimage of the searched point and to configure the default solver.
   * - ``PushForwardDistribution-SampleSize``
     - ``UnsignedInteger``
     - ``10000``
     - Number of Monte-Carlo realizations used to estimate the range and the moments of nonlinear maps above ``PushForwardDistribution-MomentsMaximumDimension``.
   * - ``PushForwardDistribution-MomentsMaximumDimension``
     - ``UnsignedInteger``
     - ``4``
     - Maximum input dimension for which the mean and the covariance of a nonlinear map are integrated deterministically.
   * - ``PushForwardDistribution-PreimageSearchSampleSize``
     - ``UnsignedInteger``
     - ``256``
     - Number of points used to decompose the search interval and to seed the damped Newton solver.
   * - ``PushForwardDistribution-NewtonMaximumIterations``
     - ``UnsignedInteger``
     - ``50``
     - Maximum number of iterations of the damped Newton solver.
   * - ``PushForwardDistribution-NewtonStepReduction``
     - ``Scalar``
     - ``0.5``
     - Reduction factor applied to the step when it fails to decrease the residual norm.
   * - ``PushForwardDistribution-MaximumBisections``
     - ``UnsignedInteger``
     - ``64``
     - Maximum number of bisection iterations used as a fallback when the damped Newton solver fails on a sign change.
   * - ``PushForwardDistribution-CoAreaConstraintTolerance``
     - ``Scalar``
     - ``1.0e-12``
     - Tolerance used to detect parallel constraints and numerically zero coefficients in the Fourier-Motzkin elimination.
   * - ``PushForwardDistribution-CoAreaTailEpsilon``
     - ``Scalar``
     - ``1.0e-16``
     - Relative density below which the end point of a one dimensional fibre is considered negligible when the integration interval is extended over the search box.
   * - ``PushForwardDistribution-CoAreaMaximumError``
     - ``Scalar``
     - ``1.0e-14``
     - Requested relative error of the integration of the density along the fibre, scaled by the magnitude of the integrand so that the relative accuracy does not degrade for small densities.
   * - ``PushForwardDistribution-CoAreaErrorFloor``
     - ``Scalar``
     - ``1.0e-7``
     - Floor applied to the requested relative error for a fibre of dimension three or more, the order of the accuracy reached by the adaptive rule across the kinks of the nested integration bounds.
   * - ``PushForwardDistribution-CoAreaMaximumSubIntervals``
     - ``UnsignedInteger``
     - ``100``
     - Maximum number of sub-intervals of the adaptive bisection used to integrate the density along the fibre.
   * - ``PushForwardDistribution-CoAreaMaximumExtensions``
     - ``UnsignedInteger``
     - ``16``
     - Maximum number of times the one dimensional integration interval is extended over the search box while the integrand is still significant at its end.
   * - ``PushForwardDistribution-NonlinearCoAreaDiscretization``
     - ``UnsignedInteger``
     - ``128``
     - Number of intervals per input dimension in the fallback triangulation of the search box used by the scalar nonlinear co-area density in :math:`\Rset^2`. When the continuation is enabled, a quarter-resolution triangulation seeds it first and this resolution is only built if the continuation fails.
   * - ``PushForwardDistribution-NonlinearCoAreaSurfaceDiscretization``
     - ``UnsignedInteger``
     - ``48``
     - Number of intervals per input dimension in the tetrahedral background mesh used by the nonlinear co-area densities from :math:`\Rset^3` to :math:`\Rset` and from :math:`\Rset^3` to :math:`\Rset^2`.
   * - ``PushForwardDistribution-NonlinearCoAreaVolumeDiscretization``
     - ``UnsignedInteger``
     - ``12``
     - Number of intervals per input dimension in the simplicial background mesh used by the nonlinear co-area density from :math:`\Rset^n` to :math:`\Rset^{n-1}` with :math:`n \geq 4`.
   * - ``PushForwardDistribution-NonlinearCoAreaLevelTolerance``
     - ``Scalar``
     - ``1.0e-12``
     - Relative tolerance used to decide that a mesh vertex lies on the requested level set in the nonlinear co-area paths.
   * - ``PushForwardDistribution-NonlinearCoAreaContinuation``
     - ``Bool``
     - ``True``
     - Refine the regular one dimensional nonlinear co-area fibres with a predictor-corrector continuation seeded by the background mesh; branching and saddle fibres fall back to the mesh result, singular fibres return an infinite density.
   * - ``PushForwardDistribution-RangeMargin``
     - ``Scalar``
     - ``0.025``
     - Relative margin added to each marginal span when the numerical range is estimated from a sample.
   * - ``PushForwardDistribution-DegenerateSearchIntervalHalfWidth``
     - ``Scalar``
     - ``1.0``
     - Half width of the fallback search box used when the quantiles of a marginal collapse to a zero-width interval.

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

Create the distribution of the single coordinate :math:`\inputRV_0` of a
three dimensional normal vector, whose two dimensional fibre is integrated
by the nested interval method:

>>> f = ot.SymbolicFunction(['x0', 'x1', 'x2'], ['x0'])
>>> distFibre = otexp.PushForwardDistribution(f, ot.Normal(3))
>>> print(distFibre.computePDF([0.5]))
0.352065326...

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
