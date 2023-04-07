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

distribution_margin1 = ot.LogNormalMuSigma().getDistribution()
distribution_margin2 = ot.Normal()
aux_marginals = [distribution_margin1, distribution_margin2]
aux_distribution = ot.ComposedDistribution(aux_marginals)

## Definition of parameters to be optimized
active_parameters = [0, 1, 2, 3, 4] # active parameters from the auxiliary distribution which will be optimized
### WARNING : native parameters of distribution have to be considered

bounds = ot.Interval([3, 0.09, 0., 50e3, 2e3], # bounds on the active parameters
                     [7, 0.5, 0.5, 100e3, 10e3])

initial_theta= [5.70, 0.1, 0., 75e3, 5e3] # initial value of the active parameters

my_IS_physical = ot.PhysicalSpaceCrossEntropyImportanceSampling(
    event,active_parameters,initial_theta,bounds,aux_distribution,0.3)

my_IS_physical.setSolver(ot.TNC())

my_IS_physical.run()
myResult = my_IS_physical.getResult()

# Compare with reference probability
assert_almost_equal(myResult.getProbabilityEstimate(),0.000124728)
