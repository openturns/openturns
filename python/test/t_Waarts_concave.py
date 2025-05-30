#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

#
# Physical model
#

limitState = ot.SymbolicFunction(
    ["X1", "X2"], ["-0.5*(X1-X2)*(X1-X2) - (X1+X2)/(sqrt(2)) + 3"]
)

dim = limitState.getInputDimension()
print(dim)

#
# probabilistic model
#
ot.RandomGenerator.SetSeed(0)
mean = [0.0] * dim
sigma = [1.0] * dim
R = ot.IdentityMatrix(dim)

myDistribution = ot.Normal(mean, sigma, R)

Covariance = myDistribution.getCovariance()

#
# limit state
#

vect = ot.RandomVector(myDistribution)

output = ot.CompositeRandomVector(limitState, vect)

myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# Calculation

#
# FORM/SORM Abdo Rackwitz
myAbdoRackwitz = ot.AbdoRackwitz()
myAbdoRackwitz.setMaximumIterationNumber(100 * dim)
myAbdoRackwitz.setMaximumAbsoluteError(1.0e-10)
myAbdoRackwitz.setMaximumRelativeError(1.0e-10)
myAbdoRackwitz.setMaximumResidualError(1.0e-10)
myAbdoRackwitz.setMaximumConstraintError(1.0e-10)
myAbdoRackwitz.setStartingPoint(myDistribution.getMean())

myAlgoAR = ot.FORM(myAbdoRackwitz, myEvent)
myAlgoAR2 = ot.SORM(myAbdoRackwitz, myEvent)

myAlgoAR.run()
# myAlgoAR2.run()

resultAR = myAlgoAR.getResult()
# resultAR2 = SORMResult(myAlgoAR2.getResult())

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
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.129032)

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
result = myLHS.getResult()
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.103926)
