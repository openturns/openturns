%feature("docstring") OT::CompositeDistribution
R"RAW(Composite distribution.

Helper class for defining the push-forward distribution of a given univariate
distribution by a given scalar function.

We note :math:`X` a scalar random variable which distribution is :math:`distX`,
which probability density function is :math:`f_X`.

Then :math:`distY` is the distribution of the scalar random variable
:math:`Y=g(X)`, which probability density function :math:`f_Y` is defined as:

.. math::

    \displaystyle f_Y(y) = \sum_{k =0}^{k=N} \frac{f_X (g^{-1}(y))}{|g'\circ g^{-1}(y)|}1_{y \in g([a_k, a_{k+1}))}

with :math:`a_0=\inf \supp{f_X}`, :math:`a_N=\sup \supp{f_X}` and
:math:`(a_1, \dots, a_N)` such that :math:`g` is monotone over
:math:`[a_k, a_{k+1})` for :math:`0 \leq k \leq N`.


Available constructors:
    CompositeDistribution(*g, distX*)

    CompositeDistribution(*g, distX, a, v*)

Parameters
----------
g : :class:`~openturns.Function`, :math:`\Rset \rightarrow \Rset`
distX : :class:`~openturns.Distribution`, univariate
a : sequence of float of dimension :math:`N+1`, :math:`a[0]=\inf \supp{distX}`, :math:`a[N]=\sup \supp{distX}`
    The bounds of the intervals on which :math:`g` is monotone, sorted in
    ascending order.
v : sequence of float of dimension :math:`N+1`,
    The values taken by :math:`g` on each bound: :math:`v[k]=g(a[k])`.

Returns
-------
distY : :class:`~openturns.Distribution`, univariate
    :math:`distY` is the push-forward distribution of :math:`distX` by :math:`g`.

Notes
-----
Its first moments are obtained by numerical integration.
The mathematical support of a CompositeDistribution is defined as the image of the numerical support of the antecedent.
Consequently, its bounds are finite.

The following :class:`~openturns.ResourceMap` keys are used:

- ``CompositeDistribution-SolverEpsilon`` (``Scalar``, default: ``1e-14``): tolerance of the non linear solver used to compute the values of the distribution.
- ``CompositeDistribution-StepNumber`` (``UnsignedInteger``, default: ``256``): number of steps of the numerical integration.

The two-argument constructor determines the monotonicity partition
:math:`(a_0,\ldots,a_N)` automatically, by solving
:math:`g'(x)=0` on the support of :math:`distX`. The four-argument
constructor lets the user specify this partition, when the user knows a priori
the monotonicity intervals of :math:`g`, or when :math:`g` is not smooth enough
for its derivatives to be computed numerically by the automatic procedure.

The roots of :math:`g'` are located by the solver on a regular grid of
:math:`CompositeDistribution-StepNumber` subintervals of the support of
:math:`distX` (defaults to 256). The solver and its tolerances can be changed
with :meth:`setSolver`, using as default absolute tolerance
:math:`CompositeDistribution-SolverEpsilon` rescaled by the antecedent range
width.

The range of :math:`distY` is the interval between the minimum and the maximum
of the values :math:`v[k]`.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> g = ot.SymbolicFunction(['x'], ['sin(x) + cos(x)'])
>>> distY = ot.CompositeDistribution(g, ot.Normal(1.0, 0.5))

>>> g = ot.SymbolicFunction(['x'], ['abs(x)'])
>>> a = [-1.0, 0.0, 2.0]
>>> v = [1.0, 0.0, 2.0]
>>> distZ = ot.CompositeDistribution(g, ot.Uniform(-1.0, 2.0), a, v)

>>> distX = ot.Normal(0.0, 1.0)
>>> a0 = distX.getRange().getLowerBound()
>>> aN = distX.getRange().getUpperBound()
>>> a = [a0[0], 0.0, 0.0, aN[0]]
>>> g = ot.SymbolicFunction(['x'], ['1.0/x'])
>>> v = [g(a0)[0], -ot.SpecFunc.Infinity, ot.SpecFunc.Infinity, g(aN)[0]]
>>> distT = ot.CompositeDistribution(g, distX, a, v)


Draw a sample:

>>> sample = distT.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompositeDistribution::getFunction
R"RAW(Accessor to the function.

Returns
-------
g :  :class:`~openturns.Function`, :math:`\Rset \rightarrow \Rset`
    the function :math:`g`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompositeDistribution::getAntecedent
"Accessor to the antecedent distribution.

Returns
-------
distX : :class:`~openturns.Distribution`, univariate
    Antecedent distribution :math:`distX`."

// ---------------------------------------------------------------------

%feature("docstring") OT::CompositeDistribution::setFunction
R"RAW(Fix the function through which the distribution is push-forwarded.

Parameters
----------
g :  :class:`~openturns.Function`, :math:`\Rset \rightarrow \Rset`
    the function :math:`g`.)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::CompositeDistribution::setAntecedent
"Fix the antecedent distribution which is push-forwarded.

Parameters
----------
distX : :class:`~openturns.Distribution`, univariate
   Distribution of the antecedent :math:`distX`."

// ---------------------------------------------------------------------

%feature("docstring") OT::CompositeDistribution::setSolver
"Solver accessor.

Parameters
----------
solver : :class:`~openturns.Solver`
    The solver used for PDF/CDF computations.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompositeDistribution::getSolver
"Solver accessor.

Returns
-------
solver : :class:`~openturns.Solver`
    The solver used for PDF/CDF computations.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompositeDistribution::computeProbability
R"RAW(Compute the interval probability.

Parameters
----------
interval : :class:`~openturns.Interval`
    An interval in :math:`\Rset`.

Returns
-------
p : float
    The probability of *interval*.

Notes
-----
This method is implemented as a segment-wise summation over the monotonicity
intervals of :math:`g`, using the precomputed CDF values of :math:`distX` at
the bounds of these intervals. It therefore avoids the numerical integration
of the PDF over the singular points located at the images of the critical
points of :math:`g`, where the PDF is not defined, an approach that would
otherwise diverge.

Examples
--------
>>> import openturns as ot
>>> distX = ot.Normal()
>>> distY = ot.CompositeDistribution(ot.SymbolicFunction(['x'], ['x^2']), distX)
>>> distY.computeProbability(ot.Interval([0.0], [1.0]))  # doctest: +SKIP
0.6826894921370856
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CompositeDistribution::getSingularities
R"RAW(Accessor to the PDF singularities.

Returns
-------
singularities : :class:`~openturns.Point`
    The singularities of the PDF of the distribution, sorted in ascending order.

Notes
-----
The PDF of :math:`g(distX)` is not defined at the images of the critical
points of :math:`g`, i.e. at the points :math:`g(x)` where
:math:`g'(x)=0`. These points are returned by this method. A monotonic
function :math:`g` has no such singularity, and the returned Point is empty.

Examples
--------
>>> import openturns as ot
>>> distY = ot.CompositeDistribution(ot.SymbolicFunction(['x'], ['x^2']), ot.Normal())
>>> distY.getSingularities()  # doctest: +SKIP
class=Point name=Unnamed dimension=1 values=[0]
)RAW"
