#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
from openturns.testing import assert_almost_equal
ot.RandomGenerator.SetSeed(1)
distribution_R = ot.LogNormalMuSigma(300.0, 30.0, 0.0).getDistribution()
distribution_F = ot.Normal(75e3, 5e3)
marginals = [distribution_R, distribution_F]
distribution = ot.ComposedDistribution(marginals)


model = ot.SymbolicFunction(['R', 'F'], ['R - F/(pi_*100.0)'])

vect = ot.RandomVector(distribution)
g = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Less(), -50.0)
myIS = otexp.StandardSpaceCrossEntropyImportanceSampling(event, 0.3)

myIS.run()
myResult = myIS.getResult()

assert_almost_equal(myResult.getProbabilityEstimate(), 1.394611e-04)
