#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


# Default constructor: null polynomial
P0 = ot.MonomialFunction()
point = 1.2
print("P0 :", P0)
print("P0(", point, ")=", P0(point))


# Standard constructor : set degree/coefficients
degrees = range(1, 6)

for degree in degrees:
    print("\n")
    P = ot.MonomialFunction(degree)
    print("P :", P)
    print("P(1) = %1.1f, dP(1) = %1.1f, d^2P(1) = %1.1f"
          % (P(1.0), P.gradient(1.0), P.hessian(1.0)))
