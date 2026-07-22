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

#
# Limit state
#
vect = ot.RandomVector(myDistribution)
output = ot.CompositeRandomVector(limitState, vect)
myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# Reference value
#
pRef = (myDistribution.getMarginal(0) - myDistribution.getMarginal(1).sqr()).computeCDF(
    0.0
)

#
# FORM/SORM Cobyla
myCobyla = ot.Cobyla()
myCobyla.setMaximumCallsNumber(1000 * dim)
myCobyla.setMaximumAbsoluteError(1.0e-4)
myCobyla.setMaximumRelativeError(1.0e-4)
myCobyla.setMaximumResidualError(1.0e-4)
myCobyla.setMaximumConstraintError(1.0e-4)
myCobyla.setStartingPoint(myDistribution.getMean())

myAlgoC = ot.FORM(myCobyla, myEvent)
myAlgoC2 = ot.SORM(myCobyla, myEvent)

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
myAbdoRackwitz.setStartingPoint(myDistribution.getMean())

myAlgoAR = ot.FORM(myAbdoRackwitz, myEvent)
myAlgoAR2 = ot.SORM(myAbdoRackwitz, myEvent)

myAlgoAR.run()
myAlgoAR2.run()

resultAR = myAlgoAR.getResult()
resultAR2 = myAlgoAR2.getResult()
pFORM = resultAR.getEventProbability()
ott.assert_almost_equal(pFORM, pRef, 1e-2, 1e-2)
pSORM = resultAR2.getEventProbabilityBreitung()
ott.assert_almost_equal(pSORM, pRef, 1e-2, 1e-2)

#
# Monte Carlo
CoV_MC = 0.05
experiment = ot.MonteCarloExperiment()
myMC = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myMC.setMaximumOuterSampling(1000000)
myMC.setBlockSize(100)
myMC.setMaximumCoefficientOfVariation(CoV_MC)
myMC.run()
result = ot.ProbabilitySimulationResult(myMC.getResult())
pMC = result.getProbabilityEstimate()
ott.assert_almost_equal(pMC, pRef, 5e-2, 5e-2)

#
# LHS
#
CoV_LHS = 0.05
experiment = ot.LHSExperiment()
experiment.setAlwaysShuffle(True)
myLHS = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myLHS.setMaximumOuterSampling(1000000)
myLHS.setBlockSize(100)
myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
myLHS.run()
result = ot.ProbabilitySimulationResult(myLHS.getResult())
pLHS = result.getProbabilityEstimate()
ott.assert_almost_equal(pLHS, pRef, 5e-2, 5e-2)
