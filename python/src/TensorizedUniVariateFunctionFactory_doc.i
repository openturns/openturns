%feature("docstring") OT::TensorizedUniVariateFunctionFactory
R"RAW(Base class for tensorized multivariate functions.

Parameters
----------
functions : list of :class:`~openturns.UniVariateFunctionFamily`
    List of univariate function factories.
enumerateFunction : :class:`~openturns.EnumerateFunction`, optional
    Associates to an integer its multi-index image in the :math:`\Nset^d`
    dimension, which is the dimension of the basis. This multi-index represents
    the collection of degrees of the univariate polynomials.

Notes
-----
TensorizedUniVariateFunctionFactory allows one to create multidimensional functions as the tensor product of univariate
functions created by their respective factories (i.e. :class:`~openturns.UniVariateFunctionFamily`):

.. math::

   \Phi_n(x_1,\dots,x_d)=\prod_{i=1}^d \phi^i_{enum(n)_i}(x_i)


where :math:`\phi^i_k` is the univariate basis of degree :math:`k` associated to the component :math:`x_i`
and :math:`enum(n)_i` is the ith component of the multi-index :math:`enum(n)`


Let's note that the exact hessian and gradient have been implemented
for the product of polynomials.

Examples
--------
>>> import openturns as ot
>>> funcColl = [ot.HaarWaveletFactory(), ot.FourierSeriesFactory(), ot.MonomialFunctionFactory()]
>>> dim = len(funcColl)
>>> enumerateFunction = ot.LinearEnumerateFunction(dim)
>>> productBasis = ot.TensorizedUniVariateFunctionFactory(funcColl, enumerateFunction)
)RAW"



%feature("docstring") OT::TensorizedUniVariateFunctionFactory::getEnumerateFunction
"Accessor to the enumerate function.

Returns
-------
enumerateFunction : :class:`~openturns.EnumerateFunction`
    The enumerate function"



%feature("docstring") OT::TensorizedUniVariateFunctionFactory::getFunctionFamilyCollection
"Accessor to the collection of function families.

Returns
-------
coll : sequence of :class:`~openturns.UniVariateFunctionFamily`
    The collection of function families"



%feature("docstring") OT::TensorizedUniVariateFunctionFactory::setEnumerateFunction
"Accessor to the enumerate function.

Parameters
----------
enumerateFunction : :class:`~openturns.EnumerateFunction`
    The enumerate function"



%feature("docstring") OT::TensorizedUniVariateFunctionFactory::setFunctionFamilyCollection
"Accessor to the collection of function families.

Parameters
----------
coll : sequence of :class:`~openturns.UniVariateFunctionFamily`
    The collection of function families"

