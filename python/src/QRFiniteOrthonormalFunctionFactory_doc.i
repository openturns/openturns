%feature("docstring") OT::QRFiniteOrthonormalFunctionFactory
R"RAW(QR-based finite orthonormal function factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructors:
    QRFiniteOrthonormalFunctionFactory(*functions, distribution*)

    QRFiniteOrthonormalFunctionFactory(*functions, distribution, experiment*)

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    The finite set of scalar functions :math:`(f_i)_{i=1,\ldots,N}` to orthonormalize.
distribution : :class:`~openturns.Distribution`
    The measure :math:`\mu` with respect to which the functions are orthonormalized.
experiment : :class:`~openturns.WeightedExperiment`, optional
    The weighted experiment used to compute the Gram matrix.
    If not provided, a :class:`~openturns.GaussProductExperiment` based on
    :class:`~openturns.Uniform` marginals covering the distribution range is used.

Notes
-----
This class takes a finite set of scalar functions and builds an orthonormal family
with respect to a given multivariate distribution. It proceeds by:

1. Evaluating the initial functions at the nodes of a weighted experiment.
2. Building the weighted design matrix.
3. Computing its QR decomposition.
4. Expressing the orthonormal functions as linear combinations of the initial ones,
   composed with a mapping to the reference domain :math:`[-1, 1]^d`.

The :meth:`buildQuadrature` method implements the Gauss-LP algorithm following
Ryu & Boyd (2015) to build a minimal quadrature rule that exactly integrates
the first :math:`n` orthonormal functions.

References
----------
Ryu, E.K. and Boyd, S.P., "Extensions of Gauss Quadrature Via Linear
Programming", Found Comput Math (2015) 15: 953-971.

See also
--------
openturns.OrthogonalProductPolynomialFactory, openturns.FiniteOrthogonalFunctionFactory

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> # Build initial basis: Hermite polynomials for correlated Normal
>>> dim = 2
>>> R = ot.CorrelationMatrix([[1.0, 0.8], [0.8, 1.0]])
>>> distribution = ot.Normal([0.0]*dim, [1.0]*dim, R)
>>> refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()]*dim)
>>> initialBasis = [refBasis.build(i) for i in range(6)]
>>> factory = otexp.QRFiniteOrthonormalFunctionFactory(initialBasis, distribution)
>>> point = [0.5]*dim
>>> for i in range(3):
...     print(factory.build(i)(point))
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QRFiniteOrthonormalFunctionFactory::getFunctionsCollection
R"RAW(Accessor to the collection of initial functions.

Returns
-------
fctColl : :class:`~openturns.FunctionCollection`
    The collection of scalar functions :math:`(f_i)_{i=1,\ldots,N}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QRFiniteOrthonormalFunctionFactory::setFunctionsCollection
R"RAW(Accessor to the collection of initial functions.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    The collection of functions :math:`(f_i)_{i=1,\ldots,N}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QRFiniteOrthonormalFunctionFactory::getExperiment
R"RAW(Accessor to the weighted experiment.

Returns
-------
experiment : :class:`~openturns.WeightedExperiment`
    The weighted experiment used to compute the Gram matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QRFiniteOrthonormalFunctionFactory::setExperiment
R"RAW(Accessor to the weighted experiment.

Parameters
----------
experiment : :class:`~openturns.WeightedExperiment`
    The weighted experiment used to compute the Gram matrix.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QRFiniteOrthonormalFunctionFactory::getCoefficients
R"RAW(Accessor to the orthonormalization coefficients matrix.

Returns
-------
C : :class:`~openturns.SquareMatrix`
    The matrix :math:`C` such that :math:`\phi_j = \sum_i C_{ij} f_i \circ T`,
    where :math:`\phi_j` are the orthonormal functions, :math:`f_i` the initial
    functions, and :math:`T` the mapping to the reference domain.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::QRFiniteOrthonormalFunctionFactory::buildQuadrature
R"RAW(Build a minimal quadrature rule based on the first n orthonormal functions.

Parameters
----------
n : int
    Number of orthonormal functions the quadrature must integrate exactly.

Returns
-------
nodes : :class:`~openturns.Sample`
    Quadrature nodes of size :math:`M`.
weights : :class:`~openturns.Point`
    Quadrature weights of length :math:`M`.

Notes
-----
This method implements the Gauss-LP algorithm of Ryu & Boyd (2015). It:

1. Computes the moments of the first :math:`n` orthonormal functions against
   the measure using a high-resolution :class:`~openturns.GaussLegendre` rule.
2. Generates a large set of candidate points uniformly distributed in the
   distribution range.
3. Solves a linear program via :class:`~openturns.HiGHS` to obtain a sparse
   quadrature rule that reproduces the moments.
4. Clusters the LP support via greedy merging.
5. Refines the nodes and weights via :class:`~openturns.TNC`.

The algorithm parameters can be configured through the :class:`~openturns.ResourceMap`:

- ``QRFiniteOrthonormalFunctionFactory-AlphaS`` (*int*, default 100): oversampling factor.
- ``QRFiniteOrthonormalFunctionFactory-Ngauss`` (*int*, default 100):
  number of Gauss-Legendre points per dimension for moment computation.
- ``QRFiniteOrthonormalFunctionFactory-Epsilon`` (*float*, default 1e-5):
  convergence tolerance for the refinement step.)RAW"
