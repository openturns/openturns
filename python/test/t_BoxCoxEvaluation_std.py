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
myFunction = ot.BoxCoxEvaluation(lambdaVector)
myFunction.setName("myBoxCoxFunction")

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

# Function wrapper uses finite differences; check evaluation consistency
func = ot.Function(myFunction)
g = func.gradient(inPoint)
ott.assert_almost_equal(g[0, 0], 1.0 / 30.0, 1e-4, 1e-4)  # lambda=0 branch
ott.assert_almost_equal(g[1, 1], 1.0 / (30.0**0.5), 1e-4, 1e-4)
ott.assert_almost_equal(g[2, 2], 1.0, 1e-4, 1e-4)  # lambda=1 branch
_ = func.hessian(inPoint)
with ott.assert_raises(Exception):
    myFunction([1.0])
# parameter accessors
_ = myFunction.getParameter()
_ = myFunction.getParameterDescription()
myFunction.setParameter(myFunction.getParameter())
# marginal / save-load
_ = myFunction.getMarginal(0)
_ = myFunction.getMarginal([0, 2])
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("boxcox_eval.xml"))
study.add("f", func)
study.save()
