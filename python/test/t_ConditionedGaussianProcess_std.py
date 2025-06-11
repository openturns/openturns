#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp


def build_kriging_result(inputSample, outputSample, covarianceModel, basis):
    """
    From data & covariance model, build a kriging result
    """
    algo = ot.KrigingAlgorithm(inputSample, outputSample, covarianceModel, basis)
    algo.setOptimizeParameters(False)  # do not optimize hyper-parameters
    algo.run()
    result = algo.getResult()
    return result


def build_gpr_result(inputSample, outputSample, covarianceModel, basis):
    """
    From data & covariance model, build a Gaussian Process Regression result
    """
    fitter_algo = otexp.GaussianProcessFitter(
        inputSample, outputSample, covarianceModel, basis
    )
    fitter_algo.setOptimizeParameters(False)  # do not optimize hyper-parameters
    fitter_algo.run()
    fitter_result = fitter_algo.getResult()
    gpr_algo = otexp.GaussianProcessRegression(fitter_result)
    gpr_algo.run()
    gpr_result = gpr_algo.getResult()
    return gpr_result


ot.TESTPREAMBLE()

ot.PlatformInfo.SetNumericalPrecision(3)
# GP use case
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
inputValidSample = ot.JointDistribution(2 * [ot.Uniform(0, 10.0)]).getSample(sampleSize)
outputValidSample = model(inputValidSample)

# 2) Definition of exponential model
# The parameters have been calibrated using TNC optimization
# and AbsoluteExponential models
covarianceModel = ot.SquaredExponential([7.63, 2.11], [7.38])

# 3) Basis definition
basis = ot.ConstantBasisFactory(inputDimension).build()

kriging_result = build_kriging_result(inputSample, outputSample, covarianceModel, basis)
gpr_result = build_gpr_result(inputSample, outputSample, covarianceModel, basis)

# Define a 2D mesh
vertices = [[1.0, 0.0], [2.0, 0.0], [2.0, 1.0], [1.0, 1.0], [1.5, 0.5]]
simplicies = [[0, 1, 4], [1, 2, 4], [2, 3, 4], [3, 0, 4]]
mesh2D = ot.Mesh(vertices, simplicies)

# update the vertices
vertices = ot.Sample(inputSample)
vertices.add(ot.JointDistribution([ot.Uniform(0.0, 10.0)] * 2).getSample(100))

for result in [kriging_result, gpr_result]:
    ot.RandomGenerator.SetSeed(0)
    process = otexp.ConditionedGaussianProcess(result, mesh2D)

    # Get a realization of the process
    realization = process.getRealization()
    print("realization = ", repr(realization))

    # Get a sample & compare it to expectation
    sample = process.getSample(5000)
    mean = sample.computeMean()
    print("Mean over 5000 realizations = ", repr(mean))

    # Check if one can sample the process over a mesh containing conditioning points
    # and 100 new points
    process = otexp.ConditionedGaussianProcess(result, ot.Mesh(vertices))
    realization = process.getRealization()
    num = 0.0
    den = 0.0
    for i in range(len(inputSample)):
        num += (realization.getValueAtIndex(i) - outputSample[i]).norm()
        den += outputSample[i].norm()
    error = num / den
    ott.assert_almost_equal(error, 0.0, 1.0e-6, 1.0e-6)

# 2D use case - #2769
model = ot.SymbolicFunction(["x", "y"], ["cos(x) + sin(y)", "cos(0.5*x) + sin(y)"])
inputSample = ot.Sample([[0.0, 0.0], [0.0, 1.0], [1.0, 0.0], [1.0, 1.0], [0.5, 0.5]])
outputSample = model(inputSample)
covarianceModel = ot.TensorizedCovarianceModel([ot.SquaredExponential([1.0, 1.0])] * 2)

algo = ot.KrigingAlgorithm(inputSample, outputSample, covarianceModel)
algo.run()
result = algo.getResult()
vertices = [[0.3, 0.6], [0.4, 0.8]]
mesh2D = ot.Mesh(vertices)
process = otexp.ConditionedGaussianProcess(result, mesh2D)
sample = process.getSample(3)
ott.assert_almost_equal(sample.getSize(), 3, 0, 0)
ott.assert_almost_equal(sample.getDimension(), 2, 0, 0)

# 2D use case (#2769) with GPR
fitter_algo = otexp.GaussianProcessFitter(inputSample, outputSample, covarianceModel)
fitter_algo.run()
fitter_result = fitter_algo.getResult()
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()
process = otexp.ConditionedGaussianProcess(gpr_result, mesh2D)
ott.assert_almost_equal(sample.getSize(), 3, 0, 0)
ott.assert_almost_equal(sample.getDimension(), 2, 0, 0)
