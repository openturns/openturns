%feature("docstring") OT::FiniteOrthogonalFunctionFactory
"FiniteOrthogonalFunction orthonormal multivariate functional family.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    The finite set of functions orthogonal wrt the given measure.
measure : :class:`~openturns.Distribution`
    The measure defining the inner product of the factory.

Notes
-----
This class implements a user-defined multivariate orthonormal basis associated with an arbitrary multidimensional distribution.

See also
--------
openturns.StandardDistributionPolynomialFactory, openturns.OrthogonalProductPolynomialFactory, openturns.SoizeGhanemFactory

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> mean = [0.0] * 2
>>> std = [1.0] * 2
>>> R = ot.CorrelationMatrix([[1.0, 0.8], [0.8, 1.0]])
>>> distribution = ot.Normal(mean, std, R)
>>> f0 = ot.SymbolicFunction(['x0', 'x1'], ['1.0'])
>>> f1 = ot.SymbolicFunction(['x0', 'x1'], ['x0'])
>>> f2 = ot.SymbolicFunction(['x0', 'x1'], ['-4.0 * x0 / 3.0 + 5.0 * x1 / 3.0'])
>>> f3 = ot.SymbolicFunction(['x0', 'x1'], ['-1.0 / sqrt(2.0) + x0^2 / sqrt(2)'])
>>> f4 = ot.SymbolicFunction(['x0', 'x1'], ['-4.0 / 3.0 - 1.885618083165693 * (-1.0 / sqrt(2.0) + x0^2 / sqrt(2.0)) + 5.0 * x0 * x1 / 3.0'])
>>> f5 = ot.SymbolicFunction(['x0', 'x1'], ['2.5141574442076222 + 16.0 / 9.0 * (-1.0 / sqrt(2.0) + x0^2 / sqrt(2.0)) - 3.142696805266918 * x0 * x1 + 25.0 / 9.0 * (-1.0 / sqrt(2.0) + x1^2 / sqrt(2.0))'])
>>> initialBasis = [f0, f1, f2, f3, f4, f5]
>>> factory = otexp.FiniteOrthogonalFunctionFactory(initialBasis, distribution)
>>> point = [0.5] * 2
>>> for i in range(len(initialBasis)):
...     value = factory.build(i)(point)
...     print(f'FiniteOrthogonalFunction_{i}({point})={value}')
FiniteOrthogonalFunction_0([0.5, 0.5])=[1]
FiniteOrthogonalFunction_1([0.5, 0.5])=[0.5]
FiniteOrthogonalFunction_2([0.5, 0.5])=[0.166667]
FiniteOrthogonalFunction_3([0.5, 0.5])=[-0.53033]
FiniteOrthogonalFunction_4([0.5, 0.5])=[0.0833333]
FiniteOrthogonalFunction_5([0.5, 0.5])=[-0.687465]
"

// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteOrthogonalFunctionFactory::getFunctionsCollection
R"RAW(Accessor to the collection of functions.

Returns
-------
fctColl : :class:`~openturns.FunctionCollection`
    The collection of scalar functions :math:`(f_i)_{i = 1, \ldots, \ell}` which
    defines the basis.)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::FiniteOrthogonalFunctionFactory::setFunctionsCollection
R"RAW(Accessor to the collection of functions defining the basis.

Parameters
----------
functions : sequence of :class:`~openturns.Function`
    The collection of functions :math:`(f_i)_{i = 1, \ldots, \ell}`.
.)RAW"
