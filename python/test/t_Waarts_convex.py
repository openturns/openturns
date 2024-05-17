#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

#
# Physical model
#
formulas = ["2.5 + 0.1*(X1-X2)^2.0 - (X1+X2)/sqrt(2.0)"]
limitState = ot.SymbolicFunction(["X1", "X2"], formulas)
dim = limitState.getInputDimension()
print(dim)

#
# Probabilistic model
#

mean = ot.Point(dim, 0.0)
mean[0] = 0.0
mean[1] = 0.0

sigma = ot.Point(dim, 0.0)
sigma[0] = 1.0
sigma[1] = 1.0

R = ot.IdentityMatrix(dim)

myDistribution = ot.Normal(mean, sigma, R)

start = myDistribution.getMean()
Covariance = myDistribution.getCovariance()

#
# Limit state
#

vect = ot.RandomVector(myDistribution)

output = ot.CompositeRandomVector(limitState, vect)

myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# Calculs

#
# FORM/SORM Abdo Rackwitz
myAbdoRackwitz = ot.AbdoRackwitz()
myAbdoRackwitz.setMaximumIterationNumber(100 * dim)
myAbdoRackwitz.setMaximumAbsoluteError(1.0e-10)
myAbdoRackwitz.setMaximumRelativeError(1.0e-10)
myAbdoRackwitz.setMaximumResidualError(1.0e-10)
myAbdoRackwitz.setMaximumConstraintError(1.0e-10)

myAlgoAR = ot.FORM(myAbdoRackwitz, myEvent, start)
myAlgoAR2 = ot.SORM(myAbdoRackwitz, myEvent, start)

myAlgoAR.run()
myAlgoAR2.run()

resultAR = myAlgoAR.getResult()
resultAR2 = myAlgoAR2.getResult()

#
# Monte Carlo
CoV_MC = 0.5
experiment = ot.MonteCarloExperiment()
myMC = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myMC.setMaximumOuterSampling(100000)
myMC.setBlockSize(1)
myMC.setMaximumCoefficientOfVariation(CoV_MC)
myMC.run()
result = myMC.getResult()
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.0042735)

#
# LHS
CoV_LHS = 0.1
experiment = ot.LHSExperiment()
experiment.setAlwaysShuffle(True)
myLHS = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myLHS.setMaximumOuterSampling(100000)
myLHS.setBlockSize(1)
myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
myLHS.run()
