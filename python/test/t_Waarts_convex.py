#! /usr/bin/env python

import openturns as ot

# from math import *

ot.TESTPREAMBLE()

#
# Physical model
#

inputFunction = ot.Description(2)
inputFunction[0] = "X1"
inputFunction[1] = "X2"

outputFunction = ot.Description(1)
outputFunction[0] = "G"

formulas = ot.Description(outputFunction.getSize())
formulas[0] = "2.5 + 0.1*(X1-X2)^2.0 - (X1+X2)/sqrt(2.0)"

EtatLimite = ot.SymbolicFunction(inputFunction, outputFunction, formulas)

dim = EtatLimite.getInputDimension()
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

output = ot.RandomVector(EtatLimite, vect)

myEvent = ot.ThresholdEvent(output, ot.Less(), 0.0)

#
# Calculs
#
#
# FORM/SORM Cobyla
myCobyla = ot.Cobyla()
myCobyla.setMaximumCallsNumber(100 * dim)
myCobyla.setMaximumAbsoluteError(1.0e-10)
myCobyla.setMaximumRelativeError(1.0e-10)
myCobyla.setMaximumResidualError(1.0e-10)
myCobyla.setMaximumConstraintError(1.0e-10)

myAlgoC = ot.FORM(myCobyla, myEvent, start)
myAlgoC2 = ot.SORM(myCobyla, myEvent, start)

myAlgoC.run()
myAlgoC2.run()

resultC = myAlgoC.getResult()
resultC2 = myAlgoC2.getResult()

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
myMC = ot.MonteCarlo(myEvent)
myMC.setMaximumOuterSampling(100000)
myMC.setBlockSize(1)
myMC.setMaximumCoefficientOfVariation(CoV_MC)
myMC.run()

#
# LHS
CoV_LHS = 0.1
myLHS = ot.LHS(myEvent)
myLHS.setMaximumOuterSampling(100000)
myLHS.setBlockSize(1)
myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
myLHS.run()
