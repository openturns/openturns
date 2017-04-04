#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    inputDimension = 2

    x = Point(inputDimension)
    for i in range(inputDimension):
        x[i] = 2. + i

    print("x=", x)

    factory = ConstantBasisFactory(inputDimension)
    print("factory=", factory)
    basis = factory.build()
    print("basis=", basis)

    f = AggregatedFunction(basis)
    y = f(x)
    print("y=", y)

    factory = LinearBasisFactory(inputDimension)
    print("factory=", factory)
    basis = factory.build()
    print("basis=", basis)

    f = AggregatedFunction(basis)
    y = f(x)
    print("y=", y)

    factory = QuadraticBasisFactory(inputDimension)
    print("factory=", factory)
    basis = factory.build()
    print("basis=", basis)

    f = AggregatedFunction(basis)
    y = f(x)
    print("y=", y)

except:
    import sys
    print("t_BasisFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
