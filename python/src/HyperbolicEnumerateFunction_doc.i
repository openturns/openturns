%feature("docstring") OT::HyperbolicEnumerateFunction
R"RAW(Hyperbolic enumerate function.

Available constructors:
    HyperbolicEnumerateFunction(*dim*)

    HyperbolicEnumerateFunction(*dim, q*)

    HyperbolicEnumerateFunction(*weight*)

    HyperbolicEnumerateFunction(*weight, q*)

Parameters
----------
dim : int
    Dimension of the :class:`~openturns.EnumerateFunction`. *dim* must be equal
    to the dimension of the :class:`~openturns.OrthogonalBasis`.
q : float
    Correspond to the q-quasi norm parameter.

    Default value is :math:`q = 0.4`.

See also
--------
EnumerateFunction, LinearEnumerateFunction, HyperbolicAnisotropicEnumerateFunction

Notes
-----
Enumeration functions are bijections from :math:`\Nset` to :math:`\Nset^{\inputDim}` (refer to :any:`enumeration_strategy`).

They can be used to enumerate a multivariate basis
built as the tensorization of univariate basis, using the indexation of each marginal basis
(refer to and :any:`enumeration_multivariate_basis`).

Examples
--------
In the following example, we create an hyperbolic enumerate function
in 2 dimension with a quasi-norm equal to 0.5.
Notice, for example, that the function with multi-index [3,0]
come before [1,1], although the sum of marginal indices is lower: this
is the result of the hyperbolic quasi-norm.

>>> import openturns as ot
>>> enumerateFunction = ot.HyperbolicEnumerateFunction(2, 0.5)
>>> for i in range(10):
...     print(enumerateFunction(i))
[0,0]
[1,0]
[0,1]
[2,0]
[0,2]
[3,0]
[0,3]
[1,1]
[4,0]
[0,4]
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HyperbolicEnumerateFunction::getQ
"Accessor to the norm.

Returns
-------
q : float
    q-quasi norm parameter."

// ---------------------------------------------------------------------

%feature("docstring") OT::HyperbolicEnumerateFunction::setQ
"Accessor to the norm.

Parameters
----------
q : float
    q-quasi norm parameter."

// ---------------------------------------------------------------------
