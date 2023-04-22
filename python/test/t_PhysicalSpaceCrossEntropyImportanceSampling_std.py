#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
from openturns.testing import assert_almost_equal
ot.RandomGenerator.SetSeed(1)

distribution_R = ot.LogNormalMuSigma(300.0, 30.0, 0.0).getDistribution()
distribution_F = ot.Normal(75.0e3, 5.0e3)
marginals = [distributionR, distributionF]
distribution = ot.ComposedDistribution(marginals)

model = ot.SymbolicFunction(['R', 'F'], ['R - F/(pi_*100.0)'])
vect = ot.RandomVector(distribution)
g = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(g, ot.Less(), -50.0)

distributionMargin1 = ot.LogNormalMuSigma().getDistribution()
distributionMargin2 = ot.Normal()
auxMarginals = [distributionMargin1, distributionMargin2]
auxDistribution = ot.ComposedDistribution(auxMarginals)

active_parameters = [0, 1, 2, 3, 4]
bounds = ot.Interval([3, 0.09, 0., 50e3, 2e3],
                     [7, 0.5, 0.5, 100e3, 10e3])
initial_theta = [5.70, 0.1, 0., 75e3, 5e3]

myISphysical = otexp.PhysicalSpaceCrossEntropyImportanceSampling(event, auxDistribution, activeParameters, initialTheta, bounds, 0.3)
my_IS_physical.setOptimizationAlgorithm(ot.TNC())
my_IS_physical.run()
myResult = my_IS_physical.getResult()
assert_almost_equal(myResult.getProbabilityEstimate(), 0.00012452, 1e-2)
