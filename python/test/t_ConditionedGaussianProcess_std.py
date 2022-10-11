#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.PlatformInfo.SetNumericalPrecision(3)
# Kriging use case
inputDimension = 2

# Learning data
levels = [8.0, 5.0]
box = ot.Box(levels)
inputSample = box.generate()
# Scale each direction
inputSample *= 10


model = ot.SymbolicFunction(["x", "y"], ["cos(0.5*x) + sin(y)"])
outputSample = model(inputSample)

# Validation data
sampleSize = 10
inputValidSample = ot.ComposedDistribution(2 * [ot.Uniform(0, 10.0)]).getSample(
    sampleSize
)
outputValidSample = model(inputValidSample)

# 2) Definition of exponential model
# The parameters have been calibrated using TNC optimization
# and AbsoluteExponential models
covarianceModel = ot.SquaredExponential([7.63, 2.11], [7.38])

# 3) Basis definition
basis = ot.ConstantBasisFactory(inputDimension).build()

# Kriging algorithm
algo = ot.KrigingAlgorithm(inputSample, outputSample, covarianceModel, basis)
algo.setOptimizeParameters(False)  # do not optimize hyper-parameters
algo.run()
result = algo.getResult()

vertices = [[1.0, 0.0], [2.0, 0.0], [2.0, 1.0], [1.0, 1.0], [1.5, 0.5]]
simplicies = [[0, 1, 4], [1, 2, 4], [2, 3, 4], [3, 0, 4]]

mesh2D = ot.Mesh(vertices, simplicies)
process = ot.ConditionedGaussianProcess(result, mesh2D)

# Get a realization of the process
realization = process.getRealization()
print("realization = ", repr(realization))

# Get a sample & compare it to expectation
sample = process.getSample(5000)
mean = sample.computeMean()
print("Mean over 5000 realizations = ", repr(mean))

# Check if one can sample the process over a mesh containing conditioning points
# and 100 new points
vertices = ot.Sample(inputSample)
vertices.add(ot.ComposedDistribution([ot.Uniform(0.0, 10.0)] * 2).getSample(100))
process = ot.ConditionedGaussianProcess(result, ot.Mesh(vertices))
realization = process.getRealization()
num = 0.0
den = 0.0
for i in range(len(inputSample)):
    num += (realization.getValueAtIndex(i) - outputSample[i]).norm()
    den += outputSample[i].norm()
error = num / den
ott.assert_almost_equal(error, 0.0, 1.0e-6, 1.0e-6)
