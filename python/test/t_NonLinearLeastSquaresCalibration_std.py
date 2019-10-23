#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import math as m
import sys

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)

m = 10
x = [[0.5 + i] for i in range(m)]

#ot.ResourceMap.SetAsUnsignedInteger( "OptimizationAlgorithm-DefaultMaximumEvaluationNumber", 100)
inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
model = ot.SymbolicFunction(inVars, formulas)
p_ref = [2.8, 1.2, 0.5]
params = [0, 1, 2]
modelX = ot.ParametricFunction(model, params, p_ref)
y = modelX(x)
y += ot.Normal([0.0]*2, [0.05]*2, ot.IdentityMatrix(2)).getSample(m)
candidate = [1.0]*3
bootstrapSizes = [0, 100]
for bootstrapSize in bootstrapSizes:
    algo = ot.NonLinearLeastSquaresCalibration(modelX, x, y, candidate)
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    # To avoid discrepance between the plaforms with or without CMinpack
    print("result (Auto)=", algo.getResult().getParameterMAP())
    algo.setOptimizationAlgorithm(ot.MultiStart(ot.TNC(), ot.LowDiscrepancyExperiment(ot.SobolSequence(), ot.Normal(candidate, ot.CovarianceMatrix(
        ot.Point(candidate).getDimension())), ot.ResourceMap.GetAsUnsignedInteger("NonLinearLeastSquaresCalibration-MultiStartSize")).generate()))
    algo.run()
    # To avoid discrepance between the plaforms with or without CMinpack
    print("result  (TNC)=", algo.getResult().getParameterMAP())
