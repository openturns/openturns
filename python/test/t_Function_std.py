#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# Instance creation
myFunc = ot.SymbolicFunction(
    ['x1', 'x2'], ['x1*sin(x2)', 'cos(x1+x2)', '(x2+1)*exp(x1-2*x2)'])

# Copy constructor
newFunc = ot.Function(myFunc)

print("myFunc=" + repr(myFunc))
print("myFunc input parameter(s)=")
for i in range(myFunc.getInputDimension()):
    print(myFunc.getInputDescription()[i])
print("myFunc output parameter(s) and marginal(s)=")
for i in range(myFunc.getOutputDimension()):
    print(myFunc.getOutputDescription()[i])
    print("Marginal function", i, "=", repr(myFunc.getMarginal(i)))

try:
    print(ot.Function() == None)
except TypeError:
    # swig<4
    print(False)
