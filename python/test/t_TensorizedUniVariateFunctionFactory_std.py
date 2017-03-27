#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Polynomial factories
factoryCollection = [ot.MonomialFunctionFactory()] * 3

dim = len(factoryCollection)
factory = ot.TensorizedUniVariateFunctionFactory(factoryCollection)
print('factory=', factory)
x = [0.5, 1.0, 1.5]
for i in range(10):
    f = factory.build(i)
    print('i=', i, 'f(X)=', f(x))
