#!/usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
ot.RandomGenerator.SetSeed(1)
distribution_R = ot.LogNormalMuSigma(300.0, 30.0, 0.0).getDistribution()
distribution_F = ot.Normal(75e3, 5e3)
marginals = [distribution_R, distribution_F]
distribution = ot.ComposedDistribution(marginals)

# create the model
model = ot.SymbolicFunction(['R', 'F'], ['R-F/(pi_*100.0)'])
#create the event 
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Less(), -50.0)
myIS = ot.StandardSpaceCrossEntropyImportanceSampling(event,0.3)
#run StandardSpaceCrossEntropyImportanceSamplingAlgorithm
myIS.run()
myResult = myIS.getResult()

# Compare with reference probability
assert_almost_equal(myResult.getProbabilityEstimate(),1.394611e-04)
