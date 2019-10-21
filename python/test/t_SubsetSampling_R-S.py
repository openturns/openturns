#!/usr/bin/env python

from __future__ import print_function, division
import openturns as ot

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

print('')
print(
    '************************************************************************************************')
print(
    '**************************************** MONTE CARLO *******************************************')
print(
    '************************************************************************************************')
print('Pf estimation = %.5e' % PFMC)
print('Pf Variance estimation = %.5e' % variance_PF_MC)
print('CoV = %.5f' % CVMC)
print('90% Confidence Interval =', '%.5e' % length90MC)
print('CI at 90% =[', '%.5e' % (PFMC - 0.5 * length90MC),
      '; %.5e' % (PFMC + 0.5 * length90MC), ']')
print('Limit state calls =', N_MC)
print(
    '************************************************************************************************')
print('')
print(
    '************************************************************************************************')
print(
    '******************************************* SUBSET SAMPLING **********************************************')
print(
    '************************************************************************************************')
print('Pf estimation = %.5e' % PFSS)
print('Pf Variance estimation = %.5e' % variance_PF_SS)
print('CoV = %.5f' % CVSS)
print('90% Confidence Interval =', '%.5e' % length90SS)
print('CI at 90% =[', '%.5e' % (PFSS - 0.5 * length90SS),
      '; %.5e' % (PFSS + 0.5 * length90SS), ']')
print('Limit state calls =', N_SS)
print(
    '************************************************************************************************')
print('')
