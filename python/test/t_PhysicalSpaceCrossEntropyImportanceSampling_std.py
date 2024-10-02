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

myISphysical = ot.PhysicalSpaceCrossEntropyImportanceSampling(
    event, auxDistribution, activeParameters, initialTheta, bounds, 0.3
)

myISphysical.setKeepSample(True)
myISphysical.setOptimizationAlgorithm(ot.TNC())
myISphysical.run()
myResult = myISphysical.getResult()
assert_almost_equal(myResult.getProbabilityEstimate(), 0.00012452, 1.0e-2, 0.0)

# check that the event sample is right
stepsNumber = myISphysical.getStepsNumber()
inputEventSample = myISphysical.getInputSample(stepsNumber - 1, myISphysical.EVENT1)
outputEventSample = myISphysical.getOutputSample(stepsNumber - 1, myISphysical.EVENT1)
outputG = model(inputEventSample)
diffSample = outputG - outputEventSample
assert_almost_equal(diffSample.computeMean(), [0.0])
