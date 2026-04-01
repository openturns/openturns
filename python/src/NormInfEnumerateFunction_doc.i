%feature("docstring") OT::NormInfEnumerateFunction
R"RAW(Infinite norm enumerate function.

Parameters
----------
dim : int, default=1
    Dimension of the :class:`~openturns.EnumerateFunction`. *dim* must be equal
    to the dimension of the :class:`~openturns.OrthogonalBasis`.

See also
--------
EnumerateFunction, LinearEnumerateFunction

Notes
-----
Enumeration functions are bijections from :math:`\Nset` to :math:`\Nset^{\inputDim}` (refer to :ref:`enumeration_strategy`).

They can be used to enumerate a multivariate basis
built as the tensorization of univariate basis, using the indexation of each marginal basis
(refer to and :ref:`enumeration_multivariate_basis`).

Examples
--------
>>> import openturns as ot
>>> # 2-dimensional case
>>> enumerateFunction = ot.NormInfEnumerateFunction(2)
>>> for i in range(10):
...     print(enumerateFunction(i))
[0,0]
[1,0]
[0,1]
[1,1]
[2,0]
[2,1]
[0,2]
[1,2]
[2,2]
[3,0])RAW"
