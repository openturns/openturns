#! /usr/bin/env python

import openturns as ot
#from math import *

ot.TESTPREAMBLE()

#
# Physical model
#

dim = 10
inputFunction = ot.Description(dim)
for i in range(dim):
    inputFunction[i] = "s0" + str(i + 1)

outputFunction = ot.Description(1)
outputFunction[0] = "g"

formulas = ot.Description(outputFunction.getSize())
formulas[0] = "10.0 - ("
for i in range(dim):
    formulas[0] = formulas[0] + inputFunction[i] + \
        "^2 / (1.0 + 0.1 * (1.0 + " + str(i) + "))"
    if (i > 0):
        formulas[0] = formulas[0] + " + "

limitState = ot.SymbolicFunction(inputFunction, outputFunction, formulas)

dim = limitState.getInputDimension()

#
# Probabilistic model
#

mean = ot.Point(dim, 0.0)

sigma = ot.Point(dim, 1.0)

R = ot.CorrelationMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

start = myDistribution.getSigma()

#
# Limit state
#

vect = ot.RandomVector(myDistribution)

output = ot.RandomVector(limitState, vect)

myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# Calculs
#
#
# FORM/SORM Cobyla
myCobyla = ot.Cobyla()
myCobyla.setMaximumEvaluationNumber(100 * dim)
myCobyla.setMaximumAbsoluteError(1.0e-4)
myCobyla.setMaximumRelativeError(1.0e-4)
myCobyla.setMaximumResidualError(1.0e-4)
myCobyla.setMaximumConstraintError(1.0e-4)

myAlgoC = ot.FORM(myCobyla, myEvent, start)
myAlgoC2 = ot.SORM(myCobyla, myEvent, start)

myAlgoC.run()
# myAlgoC2.run()

resultC = myAlgoC.getResult()
# resultC2 = myAlgoC2.getResult()

#
# FORM/SORM Abdo Rackwitz
myAbdoRackwitz = ot.AbdoRackwitz()
myAbdoRackwitz.setMaximumIterationNumber(100 * dim)
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
CoV_MC = 0.5
myMC = ot.MonteCarlo(myEvent)
myMC.setMaximumOuterSampling(1000000)
myMC.setBlockSize(1)
myMC.setMaximumCoefficientOfVariation(CoV_MC)
myMC.run()

#
# LHS
CoV_LHS = 0.1
myLHS = ot.LHS(myEvent)
myLHS.setMaximumOuterSampling(1000000)
myLHS.setBlockSize(1)
myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
myLHS.run()

#
# Directional Sampling
CoV_DS = 0.1
myDS1 = ot.DirectionalSampling(myEvent, ot.RootStrategy(
    ot.RiskyAndFast()), ot.SamplingStrategy(ot.RandomDirection()))
myDS1.setMaximumOuterSampling(1000000)
myDS1.setBlockSize(1)
myDS1.setMaximumCoefficientOfVariation(CoV_DS)
myDS1.run()

myDS2 = ot.DirectionalSampling(myEvent, ot.RootStrategy(
    ot.MediumSafe()), ot.SamplingStrategy(ot.RandomDirection()))
myDS2.setMaximumOuterSampling(1000000)
myDS2.setBlockSize(1)
myDS2.setMaximumCoefficientOfVariation(CoV_DS)
myDS2.run()

myDS3 = ot.DirectionalSampling(myEvent, ot.RootStrategy(
    ot.SafeAndSlow()), ot.SamplingStrategy(ot.RandomDirection()))
myDS3.setMaximumOuterSampling(1000000)
myDS3.setBlockSize(1)
myDS3.setMaximumCoefficientOfVariation(CoV_DS)
myDS3.run()

#
# Importance Sampling with Standard Event
meanSE = ot.Point(dim, 0.0)
for i in range(resultAR.getStandardSpaceDesignPoint().getDimension()):
    meanSE[i] = resultAR.getStandardSpaceDesignPoint()[i]
sigmaSE = ot.Point(dim, 1.0)
CorrSE = ot.IdentityMatrix(dim)
myImportanceSE = ot.Normal(meanSE, sigmaSE, CorrSE)

myStandardEvent = ot.StandardEvent(myEvent)

myISS = ot.ImportanceSampling(myStandardEvent, myImportanceSE)
myISS.setMaximumOuterSampling(1000000)
myISS.setBlockSize(1)
myISS.setMaximumCoefficientOfVariation(0.1)
myISS.run()

# Importance Sampling avec Event
meanE = Point(dim, 0.0)
for i in range(resultC.getPhysicalSpaceDesignPoint().getDimension()):
    meanE[i] = resultC.getPhysicalSpaceDesignPoint()[i]

sigmaE = ot.Point(dim, 0.0)
for i in range(resultC.getPhysicalSpaceDesignPoint().getDimension()):
    sigmaE[i] = sqrt(Covariance[i, i])

CorrE = ot.IdentityMatrix(dim)

myImportanceE = ot.Normal(meanE, sigmaE, CorrE)

myIS = ot.ImportanceSampling(myEvent, myImportanceE)
myIS.setMaximumOuterSampling(1000000)
myIS.setBlockSize(1)
myIS.setMaximumCoefficientOfVariation(0.1)
myIS.run()
#

#
# Results
#

#
# FORM/SORM Cobyla
PfC = resultC.getEventProbability()
Beta_generalizedC = resultC.getGeneralisedReliabilityIndex()
u_starC = resultC.getStandardSpaceDesignPoint()
x_starC = resultC.getPhysicalSpaceDesignPoint()
PtC = resultC.getIsStandardPointOriginInFailureSpace(
) and "true" or "false"
gammaC = resultC.getImportanceFactors()
beta_hasoferC = resultC.getHasoferReliabilityIndex()
SensitivityC = resultC.getEventProbabilitySensitivity()

# PFBreitC2 = resultC2.getEventProbabilityBreitung()
# BetaBreitC2 = resultC2.getGeneralisedReliabilityIndexBreitung()
# PFHBC2=resultC2.getEventProbabilityHohenbichler()
# BetaHBC2=resultC2.getGeneralisedReliabilityIndexHohenbichler()
# PFTvedtC2 = resultC2.getEventProbabilityTvedt()
# BetaTvedtC2 = resultC2.getGeneralisedReliabilityIndexTvedt()
# CurvC2 = resultC2.getSortedCurvatures()
# u_starC2 = resultC2.getStandardSpaceDesignPoint()
# x_starC2 = resultC2.getPhysicalSpaceDesignPoint()
# PtC2 = resultC2.getIsStandardPointOriginInFailureSpace() and "true" or "false"
# gammaC2 = resultC2.getImportanceFactors()
# beta_hasoferC2 = resultC2.getHasoferReliabilityIndex()

#
# FORM/SORM Abdo Rackwitz
PfAR = resultAR.getEventProbability()
Beta_generalizedAR = resultAR.getGeneralisedReliabilityIndex()
u_starAR = resultAR.getStandardSpaceDesignPoint()
x_starAR = resultAR.getPhysicalSpaceDesignPoint()
PtAR = resultAR.getIsStandardPointOriginInFailureSpace(
) and "true" or "false"
gammaAR = resultAR.getImportanceFactors()
beta_hasoferAR = resultAR.getHasoferReliabilityIndex()
SensitivityAR = resultAR.getEventProbabilitySensitivity()

# PFBreitAR2 = resultAR2.getEventProbabilityBreitung()
# BetaBreitAR2 = resultAR2.getGeneralisedReliabilityIndexBreitung()
# PFHBAR2=resultAR2.getEventProbabilityHohenbichler()
# BetaHBAR2=resultAR2.getGeneralisedReliabilityIndexHohenbichler()
# PFTvedtAR2 = resultAR2.getEventProbabilityTvedt()
# BetaTvedtAR2 = resultAR2.getGeneralisedReliabilityIndexTvedt()
# CurvAR2 = resultAR2.getSortedCurvatures()
# u_starAR2 = resultAR2.getStandardSpaceDesignPoint()
# x_starAR2 = resultAR2.getPhysicalSpaceDesignPoint()
# PtAR2 = resultAR2.getIsStandardPointOriginInFailureSpace() and "true" or "false"
# gammaAR2 = resultAR2.getImportanceFactors()
# beta_hasoferAR2 = resultAR2.getHasoferReliabilityIndex()

#
# Monte Carlo
ResultMC = myMC.getResult()
PFMC = ResultMC.getProbabilityEstimate()
CVMC = ResultMC.getCoefficientOfVariation()
Variance_PF_MC = ResultMC.getVarianceEstimate()
length90MC = ResultMC.getConfidenceLength(0.90)

#
# LHS
ResultLHS = myLHS.getResult()
PFLHS = ResultLHS.getProbabilityEstimate()
CVLHS = ResultLHS.getCoefficientOfVariation()
Variance_PF_LHS = ResultLHS.getVarianceEstimate()
length90LHS = ResultLHS.getConfidenceLength(0.90)

#
# Directional Sampling
ResultDS1 = myDS1.getResult()
PFDS1 = ResultDS1.getProbabilityEstimate()
CVDS1 = ResultDS1.getCoefficientOfVariation()
Variance_PF_DS1 = ResultDS1.getVarianceEstimate()
length90DS1 = ResultDS1.getConfidenceLength(0.90)

ResultDS2 = myDS2.getResult()
PFDS2 = ResultDS2.getProbabilityEstimate()
CVDS2 = ResultDS2.getCoefficientOfVariation()
Variance_PF_DS2 = ResultDS2.getVarianceEstimate()
length90DS2 = ResultDS2.getConfidenceLength(0.90)

ResultDS3 = myDS3.getResult()
PFDS3 = ResultDS3.getProbabilityEstimate()
CVDS3 = ResultDS3.getCoefficientOfVariation()
Variance_PF_DS3 = ResultDS3.getVarianceEstimate()
length90DS3 = ResultDS3.getConfidenceLength(0.90)

#
# Importance Sampling
ResultISS = myISS.getResult()
PFISS = ResultISS.getProbabilityEstimate()
CVISS = ResultISS.getCoefficientOfVariation()
Variance_PF_ISS = ResultISS.getVarianceEstimate()
length90ISS = ResultISS.getConfidenceLength(0.90)

ResultIS = myIS.getResult()
PFIS = ResultIS.getProbabilityEstimate()
CVIS = ResultIS.getCoefficientOfVariation()
Variance_PF_IS = ResultIS.getVarianceEstimate()
length90IS = ResultIS.getConfidenceLength(0.90)

#

#
# Outputs
#
print("")
print("")
print(
    "************************************************************************************************")
print(
    "***************************************** FORM  COBYLA *****************************************")
print(
    "************************************************************************************************")
print("event probability = %.5e" % PfC)
print("generalized reliability index = %.5f" % Beta_generalizedC)
print(
    "************************************************************************************************")
for i in range(u_starC.getDimension()):
    print("standard space design point = %.5f" % u_starC[i])
print(
    "************************************************************************************************")
for i in range(x_starC.getDimension()):
    print("physical space design point = %.5f" % x_starC[i])
print(
    "************************************************************************************************")
print("is standard point origin in failure space? ", PtC)
print(
    "************************************************************************************************")
for i in range(gammaC.getDimension()):
    print("importance factors = %.5f" % gammaC[i])
print(
    "************************************************************************************************")
print("Hasofer reliability index = %.5f" % beta_hasoferC)
print(
    "************************************************************************************************")
for i in range(SensitivityC.getSize()):
    for j in range(SensitivityC[i].getDimension()):
        print("Pf sensitivity = %.5f" % SensitivityC[i][j])
print(
    "************************************************************************************************")
print("")
print(
    "************************************************************************************************")
print(
    "************************************** FORM ABDO RACKWITZ **************************************")
print(
    "************************************************************************************************")
print("event probability = %.5e" % PfAR)
print("generalized reliability index = %.5f" % Beta_generalizedAR)
print(
    "************************************************************************************************")
for i in range(u_starAR.getDimension()):
    print("standard space design point = %.5f" % u_starAR[i])
print(
    "************************************************************************************************")
for i in range(x_starAR.getDimension()):
    print("physical space design point = %.5f" % x_starAR[i])
print(
    "************************************************************************************************")
print("is standard point origin in failure space? ", PtAR)
print(
    "************************************************************************************************")
for i in range(gammaAR.getDimension()):
    print("importance factors = %.5f" % gammaAR[i])
print(
    "************************************************************************************************")
print("Hasofer reliability index = %.5f" % beta_hasoferAR)
print(
    "************************************************************************************************")
for i in range(SensitivityAR.getSize()):
    for j in range(SensitivityAR[i].getDimension()):
        print("Pf sensitivity = %.5f" % SensitivityAR[i][j])
print(
    "************************************************************************************************")
print("")
print(
    "************************************************************************************************")
print(
    "***************************************** SORM  COBYLA *****************************************")
print(
    "************************************************************************************************")
print("Breitung event probability = %.5e" % PFBreitC2)
print("Breitung generalized reliability index = %.5f" % BetaBreitC2)
print("Hohenbichler event probability = %.5e" % PFHBC2)
print("Hohenbichler generalized reliability index = %.5f" % BetaHBC2)
print("Tvedt event probability = %.5e" % PFTvedtC2)
print("Tvedt generalized reliability index = %.5f" % BetaTvedtC2)
print(
    "************************************************************************************************")
for i in range(CurvC2.getDimension()):
    print("sorted curvatures = %.5f" % cleanScalar(CurvC2[i]))
print(
    "************************************************************************************************")
for i in range(u_starC2.getDimension()):
    print("standard space design point = %.5f" % u_starC2[i])
print(
    "************************************************************************************************")
for i in range(x_starC2.getDimension()):
    print("physical space design point = %.5f" % x_starC2[i])
print(
    "************************************************************************************************")
print(
    "************************************************************************************************")
print("is standard point origin in failure space? ", PtC2)
print(
    "************************************************************************************************")
for i in range(gammaC2.getDimension()):
    print("importance factors = %.5f" % gammaC2[i])
print(
    "************************************************************************************************")
print("Hasofer reliability index = %.5f" % beta_hasoferC2)
print(
    "************************************************************************************************")
print("")
print(
    "************************************************************************************************")
print(
    "************************************** SORM ABDO RACKWITZ **************************************")
print(
    "************************************************************************************************")
print("Breitung event probability = %.5e" % PFBreitAR2)
print("Breitung generalized reliability index = %.5f" % BetaBreitAR2)
print("Hohenbichler event probability = %.5e" % PFHBAR2)
print("Hohenbichler generalized reliability index = %.5f" % BetaHBAR2)
print("Tvedt event probability = %.5e" % PFTvedtAR2)
print("Tvedt generalized reliability index = %.5f" % BetaTvedtAR2)
print(
    "************************************************************************************************")
for i in range(CurvAR2.getDimension()):
    print("sorted curvatures = %.5f" % cleanScalar(CurvAR2[i]))
print(
    "************************************************************************************************")
for i in range(u_starAR2.getDimension()):
    print("standard space design point = %.5f" % u_starAR2[i])
print(
    "************************************************************************************************")
for i in range(x_starAR2.getDimension()):
    print("physical space design point = %.5f" % x_starAR2[i])
print(
    "************************************************************************************************")
print(
    "************************************************************************************************")
print("is standard point origin in failure space? ", PtAR2)
print(
    "************************************************************************************************")
for i in range(gammaAR2.getDimension()):
    print("importance factors = %.5f" % gammaAR2[i])
print(
    "************************************************************************************************")
print("Hasofer reliability index = %.5f" % beta_hasoferAR2)
print(
    "************************************************************************************************")
print("")
print(
    "************************************************************************************************")
print(
    "**************************************** MONTE CARLO *******************************************")
print(
    "************************************************************************************************")
print("Pf estimation = %.5e" % PFMC)
print("Pf Variance estimation = %.5e" % Variance_PF_MC)
print("CoV = %.5f" % CVMC)
print("90% Confidence Interval =", "%.5e" % length90MC)
print("CI at 90% =[", "%.5e" % (PFMC - 0.5 * length90MC),
      "; %.5e" % (PFMC + 0.5 * length90MC), "]")
print(
    "************************************************************************************************")
print("")
print(
    "************************************************************************************************")
print(
    "******************************************* L H S **********************************************")
print(
    "************************************************************************************************")
print("Pf estimation = %.5e" % PFLHS)
print("Pf Variance estimation = %.5e" % Variance_PF_LHS)
print("CoV = %.5f" % CVLHS)
print("90% Confidence Interval =", "%.5e" % length90LHS)
print("CI at 90% =[", "%.5e" % (PFLHS - 0.5 * length90LHS),
      "; %.5e" % (PFLHS + 0.5 * length90LHS), "]")
print(
    "************************************************************************************************")
print("")
