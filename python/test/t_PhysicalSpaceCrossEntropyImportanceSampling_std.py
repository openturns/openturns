#!/usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal

ot.RandomGenerator.SetSeed(1)

distributionR = ot.LogNormalMuSigma(300.0, 30.0, 0.0).getDistribution()
distributionF = ot.Normal(75.0e3, 5.0e3)
marginals = [distributionR, distributionF]
distribution = ot.JointDistribution(marginals)

model = ot.SymbolicFunction(["R", "F"], ["R - F / (pi_ * 100.0)"])
vect = ot.RandomVector(distribution)
g = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(g, ot.Less(), -50.0)

distributionMargin1 = ot.LogNormalMuSigma().getDistribution()
distributionMargin2 = ot.Normal()
auxMarginals = [distributionMargin1, distributionMargin2]
auxDistribution = ot.JointDistribution(auxMarginals)

activeParameters = [0, 1, 2, 3, 4]
bounds = ot.Interval([3, 0.09, 0.0, 50e3, 2e3], [7, 0.5, 0.5, 100e3, 10e3])
initialTheta = [5.70, 0.1, 0.0, 75.0e3, 5.0e3]

algo = ot.PhysicalSpaceCrossEntropyImportanceSampling(
    event, auxDistribution, activeParameters, initialTheta, bounds, 0.3
)

algo.setKeepSample(True)
algo.setOptimizationAlgorithm(ot.TNC())
algo.run()
result = algo.getResult()
print(result)
assert_almost_equal(result.getProbabilityEstimate(), 0.000135442, 5e-2, 0.0)

# check that the event sample is right
stepsNumber = algo.getStepsNumber()
inputEventSample = algo.getInputSample(stepsNumber - 1, algo.EVENT1)
outputEventSample = algo.getOutputSample(stepsNumber - 1, algo.EVENT1)
outputG = model(inputEventSample)
diffSample = outputG - outputEventSample
assert_almost_equal(diffSample.computeMean(), [0.0])

# Test setter of event
ot.RandomGenerator.SetSeed(0)
X = ot.RandomVector(ot.Normal())
Y = ot.CompositeRandomVector(ot.SymbolicFunction(['X'], ['X']), X)
event = ot.ThresholdEvent(Y, ot.Less(), -2.0)
aux_distribution = ot.Normal()
active_parameters = [0,1]
bounds = ot.Interval([-2.0, 0.5], [2.0, 2.0])
initial_theta = [0., 1.]
algo = ot.PhysicalSpaceCrossEntropyImportanceSampling(event,aux_distribution,active_parameters,initial_theta,bounds,0.3)
algo.setMaximumOuterSampling(100000)
algo.run()
result = algo.getResult()
assert_almost_equal(result.getProbabilityEstimate(), ot.Normal().computeCDF(-2), 1.0e-2, 0.0)
ot.RandomGenerator.SetSeed(0)
Y2 = ot.CompositeRandomVector(ot.SymbolicFunction(['X'], ['2 * X']), X)
event2 = ot.ThresholdEvent(Y2, ot.Less(), -2.0)
algo.setEvent(event2)
algo.run()
result2 = algo.getResult()
assert_almost_equal(result2.getProbabilityEstimate(), ot.Normal().computeCDF(-1), 1.0e-2, 0.0)
