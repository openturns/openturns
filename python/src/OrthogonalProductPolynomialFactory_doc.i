%feature("docstring") OT::OrthogonalProductPolynomialFactory
R"RAW(Base class for orthogonal multivariate polynomials.

Available constructors:
    OrthogonalProductPolynomialFactory(*polynomials*)

    OrthogonalProductPolynomialFactory(*polynomials, enumerateFunction*)

    OrthogonalProductPolynomialFactory(*marginals*)

    OrthogonalProductPolynomialFactory(*marginals, enumerateFunction*)

Parameters
----------
polynomials : sequence of :class:`~openturns.OrthogonalUniVariatePolynomialFamily`
    List of orthogonal univariate polynomials factories with the same
    dimension as the orthogonal basis.
enumerateFunction : :class:`~openturns.EnumerateFunction`
    Associates to an integer its multi-index image in the :math:`\Nset^d`
    dimension, which is the dimension of the basis. This multi-index represents
    the collection of degrees of the univariate polynomials.
marginals : sequence of :class:`~openturns.Distribution`
    List of physical space marginals.

Notes
-----
OrthogonalProductPolynomialFactory is a particular case of implementation of
the :class:`~openturns.OrthogonalBasis` in the case of polynomial chaos
expansion. It provides to the OrthogonalBasis the persistent types of the
univariate orthogonal polynomials (e.g. Hermite, Legendre, Laguerre and Jacobi)
needed to determine the distribution measure of projection of the input
variable. Let's note that the exact hessian and gradient have been implemented
for the product of polynomials.

See also
--------
StandardDistributionPolynomialFactory

Examples
--------
Create from a collection of orthogonal polynomials.

>>> import openturns as ot
>>> polyColl = [ot.HermiteFactory(), ot.LegendreFactory(), ot.LaguerreFactory()]
>>> productBasis = ot.OrthogonalProductPolynomialFactory(polyColl)

Easy way to create a multivariate orthonormal basis from a distribution.

>>> Xdist = ot.JointDistribution([ot.Normal(), ot.Uniform(), ot.Gamma(2.75, 1.0)])
>>> marginals = [Xdist.getMarginal(i) for i in range(Xdist.getDimension())]
>>> productBasis = ot.OrthogonalProductPolynomialFactory(marginals)

Set an enumerate function.

>>> polyColl = [ot.HermiteFactory(), ot.LegendreFactory(), ot.LaguerreFactory()]
>>> enumerateFunction = ot.LinearEnumerateFunction(3)
>>> productBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalProductPolynomialFactory::getPolynomialFamilyCollection
"Get the collection of univariate orthogonal polynomial families.

Returns
-------
polynomialFamily : list of :class:`~openturns.OrthogonalUniVariatePolynomialFamily`
    List of orthogonal univariate polynomials families."

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalProductPolynomialFactory::getNodesAndWeights
R"RAW(Get the nodes and the weights.

Parameters
----------
degrees : list of positive int (:math:`k_1, \dots, k_n`)
    List of :math:`n` polynomial orders associated with the :math:`n`
    univariate polynomials of the basis.

Returns
-------
nodes : :class:`~openturns.Sample`
weights : :class:`~openturns.Point`
    Nodes and weights of the multivariate polynomial associated with the
    marginal degrees (:math:`k_1, \dots, k_n`) as the tensor product
    of the marginal orthogonal univariate polynomials, to build multivariate
    quadrature rules.

See also
--------
OrthogonalUniVariatePolynomialFamily.getNodesAndWeights

Examples
--------
>>> import openturns as ot
>>> # Define the model
>>> myModel = ot.SymbolicFunction(['x1','x2','x3'], ['1+x1*x2 + 2*x3^2'])
>>> # Create a distribution of dimension 3
>>> Xdist = ot.JointDistribution([ot.Normal(), ot.Uniform(), ot.Gamma(2.75, 1.0)])
>>> # Construct the multivariate orthonormal basis
>>> polyColl = [ot.HermiteFactory(), ot.LegendreFactory(), ot.LaguerreFactory(2.75)]
>>> enumerateFunction = ot.LinearEnumerateFunction(3)
>>> productBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)
>>> nodes, weights = productBasis.getNodesAndWeights([2, 3, 1])
>>> print(nodes[:2])
    [ v0        v1        v2        ]
0 : [ -1        -0.774597  3.75     ]
1 : [  1        -0.774597  3.75     ]
>>> print(weights[:2])
[0.138889,0.138889])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalProductPolynomialFactory::getMarginal
R"RAW(Get the marginal orthogonal polynomials.

Parameters
----------
indices : sequence of int, :math:`0 \leq i < \inputDim`
    List of marginal indices of the input variables.

Returns
-------
polynomialFamilylist : list of :class:`~openturns.OrthogonalUniVariatePolynomialFamily`
    The marginal orthogonal polynomials.

Examples
--------
>>> import openturns as ot
>>> funcColl = [ot.LegendreFactory(), ot.LaguerreFactory(), ot.HermiteFactory()]
>>> productBasis = ot.OrthogonalProductPolynomialFactory(funcColl)
>>> marginalProduct = productBasis.getMarginal([0, 2])  # [ot.LegendreFactory(), ot.HermiteFactory()])RAW"
