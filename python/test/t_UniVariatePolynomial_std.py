#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Default constructor: null polynomial
    P = UniVariatePolynomial()
    point = 1.2
    print("P=", P)
    print("P(", point, ")=", P(point))

    # Constructor: construct the 1D polynomial with a vector of Point
    # e.g. UniVariatePolynomial P1 ( coefficients )
    coefficients = Point(6)
    coefficients[0] = 1.3
    coefficients[1] = 2.4
    coefficients[2] = 2.0
    coefficients[3] = -0.5
    coefficients[4] = 4.0
    coefficients[5] = 0.0

    print("Initial coefficients=", coefficients)
    P1 = UniVariatePolynomial(coefficients)
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
    coefficients = Point(3)
    coefficients[0] = -2.5
    coefficients[1] = 3.5
    coefficients[2] = -1.2
    P2 = UniVariatePolynomial(coefficients)
    print("P2=", P2)
    print("P1+P2=", P1 + P2)

    # Subtraction of two polynomials
    print("P1-P2=", P1 - P2)

    # Multiplication of two polynomials
    print("P1*P2=", P1 * P2)

    # Multiplication of two polynomials using FFT
    ResourceMap.SetAsUnsignedInteger(
        "UniVariatePolynomialImplementation-LargeDegree", 0)
    print("P1*P2=", P1 * P2)

except:
    import sys
    print("t_UniVariatePolynomial_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
