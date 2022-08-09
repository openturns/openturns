#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

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
myProcess1.setSamplingMethod(ot.GaussianProcess.CHOLESKY)
print("mean over ", size, " realizations = ",
      myProcess1.getSample(size).computeMean())
myProcess1.setSamplingMethod(ot.GaussianProcess.GIBBS)
print("mean over ", size, " realizations = ",
      myProcess1.getSample(size).computeMean())

# With constant trend
trend = ot.TrendTransform(ot.SymbolicFunction("t", "4.0"), myTimeGrid)
myProcess2 = ot.GaussianProcess(trend, myCovModel, myTimeGrid)
myProcess2.setSamplingMethod(ot.GaussianProcess.GIBBS)
print("myProcess2 = ", myProcess2)
print("is stationary? ", myProcess2.isStationary())
print("mean over ", size, " realizations= ",
      myProcess2.getSample(size).computeMean())

# With varying trend
trend3 = ot.TrendTransform(ot.SymbolicFunction("t", "sin(t)"), myTimeGrid)
myProcess3 = ot.GaussianProcess(trend3, myCovModel, myTimeGrid)
print("myProcess3 = ", myProcess3)
print("is stationary? ", myProcess3.isStationary())
myProcess3.setSamplingMethod(ot.GaussianProcess.CHOLESKY)
print("mean over ", size, " realizations = ",
      myProcess3.getSample(size).computeMean())
myProcess3.setSamplingMethod(ot.GaussianProcess.GIBBS)
print("mean over ", size, " realizations = ",
      myProcess3.getSample(size).computeMean())

cov = ot.ExponentialModel([2.0] * inputDimension, [1.0, 2.0, 3.0])
model = ot.GaussianProcess(cov, myTimeGrid)
print('model=', model)
print('marginal=', model.getMarginal([0, 2]))

# FIX #2121
ot.RandomGenerator.SetSeed(0)
standard_deviation = 10.0
mesh = ot.Mesh([[0.0]]) # singleton
cov_matrix = ot.CovarianceMatrix(1, [standard_deviation**2]) # associated "covariance matrix"
covModel = ot.UserDefinedCovarianceModel(mesh, cov_matrix)

# Create the "Gaussian Process" discretized on a singleton
myProcess = ot.GaussianProcess(covModel, mesh)
myProcess.setSamplingMethod(ot.GaussianProcess.GIBBS)
size = 1000
sample = ot.Sample(size, 1)
for i in range(size):
    sample[i] = myProcess.getRealization().getValues()[0]

ott.assert_almost_equal(sample.computeStandardDeviation()[0], 10.2016)