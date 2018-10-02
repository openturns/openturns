#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


# Default dimension parameter to evaluate the model
inputDimension = 1
outputDimension = 1

# Amplitude values
amplitude = [1.0] * outputDimension
# Scale values
scale = [1.0] * inputDimension

tmin = 0.0
step = 0.1
n = 11

myTimeGrid = ot.RegularGrid(tmin, step, n)
size = 25

# Second order model with parameters
myCovModel = ot.ExponentialModel(scale, amplitude)
print("myCovModel = ", myCovModel)

myProcess1 = ot.GaussianProcess(myCovModel, myTimeGrid)
print("myProcess1 = ", myProcess1)
print("is stationary? ", myProcess1.isStationary())
myProcess1.setSamplingMethod(0)
print("mean over ", size, " realizations = ",
      myProcess1.getSample(size).computeMean())
myProcess1.setSamplingMethod(2)
print("mean over ", size, " realizations = ",
      myProcess1.getSample(size).computeMean())

# With constant trend
trend = ot.TrendTransform(ot.SymbolicFunction("t", "4.0"), myTimeGrid)
myProcess2 = ot.GaussianProcess(trend, myCovModel, myTimeGrid)
myProcess2.setSamplingMethod(2)
print("myProcess2 = ", myProcess2)
print("is stationary? ", myProcess2.isStationary())
print("mean over ", size, " realizations= ",
      myProcess2.getSample(size).computeMean())

# With varying trend
trend3 = ot.TrendTransform(ot.SymbolicFunction("t", "sin(t)"), myTimeGrid)
myProcess3 = ot.GaussianProcess(trend3, myCovModel, myTimeGrid)
print("myProcess3 = ", myProcess3)
print("is stationary? ", myProcess3.isStationary())
myProcess3.setSamplingMethod(0)
print("mean over ", size, " realizations = ",
      myProcess3.getSample(size).computeMean())
myProcess3.setSamplingMethod(2)
print("mean over ", size, " realizations = ",
      myProcess3.getSample(size).computeMean())

cov = ot.ExponentialModel([2.0] * inputDimension, [1.0, 2.0, 3.0])
model = ot.GaussianProcess(cov, myTimeGrid)
print('model=', model)
print('marginal=', model.getMarginal([0, 2]))
