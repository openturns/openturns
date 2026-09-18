%feature("docstring") OT::OrthogonalUniVariatePolynomial
"Base class for orthogonal univariate polynomials.

.. warning::

    Orthogonal univariate polynomials are not intended to be created manually.
    They should be constructed with the subclasses of
    :class:`~openturns.OrthogonalUniVariatePolynomialFamily` (like e.g.
    :class:`~openturns.HermiteFactory`). Constructor parameters are therefore
    intentionally not documented.

See Also
--------
OrthogonalUniVariatePolynomialFamily

Examples
--------
>>> import openturns as ot

Create a univariate polynomial from a list of coefficients:

>>> for i in range(3):
...     print(ot.OrthogonalUniVariatePolynomialFamily().build(i))
1
X
-0.707107 + 0.707107 * X^2"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalUniVariatePolynomial::getRecurrenceCoefficients
R"RAW(Accessor to the recurrence coefficients.

Returns
-------
recurrence_coefficients : list of :class:`Point`
    The list of recurrence coefficients that defined the orthogonal univariate
    polynomial from the very first univariate orthogonal polynomial
    :math:`P_0`.

Notes
-----
Any sequence of orthogonal polynomials has a recurrence formula relating any
three consecutive polynomials as follows:

.. math::

    P_{i + 1} = (a_i x + b_i) P_i + c_i P_{i - 1}, \quad i > 1

Examples
--------
>>> import openturns as ot
>>> polynomial = ot.OrthogonalUniVariatePolynomialFamily().build(2)
>>> print(polynomial.getRecurrenceCoefficients())
0 : [  1         0         0        ]
1 : [  0.707107  0        -0.707107 ])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalUniVariatePolynomial::getA
R"RAW(Accessor to the scale coefficient of the affine transformation.

The affine transformation maps the variable :math:`x` to the variable
:math:`z = a x + b`: if :math:`X` is a random variable distributed according
to the associated measure, then :math:`Z = aX + b` is distributed according
to its standard representative. The polynomial is evaluated at :math:`x` as
the composition of the polynomial :math:`p_n` orthonormal with respect to the
standard representative and of the affine transformation :math:`T`,
i.e. :math:`P_n(x) = p_n(a x + b)`.

Returns
-------
a : float
    The scale coefficient :math:`a` of the affine transformation.

Examples
--------
>>> import openturns as ot
>>> polynomial = ot.OrthogonalUniVariatePolynomialFamily().build(2)
>>> print(polynomial.getA())
1.0)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalUniVariatePolynomial::setA
R"RAW(Accessor to the scale coefficient of the affine transformation.

The affine transformation maps the variable :math:`x` to the variable
:math:`z = a x + b`: if :math:`X` is a random variable distributed according
to the associated measure, then :math:`Z = aX + b` is distributed according
to its standard representative. The polynomial is evaluated at :math:`x` as
the composition of the polynomial :math:`p_n` orthonormal with respect to the
standard representative and of the affine transformation :math:`T`,
i.e. :math:`P_n(x) = p_n(a x + b)`.

Parameters
----------
a : float
    The scale coefficient :math:`a` of the affine transformation.

Examples
--------
>>> import openturns as ot
>>> polynomial = ot.OrthogonalUniVariatePolynomialFamily().build(2)
>>> polynomial.setA(2.0)
>>> print(polynomial.getA())
2.0)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalUniVariatePolynomial::getB
R"RAW(Accessor to the translation coefficient of the affine transformation.

The affine transformation maps the variable :math:`x` to the variable
:math:`z = a x + b`: if :math:`X` is a random variable distributed according
to the associated measure, then :math:`Z = aX + b` is distributed according
to its standard representative. The polynomial is evaluated at :math:`x` as
the composition of the polynomial :math:`p_n` orthonormal with respect to the
standard representative and of the affine transformation :math:`T`,
i.e. :math:`P_n(x) = p_n(a x + b)`.

Returns
-------
b : float
    The translation coefficient :math:`b` of the affine transformation.

Examples
--------
>>> import openturns as ot
>>> polynomial = ot.OrthogonalUniVariatePolynomialFamily().build(2)
>>> print(polynomial.getB())
0.0)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::OrthogonalUniVariatePolynomial::setB
R"RAW(Accessor to the translation coefficient of the affine transformation.

The affine transformation maps the variable :math:`x` to the variable
:math:`z = a x + b`: if :math:`X` is a random variable distributed according
to the associated measure, then :math:`Z = aX + b` is distributed according
to its standard representative. The polynomial is evaluated at :math:`x` as
the composition of the polynomial :math:`p_n` orthonormal with respect to the
standard representative and of the affine transformation :math:`T`,
i.e. :math:`P_n(x) = p_n(a x + b)`.

Parameters
----------
b : float
    The translation coefficient :math:`b` of the affine transformation.

Examples
--------
>>> import openturns as ot
>>> polynomial = ot.OrthogonalUniVariatePolynomialFamily().build(2)
>>> polynomial.setB(1.0)
>>> print(polynomial.getB())
1.0)RAW"
