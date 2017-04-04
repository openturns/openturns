#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


# Define factory
factory = ot.MonomialFunctionFactory()
print ("factory = ", factory)
print ("Build functions for degrees in 1..9")
for i in range(10):
    print ("P_%d=%s" % (i, factory.build(i).__str__()))

# Build as Monomial factory
print ("Build as MonomialFunction for degrees in 1..9")
for i in range(10):
    print ("P_%d=%s" % (i, factory.buildAsMonomialFunction(i).__repr__()))
