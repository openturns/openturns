#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

dim = 2

x = [2.0 + i for i in range(dim)]

print("x=", x)

factory = ot.ConstantBasisFactory(dim)
print("factory=", factory)
basis = factory.build()
print("basis=", basis)

f = ot.AggregatedFunction(basis)
y = f(x)
print("y=", y)

factory = ot.LinearBasisFactory(dim)
print("factory=", factory)
basis = factory.build()
print("basis=", basis)

f = ot.AggregatedFunction(basis)
y = f(x)
print("y=", y)

factory = ot.QuadraticBasisFactory(dim)
print("factory=", factory)
basis = factory.build()
print("basis=", basis)

f = ot.AggregatedFunction(basis)
y = f(x)
print("y=", y)

