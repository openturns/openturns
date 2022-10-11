#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


#
# Physical model
#

inputFunction = ot.Description(6)
inputFunction[0] = "X1"
inputFunction[1] = "X2"
inputFunction[2] = "X3"
inputFunction[3] = "X4"
inputFunction[4] = "X5"
inputFunction[5] = "X6"

outputFunction = ot.Description(1)
outputFunction[0] = "G"

formulas = ot.Description(outputFunction.getSize())
formulas[
    0
] = "X1 + 2*X2 + 2*X3 + X4 - 5*X5 - 5*X6 +0.001*(sin(100*X1)+sin(100*X2)+sin(100*X3)+sin(100*X4)+sin(100*X5)+sin(100*X6))"

EtatLimite = ot.SymbolicFunction(inputFunction, outputFunction, formulas)

dim = EtatLimite.getInputDimension()
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

marginal = ot.LogNormal(mean[0], sigma[0], BorneInf, ot.LogNormal.MUSIGMA)
marginal.setName("First")
component[0] = "One"
marginal.setDescription(component)
# Fill the first marginal of aCollection
aCollection.add(Distribution(marginal, "First"))

# Create a second marginal : distribution 1D
marginal = ot.LogNormal(mean[1], sigma[1], BorneInf, ot.LogNormal.MUSIGMA)
marginal.setName("Second")
component[0] = "Two"
marginal.setDescription(component)
# Fill the second marginal of aCollection
aCollection.add(Distribution(marginal, "Second"))

# Create a third marginal : distribution 1D
marginal = ot.LogNormal(mean[2], sigma[2], BorneInf, ot.LogNormal.MUSIGMA)
marginal.setName("Third")
component[0] = "Three"
marginal.setDescription(component)
# Fill the third marginal of aCollection
aCollection.add(Distribution(marginal, "Third"))

# Create a forth marginal : distribution 1D
marginal = ot.LogNormal(mean[3], sigma[3], BorneInf, ot.LogNormal.MUSIGMA)
marginal.setName("Forth")
component[0] = "Four"
marginal.setDescription(component)
# Fill the forth marginal of aCollection
aCollection.add(Distribution(marginal, "Forth"))

# Create a fifth marginal : distribution 1D
marginal = ot.LogNormal(mean[4], sigma[4], BorneInf, LogNormal.MUSIGMA)
marginal.setName("Fifth")
component[0] = "Five"
marginal.setDescription(component)
# Fill the fifth marginal of aCollection
aCollection.add(Distribution(marginal, "Fifth"))

# Create a sixth marginal : distribution 1D
marginal = ot.LogNormal(mean[5], sigma[5], BorneInf, LogNormal.MUSIGMA)
marginal.setName("Sixth")
component[0] = "Six"
marginal.setDescription(component)
# Fill the sixth marginal of aCollection
aCollection.add(Distribution(marginal, "Sixth"))

# Create a copula : IndependentCopula (pas de correlation
aCopula = ot.IndependentCopula(aCollection.getSize())
aCopula.setName("Independent copula")

# Instantiate one distribution object
myDistribution = ot.ComposedDistribution(aCollection, aCopula)
myDistribution.setName("myDist")

start = myDistribution.getMean()
Covariance = myDistribution.getCovariance()

#
# limit state
#

vect = ot.RandomVector(myDistribution)

output = ot.RandomVector(EtatLimite, vect)

myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# Calculs
#
#
# FORM/SORM Cobyla
myCobyla = ot.Cobyla()
parameters.setRhoBeg(0.1)
myCobyla.setMaximumEvaluationNumber(1000 * dim)
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
myMC = ot.MonteCarlo(myEvent)
myMC.setMaximumOuterSampling(1000)
myMC.setBlockSize(100)
myMC.setMaximumCoefficientOfVariation(CoV_MC)
myMC.run()

#
# LHS
CoV_LHS = 0.1
myLHS = ot.LHS(myEvent)
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

myISS = ot.ImportanceSampling(myStandardEvent, myImportanceSE)
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
    sigmaE[i] = sqrt(Covariance[i, i])

CorrE = ot.IdentityMatrix(dim)

myImportanceE = ot.Normal(meanE, sigmaE, CorrE)

myIS = ot.ImportanceSampling(myEvent, myImportanceE)
myIS.setMaximumOuterSampling(1000)
myIS.setBlockSize(10)
myIS.setMaximumCoefficientOfVariation(0.1)
myIS.run()
#

#
# Outputs
#
print("")
print("")
print(
    "************************************************************************************************"
)
print(
    "***************************************** FORM  COBYLA *****************************************"
)
print(
    "************************************************************************************************"
)
print("event probability = %.5e" % PfC)
print("generalized reliability index = %.5f" % Beta_generalizedC)
print(
    "************************************************************************************************"
)
for i in range(u_starC.getDimension()):
    print("standard space design point = %.5f" % u_starC[i])
print(
    "************************************************************************************************"
)
for i in range(x_starC.getDimension()):
    print("physical space design point = %.5f" % x_starC[i])
print(
    "************************************************************************************************"
)
print("is standard point origin in failure space? ", PtC)
print(
    "************************************************************************************************"
)
for i in range(gammaC.getDimension()):
    print("importance factors = %.5f" % gammaC[i])
print(
    "************************************************************************************************"
)
print("Hasofer reliability index = %.5f" % beta_hasoferC)
print(
    "************************************************************************************************"
)
for i in range(SensitivityC.getSize()):
    for j in range(SensitivityC[i].getDimension()):
        print("Pf sensitivity = %.5f" % SensitivityC[i][j])
print(
    "************************************************************************************************"
)
print("")
print(
    "************************************************************************************************"
)
print(
    "************************************** FORM ABDO RACKWITZ **************************************"
)
print(
    "************************************************************************************************"
)
print("event probability = %.5e" % PfAR)
print("generalized reliability index = %.5f" % Beta_generalizedAR)
print(
    "************************************************************************************************"
)
for i in range(u_starAR.getDimension()):
    print("standard space design point = %.5f" % u_starAR[i])
print(
    "************************************************************************************************"
)
for i in range(x_starAR.getDimension()):
    print("physical space design point = %.5f" % x_starAR[i])
print(
    "************************************************************************************************"
)
print("is standard point origin in failure space? ", PtAR)
print(
    "************************************************************************************************"
)
for i in range(gammaAR.getDimension()):
    print("importance factors = %.5f" % gammaAR[i])
print(
    "************************************************************************************************"
)
print("Hasofer reliability index = %.5f" % beta_hasoferAR)
print(
    "************************************************************************************************"
)
for i in range(SensitivityAR.getSize()):
    for j in range(SensitivityAR[i].getDimension()):
        print("Pf sensitivity = %.5f" % SensitivityAR[i][j])
print(
    "************************************************************************************************"
)
print("")
print(
    "************************************************************************************************"
)
print(
    "***************************************** SORM  COBYLA *****************************************"
)
print(
    "************************************************************************************************"
)
print("Breitung event probability = %.5e" % PFBreitC2)
print("Breitung generalized reliability index = %.5f" % BetaBreitC2)
print("Hohenbichler event probability = %.5e" % PFHBC2)
print("Hohenbichler generalized reliability index = %.5f" % BetaHBC2)
print("Tvedt event probability = %.5e" % PFTvedtC2)
print("Tvedt generalized reliability index = %.5f" % BetaTvedtC2)
print(
    "************************************************************************************************"
)
for i in range(CurvC2.getDimension()):
    print("sorted curvatures = %.5f" % cleanScalar(CurvC2[i]))
print(
    "************************************************************************************************"
)
for i in range(u_starC2.getDimension()):
    print("standard space design point = %.5f" % u_starC2[i])
print(
    "************************************************************************************************"
)
for i in range(x_starC2.getDimension()):
    print("physical space design point = %.5f" % x_starC2[i])
print(
    "************************************************************************************************"
)
print(
    "************************************************************************************************"
)
print("is standard point origin in failure space? ", PtC2)
print(
    "************************************************************************************************"
)
for i in range(gammaC2.getDimension()):
    print("importance factors = %.5f" % gammaC2[i])
print(
    "************************************************************************************************"
)
print("Hasofer reliability index = %.5f" % beta_hasoferC2)
print(
    "************************************************************************************************"
)
print("")
print(
    "************************************************************************************************"
)
print(
    "************************************** SORM ABDO RACKWITZ **************************************"
)
print(
    "************************************************************************************************"
)
print("Breitung event probability = %.5e" % PFBreitAR2)
print("Breitung generalized reliability index = %.5f" % BetaBreitAR2)
print("Hohenbichler event probability = %.5e" % PFHBAR2)
print("Hohenbichler generalized reliability index = %.5f" % BetaHBAR2)
print("Tvedt event probability = %.5e" % PFTvedtAR2)
print("Tvedt generalized reliability index = %.5f" % BetaTvedtAR2)
print(
    "************************************************************************************************"
)
for i in range(CurvAR2.getDimension()):
    print("sorted curvatures = %.5f" % cleanScalar(CurvAR2[i]))
print(
    "************************************************************************************************"
)
for i in range(u_starAR2.getDimension()):
    print("standard space design point = %.5f" % u_starAR2[i])
print(
    "************************************************************************************************"
)
for i in range(x_starAR2.getDimension()):
    print("physical space design point = %.5f" % x_starAR2[i])
print(
    "************************************************************************************************"
)
print(
    "************************************************************************************************"
)
print("is standard point origin in failure space? ", PtAR2)
print(
    "************************************************************************************************"
)
for i in range(gammaAR2.getDimension()):
    print("importance factors = %.5f" % gammaAR2[i])
print(
    "************************************************************************************************"
)
print("Hasofer reliability index = %.5f" % beta_hasoferAR2)
print(
    "************************************************************************************************"
)
print("")
print(
    "************************************************************************************************"
)
print(
    "**************************************** MONTE CARLO *******************************************"
)
print(
    "************************************************************************************************"
)
print("Pf estimation = %.5e" % PFMC)
print("Pf Variance estimation = %.5e" % Variance_PF_MC)
print("CoV = %.5f" % CVMC)
print("90% Confidence Interval =", "%.5e" % length90MC)
print(
    "CI at 90% =[",
    "%.5e" % (PFMC - 0.5 * length90MC),
    "; %.5e" % (PFMC + 0.5 * length90MC),
    "]",
)
print(
    "************************************************************************************************"
)
print("")
print(
    "************************************************************************************************"
)
print(
    "******************************************* L H S **********************************************"
)
print(
    "************************************************************************************************"
)
print("Pf estimation = %.5e" % PFLHS)
print("Pf Variance estimation = %.5e" % Variance_PF_LHS)
print("CoV = %.5f" % CVLHS)
print("90% Confidence Interval =", "%.5e" % length90LHS)
print(
    "CI at 90% =[",
    "%.5e" % (PFLHS - 0.5 * length90LHS),
    "; %.5e" % (PFLHS + 0.5 * length90LHS),
    "]",
)
print(
    "************************************************************************************************"
)
print("")
