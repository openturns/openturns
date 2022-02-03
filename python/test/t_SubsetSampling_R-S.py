#!/usr/bin/env python

from __future__ import print_function, division
import openturns as ot
import openturns.testing as ott

#
# Physical model
#

limitState = ot.SymbolicFunction(['u1', 'u2'], ['u1-u2'])
dim = limitState.getInputDimension()

#
# Probabilistic model
#

mean = ot.Point(dim, 0.0)
mean[0] = 7.
mean[1] = 2.
sigma = ot.Point(dim, 1.0)

R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

#
# Limit state
#

vect = ot.RandomVector(myDistribution)

output = ot.CompositeRandomVector(limitState, vect)

myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# Computation
#
bs = 1

# Monte Carlo
experiment = ot.MonteCarloExperiment()
myMC = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myMC.setMaximumOuterSampling(int(1e6) // bs)
myMC.setBlockSize(bs)
myMC.setMaximumCoefficientOfVariation(-1.0)
myMC.run()

#
# SubsetSampling
mySS = ot.SubsetSampling(myEvent)
mySS.setMaximumOuterSampling(10000 // bs)
mySS.setBlockSize(bs)
mySS.setKeepEventSample(True)
mySS.run()

#
# Results
#

# Monte Carlo
resultMC = myMC.getResult()
PFMC = resultMC.getProbabilityEstimate()
CVMC = resultMC.getCoefficientOfVariation()
variance_PF_MC = resultMC.getVarianceEstimate()
length90MC = resultMC.getConfidenceLength(0.90)
N_MC = resultMC.getOuterSampling() * resultMC.getBlockSize()

#
# SubsetSampling
resultSS = mySS.getResult()
PFSS = resultSS.getProbabilityEstimate()
CVSS = resultSS.getCoefficientOfVariation()
variance_PF_SS = resultSS.getVarianceEstimate()
length90SS = resultSS.getConfidenceLength(0.90)
N_SS = resultSS.getOuterSampling() * resultSS.getBlockSize()

#

print('-'*100)
print('MONTE CARLO')
print('Pf estimation = %.5e' % PFMC)
print('Pf Variance estimation = %.5e' % variance_PF_MC)
print('CoV = %.5f' % CVMC)
print('90% Confidence Interval =', '%.5e' % length90MC)
print('CI at 90% =[', '%.5e' % (PFMC - 0.5 * length90MC),
      '; %.5e' % (PFMC + 0.5 * length90MC), ']')
print('Limit state calls =', N_MC)
print('-'*100)
print('SUBSET SAMPLING')
print('Pf estimation = %.5e' % PFSS)
print('Pf Variance estimation = %.5e' % variance_PF_SS)
print('CoV = %.5f' % CVSS)
print('90% Confidence Interval =', '%.5e' % length90SS)
print('CI at 90% =[', '%.5e' % (PFSS - 0.5 * length90SS),
      '; %.5e' % (PFSS + 0.5 * length90SS), ']')
print('Limit state calls =', N_SS)


# check that the event sample is right
inputEventSample = mySS.getEventInputSample()
outputEventSample = mySS.getEventOutputSample()
ouputG = limitState(inputEventSample)
diffSample = ouputG - outputEventSample
ott.assert_almost_equal(diffSample.computeMean(), [0.0])

# null variance case
print('-'*100)
f = ot.SymbolicFunction(['x'], ['x'])
X = ot.Normal()
Y = ot.CompositeRandomVector(f, ot.RandomVector(X))
event = ot.ThresholdEvent(Y, ot.Less(), 5000)
mc = ot.ProbabilitySimulationAlgorithm(event)
mc.run()
result = mc.getResult()
print('MC:    ', result)
subset = ot.SubsetSampling(event)
subset.setBlockSize(18)
subset.run()
result = subset.getResult()
print('SUBSET:', result)
assert subset.getStepsNumber() == 1, "wrong steps"
assert result.getProbabilityEstimate() == 1.0, "wrong pf"
assert result.getVarianceEstimate() == 0.0, "wrong var"

# case with last step threshold close from global threshold (before)
print('-'*100)
event = ot.ThresholdEvent(Y, ot.Less(), -3.33832)
mc = ot.ProbabilitySimulationAlgorithm(event)
mc.run()
print('MC:    ', mc.getResult())
ot.RandomGenerator.SetSeed(65132)
subset = ot.SubsetSampling(event)
subset.setMaximumOuterSampling(500)
subset.run()
print('SUBSET:', subset.getResult())
print('steps=', subset.getStepsNumber())
print('T=', subset.getThresholdPerStep())

# case with last step threshold close from global threshold (after)
print('-'*100)
event = ot.ThresholdEvent(Y, ot.Less(), -3.338325)
mc = ot.ProbabilitySimulationAlgorithm(event)
mc.run()
print('MC:    ', mc.getResult())
ot.RandomGenerator.SetSeed(65132)
subset = ot.SubsetSampling(event)
subset.setConvergenceStrategy(ot.Full())
subset.setMaximumOuterSampling(500)
subset.run()
print('SUBSET:', subset.getResult())
print('steps=', subset.getStepsNumber())
print('T=', subset.getThresholdPerStep())
