%define OT_UniVariatePolynomial_doc
"Base class for univariate polynomials.

Parameters
----------
coefficients : sequence of float
    Polynomial coefficients in increasing polynomial order.

Examples
--------
>>> import openturns as ot

Create a univariate polynomial from a list of coefficients:

>>> P = ot.UniVariatePolynomial([1.0, 2.0, 3.0])
>>> print(P)
1 + 2 * X + 3 * X^2

Univariate polynomials are of course callable:

>>> print(P(1.0))
6.0

Addition, subtraction and multiplication of univariate polynomials:

>>> P = ot.UniVariatePolynomial([1.0, 2.0, 3.0])
>>> Q = ot.UniVariatePolynomial([1.0, 2.0])
>>> print(f'({P}) + ({Q}) = {P + Q}')
(1 + 2 * X + 3 * X^2) + (1 + 2 * X) = 2 + 4 * X + 3 * X^2
>>> print(f'({P}) - ({Q}) = {P - Q}')
(1 + 2 * X + 3 * X^2) - (1 + 2 * X) = 3 * X^2
>>> print(f'({P}) * ({Q}) = {P * Q}')
(1 + 2 * X + 3 * X^2) * (1 + 2 * X) = 1 + 4 * X + 7 * X^2 + 6 * X^3

Notes
-----
The following :class:`~openturns.ResourceMap` key is used:

- ``UniVariatePolynomial-SmallDegree`` (``UnsignedInteger``, default: ``400``): degree below which the polynomials are considered as small."
%enddef
%feature("docstring") OT::UniVariatePolynomialImplementation::setCoefficients
OT_UniVariatePolynomial_setCoefficients_doc
