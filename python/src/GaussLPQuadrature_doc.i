%feature("docstring") OT::GaussLPQuadrature
"Gauss-LP quadrature algorithm.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Notes
-----
Builds a minimal quadrature rule that exactly integrates the first :math:`n`
orthonormal functions against the measure. The algorithm proceeds by:

1. Computing the moments of the orthonormal functions via Gauss-Legendre integration
2. Solving a linear program to find a sparse set of candidate points
3. Greedy clustering of nearby points
4. Local refinement via TNC optimization

The Gauss-LP approach was introduced by [ryu2015]_ and extended to
the multivariate setting with a greedy clustering strategy by [jakeman2018]_.

The following :class:`~openturns.ResourceMap` keys are used:

- ``GaussLPQuadrature-AlphaS`` (``UnsignedInteger``, default: ``100``): multiplier for the number of LP candidate points.
- ``GaussLPQuadrature-Ngauss`` (``UnsignedInteger``, default: ``100``): number of 1D Gauss-Legendre nodes for moment computation.
- ``GaussLPQuadrature-Epsilon`` (``Scalar``, default: ``1.0e-5``): tolerance for quadrature residual.
- ``GaussLPQuadrature-Solver`` (``String``, default: ``choose``): HiGHS solver used for the linear programming solve.
- ``GaussLPQuadrature-FallbackSolver`` (``String``, default: ``ipm``): HiGHS solver used when the first linear programming solve fails; an empty value disables the retry.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    Orthonormal functions.
distribution : :class:`~openturns.Distribution`
    The measure.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> dim = 2
>>> distribution = ot.Normal(dim)
>>> refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
>>> basis = [refBasis.build(i) for i in range(6)]
>>> algo = otexp.FiniteOrthonormalizationAlgorithm(basis, distribution)
>>> algo.run()
>>> quad = otexp.GaussLPQuadrature(algo.getOrthonormalFunctions(), distribution)
>>> nodes, weights = quad.build(3) # doctest: +SKIP
"

%feature("docstring") OT::GaussLPQuadrature::build
"Build a minimal quadrature rule.

Parameters
----------
n : int
    Number of orthonormal functions to integrate exactly.

Returns
-------
nodes : :class:`~openturns.Sample`
    The quadrature nodes.
weights : :class:`~openturns.Point`
    The quadrature weights.
"
