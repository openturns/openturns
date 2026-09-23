#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Realization issued from a SpectralProcess
dimension = 1

# Parameters of the distribution
N = 101
t0 = 0.0
dt = 0.1
myTimeGrid = ot.RegularGrid(t0, dt, N)

# Create a Sample
# parameters of gaussien impose a few risk to get negative values
mySample = ot.Normal(10, 3).getSample(N)

# get a realization from distribution
myRealization = ot.TimeSeries(myTimeGrid, mySample)

# Create the lambda parameter
lambdaVector = ot.Point(dimension)
for index in range(dimension):
    lambdaVector[index] = (index + 2.0) * 0.1

myBoxCox = ot.BoxCoxTransform(lambdaVector)

print("myBoxCox=", myBoxCox)

# Get the input and output dimension
print("myBoxCox input dimension = ", myBoxCox.getInputDimension())
print("myBoxCox output dimension = ", myBoxCox.getOutputDimension())

# Evaluation of the BoxCoxTransform on the realization
print("input time series =")
print(myRealization)
print("output time series = ")
print(myBoxCox(myRealization))

print("gradient=", myBoxCox.gradient([0.5]))
print("hessian=", myBoxCox.hessian([0.5]))

# Call the getInverse method
myInverseBoxCox = myBoxCox.getInverse()
print("myInverseBoxCox = ", myInverseBoxCox)

# Get the number of calls
print("number of call(s) : ", myBoxCox.getCallsNumber())

# BoxCoxGradient direct coverage (no prints)
bc = ot.BoxCoxTransform([0.0, 1e-10, 0.5, 1.0])
_ = repr(bc.getGradient())
_ = str(bc.getGradient())
assert "BoxCoxGradient" in repr(bc.getGradient())
g = bc.gradient([1.0, 1.0, 4.0, 2.0])
ott.assert_almost_equal(g[0, 0], 1.0, 1e-8, 1e-8)
ott.assert_almost_equal(g[0, 3], 1.0, 1e-8, 1e-8)
with ott.assert_raises(Exception):
    bc.gradient([1.0])
with ott.assert_raises(Exception):
    bc.gradient([-5.0, 1.0, 1.0, 1.0])
_ = bc.getInverse().gradient([0.5, 0.1, 0.2, 0.3])
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("boxcox_tr.xml"))
study.add("t", bc)
study.save()
