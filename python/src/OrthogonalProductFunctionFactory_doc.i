%feature("docstring") OT::OrthogonalProductFunctionFactory
R"RAW(Base class for orthogonal multivariate functions.

Parameters
----------
functions : list of :class:`~openturns.OrthogonalUniVariateFunctionFamily`
    List of orthogonal univariate function factories with the same
    dimension as the orthogonal basis.
enumerateFunction : :class:`~openturns.EnumerateFunction`, optional
    Associates to an integer its multi-index image in the :math:`\Nset^d`
    dimension, which is the dimension of the basis. This multi-index represents
    the collection of degrees of the univariate polynomials.

Notes
-----
OrthogonalProductFunctionFactory is a particular case of implementation of
the :class:`~openturns.OrthogonalBasis` in the case of polynomial chaos
expansion. It provides to the OrthogonalBasis the persistent types of the
univariate orthogonal polynomials (e.g. Hermite, Legendre, Laguerre and Jacobi)
needed to determine the distribution measure of projection of the input
variable. Let's note that the exact hessian and gradient have been implemented
for the product of polynomials.
To facilitate the construction of the basis it is recommended to use the class
:class:`~openturns.StandardDistributionPolynomialFactory`.

See also
--------
OrthogonalProductPolynomialFactory

Examples
--------
Create from a list of orthogonal functions.

>>> import openturns as ot
>>> funcColl = [ot.HaarWaveletFactory(), ot.FourierSeriesFactory()]
>>> productBasis = ot.OrthogonalProductFunctionFactory(funcColl)

Set an enumerate function.

>>> funcColl = [ot.HaarWaveletFactory(), ot.FourierSeriesFactory()]
>>> inputDimension = len(funcColl)
>>> enumerateFunction = ot.LinearEnumerateFunction(inputDimension)
>>> productBasis = ot.OrthogonalProductFunctionFactory(funcColl, enumerateFunction)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalProductFunctionFactory::getFunctionFamilyCollection
"Get the collection of univariate orthogonal function families.

Returns
-------
polynomialFamily : list of :class:`~openturns.OrthogonalUniVariateFunctionFamily`
    List of orthogonal univariate function families."

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalProductFunctionFactory::getMarginal
R"RAW(Get the marginal orthogonal functions.

Parameters
----------
indices : sequence of int, :math:`0 \leq i < \inputDim`
    List of marginal indices of the input variables.

Returns
-------
functionFamilylist : list of :class:`~openturns.OrthogonalUniVariateFunctionFamily`
    The marginal orthogonal functions.

Examples
--------
>>> import openturns as ot
>>> funcColl = [ot.HaarWaveletFactory(), ot.FourierSeriesFactory(), ot.HaarWaveletFactory()]
>>> productBasis = ot.OrthogonalProductFunctionFactory(funcColl)
>>> marginalProduct = productBasis.getMarginal([0, 2])  #  [ot.HaarWaveletFactory(), ot.HaarWaveletFactory()])RAW"
