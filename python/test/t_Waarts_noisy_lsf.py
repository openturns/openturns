#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()


#
# Physical model
#

inputFunction = ot.Description.BuildDefault(7, "X")[1:]

formulas = [
    "X1 + 2*X2 + 2*X3 + X4 - 5*X5 - 5*X6 +0.001*(sin(100*X1)+sin(100*X2)+sin(100*X3)+sin(100*X4)+sin(100*X5)+sin(100*X6))"]

limitState = ot.SymbolicFunction(inputFunction, formulas)

dim = limitState.getInputDimension()
print(dim)

#
# Probabilistic model
#

mean = ot.Point(dim, 0.0)
mean[0] = 120.0
mean[1] = 120.0
mean[2] = 120.0
mean[3] = 120.0
mean[4] = 50.0
mean[5] = 40.0

sigma = ot.Point(dim, 0.0)
sigma[0] = 12.0
sigma[1] = 12.0
sigma[2] = 12.0
sigma[3] = 12.0
sigma[4] = 15.0
sigma[5] = 12.0

BorneInf = 0.0

component = ot.Description(1)

aCollection = ot.DistributionCollection()
for i in range(len(mean)):
    marginal = ot.LogNormalMuSigma(mean[i], sigma[i], BorneInf).getDistribution()
    aCollection.add(marginal)

# Create a copula : IndependentCopula (pas de correlation
aCopula = ot.IndependentCopula(aCollection.getSize())

# Instantiate one distribution object
myDistribution = ot.JointDistribution(aCollection, aCopula)

start = myDistribution.getMean()
Covariance = myDistribution.getCovariance()

#
# limit state
#

vect = ot.RandomVector(myDistribution)

output = ot.CompositeRandomVector(limitState, vect)

myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# Calculs
#
#
# FORM/SORM Cobyla
myCobyla = ot.Cobyla()
myCobyla.setRhoBeg(0.1)
myCobyla.setMaximumCallsNumber(1000 * dim)
myCobyla.setMaximumAbsoluteError(1.0e-4)
myCobyla.setMaximumRelativeError(1.0e-4)
myCobyla.setMaximumResidualError(1.0e-4)
myCobyla.setMaximumConstraintError(1.0e-4)

myAlgoC = ot.FORM(myCobyla, myEvent, start)

myAlgoC.run()

resultC = ot.FORMResult(myAlgoC.getResult())

#
# FORM/SORM Abdo Rackwitz
myAbdoRackwitz = ot.AbdoRackwitz()
myAbdoRackwitz.setMaximumIterationNumber(1000 * dim)
myAbdoRackwitz.setMaximumAbsoluteError(1.0e-6)
myAbdoRackwitz.setMaximumRelativeError(1.0e-6)
myAbdoRackwitz.setMaximumResidualError(1.0e-6)
myAbdoRackwitz.setMaximumConstraintError(1.0e-6)

myAlgoAR = ot.FORM(myAbdoRackwitz, myEvent, start)

myAlgoAR.run()

resultAR = ot.FORMResult(myAlgoAR.getResult())

#
# Monte Carlo
CoV_MC = 0.5
experiment = ot.MonteCarloExperiment()
myMC = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myMC.setMaximumOuterSampling(1000)
myMC.setBlockSize(100)
myMC.setMaximumCoefficientOfVariation(CoV_MC)
myMC.run()
result = myMC.getResult()
ott.assert_almost_equal(result.getProbabilityEstimate(), 0.0133333)

#
# LHS
CoV_LHS = 0.1
experiment = ot.LHSExperiment()
experiment.setAlwaysShuffle(True)
myLHS = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myLHS.setMaximumOuterSampling(1000)
myLHS.setBlockSize(10)
myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
myLHS.run()

#
# Directional Sampling
CoV_DS = 0.1
myDS1 = ot.DirectionalSampling(
    myEvent,
    ot.RootStrategy(ot.RiskyAndFast()),
    ot.SamplingStrategy(ot.RandomDirection()),
)
myDS1.setMaximumOuterSampling(1000)
myDS1.setBlockSize(10)
myDS1.setMaximumCoefficientOfVariation(CoV_DS)
myDS1.run()

myDS2 = ot.DirectionalSampling(
    myEvent, ot.RootStrategy(ot.MediumSafe()), ot.SamplingStrategy(ot.RandomDirection())
)
myDS2.setMaximumOuterSampling(1000)
myDS2.setBlockSize(10)
myDS2.setMaximumCoefficientOfVariation(CoV_DS)
myDS2.run()

myDS3 = ot.DirectionalSampling(
    myEvent,
    ot.RootStrategy(ot.SafeAndSlow()),
    ot.SamplingStrategy(ot.RandomDirection()),
)
myDS3.setMaximumOuterSampling(1000)
myDS3.setBlockSize(10)
myDS3.setMaximumCoefficientOfVariation(CoV_DS)
myDS3.run()

#
# Importance Sampling avec Standard Event
meanSE = ot.Point(dim, 0.0)
for i in range(resultAR.getStandardSpaceDesignPoint().getDimension()):
    meanSE[i] = resultAR.getStandardSpaceDesignPoint()[i]
sigmaSE = ot.Point(dim, 1.0)
CorrSE = ot.IdentityMatrix(dim)
myImportanceSE = ot.Normal(meanSE, sigmaSE, CorrSE)

myStandardEvent = ot.StandardEvent(myEvent)

experiment = ot.ImportanceSamplingExperiment(myImportanceSE)
myISS = ot.ProbabilitySimulationAlgorithm(myStandardEvent)
myISS.setMaximumOuterSampling(1000)
myISS.setBlockSize(10)
myISS.setMaximumCoefficientOfVariation(0.1)
myISS.run()

# Importance Sampling avec Event
meanE = ot.Point(dim, 0.0)
for i in range(resultC.getPhysicalSpaceDesignPoint().getDimension()):
    meanE[i] = resultC.getPhysicalSpaceDesignPoint()[i]

sigmaE = ot.Point(dim, 0.0)
for i in range(resultC.getPhysicalSpaceDesignPoint().getDimension()):
    sigmaE[i] = m.sqrt(Covariance[i, i])

CorrE = ot.IdentityMatrix(dim)

myImportanceE = ot.Normal(meanE, sigmaE, CorrE)

experiment = ot.ImportanceSamplingExperiment(myImportanceE)
myIS = ot.ProbabilitySimulationAlgorithm(myEvent)
myIS.setMaximumOuterSampling(1000)
myIS.setBlockSize(10)
myIS.setMaximumCoefficientOfVariation(0.1)
myIS.run()
