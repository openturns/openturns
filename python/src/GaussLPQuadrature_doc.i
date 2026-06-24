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

The following :class:`~openturns.ResourceMap` keys can be set:

- *GaussLPQuadrature-AlphaS*: multiplier for the number of LP candidate points
  (default=100)
- *GaussLPQuadrature-Ngauss*: number of 1D Gauss-Legendre nodes for moment
  computation (default=100)
- *GaussLPQuadrature-Epsilon*: tolerance for quadrature residual
  (default=1.0e-5)

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
>>> algo = otexp.QROrthonormalizationAlgorithm(basis, distribution)
>>> algo.run()
>>> quad = otexp.GaussLPQuadrature(algo.getOrthonormalFunctions(), distribution)
>>> nodes, weights = quad.build(3)
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
