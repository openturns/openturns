#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Dimension of test
dimension = 2

# function
function = ot.SymbolicFunction("x", "sin(x)")

# Init of the function
myFunction = ot.TrendEvaluation(function)
myFunction.setName("myTrendFunction")

# We check that for a Point with same values, the function is
# done by component
inPoint = ot.Point(dimension, 30.0)

# result point of the function
outPoint = myFunction(inPoint)
print("myFunction=", myFunction)
print(myFunction.getName(), "( ", inPoint, " ) = ", outPoint)

# Creation of a Sample
size = 10
inSample = ot.Sample(size, dimension)
for index in range(size):
    inSample[index] = ot.Point(dimension, index + 1)

# result of the function
outSample = myFunction(inSample)
print(myFunction.getName(), "( ", repr(inSample), " ) = ", repr(outSample))
