%feature("docstring") OT::LinearEnumerateFunction
R"RAW(Linear enumerate function.

Parameters
----------
dim : int
    Dimension.

Notes
-----
Enumeration functions are bijections from :math:`\Nset` to :math:`\Nset^{\inputDim}` (refer to :any:`enumeration_strategy`).

They can be used to enumerate a multivariate basis
built as the tensorization of univariate basis, using the indexation of each marginal basis
(refer to and :any:`enumeration_multivariate_basis`).

See also
--------
EnumerateFunction, HyperbolicAnisotropicEnumerateFunction

Examples
--------
>>> import openturns as ot
>>> # 4-dimensional case
>>> enumerateFunction = ot.LinearEnumerateFunction(4)
>>> for i in range(9):
...     print(enumerateFunction(i))
[0,0,0,0]
[1,0,0,0]
[0,1,0,0]
[0,0,1,0]
[0,0,0,1]
[2,0,0,0]
[1,1,0,0]
[1,0,1,0]
[1,0,0,1])RAW"
