#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Dimension of test
dimension = 3

# Lambda
lambdaVector = ot.Point(dimension)
lambdaVector[0] = 0.
lambdaVector[1] = 0.5
lambdaVector[2] = 1

# Init of the function
myFunction = ot.InverseBoxCoxEvaluation(lambdaVector)
myFunction.setName("myInverseBoxCoxFunction")

# We check that for a Point with same values, the function is
# done by component
inPoint = ot.Point(dimension, 2.0)

# result point of the function
outPoint = myFunction(inPoint)
print("myFunction=", myFunction)
print(myFunction.getName(), "( ", inPoint, " ) = ", outPoint)

# Creation of a Sample
size = 10
inSample = ot.Sample(size, dimension)
for index in range(size):
    inSample[index] = ot.Point(dimension, (1.0 / (index + 1)))

# result of the function
outSample = myFunction(inSample)
print(myFunction.getName(), "( ", repr(inSample), " ) = ", repr(outSample))
