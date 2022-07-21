#! /usr/bin/env python

from openturns import *

TESTPREAMBLE()

try:

    # Dimension of test
    dimension = 3

    # Lambda
    lambdaVector = Point(dimension)
    lambdaVector[0] = 0.
    lambdaVector[1] = 0.5
    lambdaVector[2] = 1

    # Init of the function
    myFunction = InverseBoxCoxEvaluation(lambdaVector)
    myFunction.setName("myInverseBoxCoxFunction")

    # We check that for a Point with same values, the function is
    # done by component
    inPoint = Point(dimension, 2.0)

    # result point of the function
    outPoint = myFunction(inPoint)
    print("myFunction=", myFunction)
    print(myFunction.getName(), "( ", inPoint, " ) = ", outPoint)

    # Creation of a Sample
    size = 10
    inSample = Sample(size, dimension)
    for index in range(size):
        inSample[index] = Point(dimension, (1.0 / (index + 1)))

    # result of the function
    outSample = myFunction(inSample)
    print(myFunction.getName(), "( ", repr(inSample), " ) = ", repr(outSample))


except:
    import sys
    print("t__InverseBoxCoxEvaluation_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
