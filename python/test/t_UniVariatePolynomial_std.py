#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Default constructor: null polynomial
P = ot.UniVariatePolynomial()
point = 1.2
print("P=", P)
print("P(", point, ")=", P(point))

# Constructor: construct the 1D polynomial with a vector of Point
# e.g. UniVariatePolynomial P1 ( coefficients )
coefficients = ot.Point(6)
coefficients[0] = 1.3
coefficients[1] = 2.4
coefficients[2] = 2.0
coefficients[3] = -0.5
coefficients[4] = 4.0
coefficients[5] = 0.0

print("Initial coefficients=", coefficients)
P1 = ot.UniVariatePolynomial(coefficients)
print("P1=", repr(P1))
print("P1=", P1)
print("P1=", P1.__str__("Y", ""))
print("P1 degree=", P1.getDegree())
print("P1 roots=", P1.getRoots())
print("P1(", point, ")=", P1(point))

# Multiplication by a scalar (postfix only)
print("P1*2.0 =", P1 * 2.0)

# P1.incrementDegree(incDeg) : multiply P1 by X^incDeg
print("P1.incrementDegree(3)=", P1.incrementDegree(3))
print("P1.incrementDegree(0)=", P1.incrementDegree(0))
print("P1.incrementDegree(1)=", P1.incrementDegree(1))
print("P1.incrementDegree()=", P1.incrementDegree())

# Sum of two polynomials
coefficients = ot.Point(3)
coefficients[0] = -2.5
coefficients[1] = 3.5
coefficients[2] = -1.2
P2 = ot.UniVariatePolynomial(coefficients)
print("P2=", P2)
print("P1+P2=", P1 + P2)

# Subtraction of two polynomials
print("P1-P2=", P1 - P2)

# Multiplication of two polynomials
print("P1*P2=", P1 * P2)

# Multiplication of two polynomials using FFT
print("P1*P2=", P1 * P2)

# Create a polynomial with a leading term equal to 0 (do not print, then
# a first term with unity coefficient (hide the coefficient when printing),
# and then several coefficients equal to 1 or 0
P1 = ot.UniVariatePolynomial([0.0, 1.0, -2.0, -1.0, 0.0, 1.0])
print("P1=", P1)
print("(repr) P1=", repr(P1))
print("(html) P1=", P1._repr_html_())
