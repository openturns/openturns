#!/usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# QQPlot tests
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)
sample2 = ot.Gamma(3.0, 4.0, 0.0).getSample(size)
twoSamplesQQPlot = ot.VisualTest.DrawQQplot(sample, sample2)
print("twoSamplesQQPlot = ", twoSamplesQQPlot)

sampleDistributionQQPlot = ot.VisualTest.DrawQQplot(sample, normal)
print("sampleDistributionQQPlot = ", sampleDistributionQQPlot)

dist = ot.Geometric()
qq_plot = ot.VisualTest.DrawQQplot(dist.getSample(size), dist)
print("discrete QQPlot = ", qq_plot)

# HenryLine test
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)
henryPlot = ot.VisualTest.DrawHenryLine(sample)
print("HenryPlot = ", henryPlot)

# LinearModel tests
dimension = 2
R = ot.CorrelationMatrix(dimension)
R[0, 1] = 0.8
distribution = ot.Normal(ot.Point(dimension, 3.0), ot.Point(dimension, 2.0), R)
size = 100
sample2D = distribution.getSample(size)
firstSample = ot.Sample(size, 1)
secondSample = ot.Sample(size, 1)
for i in range(size):
    firstSample[i] = ot.Point(1, sample2D[i, 0])
    secondSample[i] = ot.Point(1, sample2D[i, 1])

lmtest = ot.LinearModelAlgorithm(firstSample, secondSample).getResult()
drawLinearModelVTest = ot.VisualTest.DrawLinearModel(lmtest)
print("LinearModelV = ", drawLinearModelVTest)

drawLinearModelResidualTest = ot.VisualTest.DrawLinearModelResidual(lmtest)
print("LinearModelR = ", drawLinearModelResidualTest)

# CobWeb tests
size = 100
inputDimension = 6
inputSample = ot.Normal(inputDimension).getSample(size)
inputVar = ["X" + str(i) for i in range(inputDimension)]
formula = ot.Description(1)
expression = ""
for i in range(inputDimension):
    if i > 0:
        expression += "+"
    expression += "cos(" + str(i + 1) + "*" + inputVar[i] + ")"
formula[0] = expression
model = ot.SymbolicFunction(inputVar, formula)
outputSample = model(inputSample)
cobwebValue = ot.VisualTest.DrawParallelCoordinates(
    inputSample, outputSample, 2.5, 3.0, "red", False
)
print("cobwebValue = ", cobwebValue)

cobwebQuantile = ot.VisualTest.DrawParallelCoordinates(
    inputSample, outputSample, 0.7, 0.9, "red", False
)
print("cobwebQuantile = ", cobwebQuantile)

# KendallPlot tests
size = 100
copula1 = ot.FrankCopula(1.5)
copula2 = ot.GumbelCopula(4.5)
sample1 = copula1.getSample(size)
sample1.setName("data 1")
sample2 = copula2.getSample(size)
sample2.setName("data 2")
kendallPlot1 = ot.VisualTest.DrawKendallPlot(sample1, copula2)
print("KendallPlot1 = ", kendallPlot1)

kendallPlot2 = ot.VisualTest.DrawKendallPlot(sample2, sample1)
print("KendallPlot2 = ", kendallPlot2)

# Clouds
sample = ot.Normal(4).getSample(200)
clouds = ot.VisualTest.DrawPairs(sample)
print("Clouds = ", clouds)
distribution = ot.ComposedDistribution(
    [ot.HistogramFactory().build(sample.getMarginal(i)) for i in range(4)]
)
cloudsMarginals = ot.VisualTest.DrawPairsMarginals(sample, distribution)
print("CloudsMarginals = ", cloudsMarginals)
