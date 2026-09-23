#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Dimension of test
dimension = 3

# Lambda
lambdaVector = ot.Point(dimension)
lambdaVector[0] = 0.0
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

# Function wrapper uses finite differences; check consistency
func = ot.Function(myFunction)
_ = func.gradient(inPoint)
_ = func.hessian(inPoint)
with ott.assert_raises(Exception):
    myFunction([1.0])
_ = myFunction.getParameter()
_ = myFunction.getParameterDescription()
myFunction.setParameter(myFunction.getParameter())
_ = myFunction.getMarginal(1)
_ = myFunction.getMarginal([0, 1])
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("invboxcox_eval.xml"))
study.add("f", func)
study.save()
