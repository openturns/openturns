%feature("docstring") OT::HyperbolicAnisotropicEnumerateFunction
R"RAW(Hyperbolic and anisotropic enumerate function.

Available constructors:
    HyperbolicAnisotropicEnumerateFunction(*dim*)

    HyperbolicAnisotropicEnumerateFunction(*dim, q*)

    HyperbolicAnisotropicEnumerateFunction(*weight*)

    HyperbolicAnisotropicEnumerateFunction(*weight, q*)

Parameters
----------
dim : int
    Dimension of the :class:`~openturns.EnumerateFunction`. *dim* must be equal
    to the dimension of the :class:`~openturns.OrthogonalBasis`.
q : float
    Correspond to the q-quasi norm parameter.

    Default value is :math:`q = 0.4`.
weight : sequence of float
    Weights of the indices in each dimension.

    Default value  is :math:`w_i = 1` for any :math:`i`.

See also
--------
EnumerateFunction, LinearEnumerateFunction

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
>>> enumerateFunction = ot.HyperbolicAnisotropicEnumerateFunction(2, 0.5)
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

In the following example, we create an hyperbolic enumerate function
in 3 dimensions based on the weights [1,2,4].
Notice that the first marginal index, with weight equal to 1, comes
first in the enumeration.

>>> import openturns as ot
>>> enumerateFunction = ot.HyperbolicAnisotropicEnumerateFunction([1, 2, 4])
>>> for i in range(20):
...     print('i=', i, 'enum=', enumerateFunction(i))
i= 0 enum= [0,0,0]
i= 1 enum= [1,0,0]
i= 2 enum= [0,1,0]
i= 3 enum= [2,0,0]
i= 4 enum= [3,0,0]
i= 5 enum= [0,0,1]
i= 6 enum= [0,2,0]
i= 7 enum= [4,0,0]
i= 8 enum= [5,0,0]
i= 9 enum= [0,3,0]
i= 10 enum= [6,0,0]
i= 11 enum= [7,0,0]
i= 12 enum= [0,0,2]
i= 13 enum= [0,4,0]
i= 14 enum= [8,0,0]
i= 15 enum= [1,1,0]
i= 16 enum= [9,0,0]
i= 17 enum= [0,5,0]
i= 18 enum= [10,0,0]
i= 19 enum= [11,0,0]
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::HyperbolicAnisotropicEnumerateFunction::getQ
"Accessor to the norm.

Returns
-------
q : float
    q-quasi norm parameter."

// ---------------------------------------------------------------------

%feature("docstring") OT::HyperbolicAnisotropicEnumerateFunction::getWeight
"Accessor to the weights.

Returns
-------
w : :class:`~openturns.Point`
    Weights of the indices in each dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::HyperbolicAnisotropicEnumerateFunction::setQ
"Accessor to the norm.

Parameters
----------
q : float
    q-quasi norm parameter."

// ---------------------------------------------------------------------

%feature("docstring") OT::HyperbolicAnisotropicEnumerateFunction::setWeight
"Accessor to the weights.

Parameters
----------
w : sequence of float
    Weights of the indices in each dimension."

