#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

#
# Physical model
#
limitState = ot.SymbolicFunction(["r", "s"], ["r - s^2"])
dim = limitState.getInputDimension()

#
# Probabilistic model
#
mean = [11.0, 1.5]
sigma = [1.0, 0.5]
R = ot.CorrelationMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)
start = myDistribution.getMean()

#
# Limit state
#
vect = ot.RandomVector(myDistribution)
output = ot.CompositeRandomVector(limitState, vect)
myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# FORM/SORM Cobyla
myCobyla = ot.Cobyla()
myCobyla.setMaximumCallsNumber(1000 * dim)
myCobyla.setMaximumAbsoluteError(1.0e-4)
myCobyla.setMaximumRelativeError(1.0e-4)
myCobyla.setMaximumResidualError(1.0e-4)
myCobyla.setMaximumConstraintError(1.0e-4)

myAlgoC = ot.FORM(myCobyla, myEvent, start)
myAlgoC2 = ot.SORM(myCobyla, myEvent, start)

myAlgoC.run()
myAlgoC2.run()

resultC = myAlgoC.getResult()
resultC2 = myAlgoC2.getResult()

#
# FORM/SORM Abdo Rackwitz
myAbdoRackwitz = ot.AbdoRackwitz()
myAbdoRackwitz.setMaximumIterationNumber(1000)
myAbdoRackwitz.setMaximumAbsoluteError(1.0e-4)
myAbdoRackwitz.setMaximumRelativeError(1.0e-4)
myAbdoRackwitz.setMaximumResidualError(1.0e-4)
myAbdoRackwitz.setMaximumConstraintError(1.0e-4)

myAlgoAR = ot.FORM(myAbdoRackwitz, myEvent, start)
myAlgoAR2 = ot.SORM(myAbdoRackwitz, myEvent, start)

myAlgoAR.run()
myAlgoAR2.run()

resultAR = myAlgoAR.getResult()
resultAR2 = myAlgoAR2.getResult()

#
# Monte Carlo
CoV_MC = 0.1
experiment = ot.MonteCarloExperiment()
myMC = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myMC.setMaximumOuterSampling(1000000)
myMC.setBlockSize(1000)
myMC.setMaximumCoefficientOfVariation(CoV_MC)
myMC.run()
result = myMC.getResult()
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.000258312)

#
# LHS
CoV_LHS = 0.1
experiment = ot.LHSExperiment()
experiment.setAlwaysShuffle(True)
myLHS = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myLHS.setMaximumOuterSampling(1000000)
myLHS.setBlockSize(100)
myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
myLHS.run()
result = myMC.getResult()
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.000258312)
