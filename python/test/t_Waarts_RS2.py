#! /usr/bin/env python

import openturns as ot

# from math import *

ot.TESTPREAMBLE()


def cleanScalar(inScalar):
    if abs(inScalar) < 1.0e-10:
        inScalar = 0.0
    return inScalar


#
# Physical model
#

inputFunction = ot.Description(2)
inputFunction[0] = "r"
inputFunction[1] = "s"

outputFunction = ot.Description(1)
outputFunction[0] = "g"

formulas = ot.Description(outputFunction.getSize())
formulas[0] = "r - s^2"

limitState = ot.SymbolicFunction(inputFunction, outputFunction, formulas)

dim = limitState.getInputDimension()

#
# Probabilistic model
#

mean = ot.Point(dim)
mean[0] = 11.0
mean[1] = 1.5

sigma = ot.Point(dim)
sigma[0] = 1.0
sigma[1] = 0.5

R = ot.CorrelationMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

start = myDistribution.getMean()

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
myMC = ot.MonteCarlo(myEvent)
myMC.setMaximumOuterSampling(1000000)
myMC.setBlockSize(1000)
myMC.setMaximumCoefficientOfVariation(CoV_MC)
myMC.run()

#
# LHS
CoV_LHS = 0.1
myLHS = ot.LHS(myEvent)
myLHS.setMaximumOuterSampling(1000000)
myLHS.setBlockSize(100)
myLHS.setMaximumCoefficientOfVariation(CoV_LHS)
myLHS.run()
