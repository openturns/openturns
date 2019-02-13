#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Generate a Normal sample
    normal = Normal(1)
    size = 100
    sample = normal.getSample(size)
    sampleCDF = VisualTest.DrawEmpiricalCDF(
        sample, sample.getMin()[0] - 1.0, sample.getMax()[0] + 1.0)
    print("sampleCDF = ", sampleCDF)

    sampleCDF = VisualTest.DrawEmpiricalCDF(sample, -0.5, 0.5)
    print("sampleCDFZoom = ", sampleCDF)

    # Histogram tests
    normal = Normal(1)
    size = 100
    sample = normal.getSample(size)
    sampleHist = VisualTest.DrawHistogram(sample, 10)
    print("sampleHist = ", sampleHist)

    sampleHist = VisualTest.DrawHistogram(sample)
    print("sampleHistOpt = ", sampleHist)

    # QQPlot tests
    size = 100
    normal = Normal(1)
    sample = normal.getSample(size)
    sample2 = Gamma(3.0, 4.0, 0.0).getSample(size)
    twoSamplesQQPlot = VisualTest.DrawQQplot(sample, sample2)
    print("twoSamplesQQPlot = ", twoSamplesQQPlot)

    sampleDistributionQQPlot = VisualTest.DrawQQplot(sample, normal)
    print("sampleDistributionQQPlot = ", sampleDistributionQQPlot)

    # HenryLine test
    size = 100
    normal = Normal(1)
    sample = normal.getSample(size)
    henryPlot = VisualTest.DrawHenryLine(sample)
    print("HenryPlot = ", henryPlot)

    # Clouds tests
    dimension = (2)
    R = CorrelationMatrix(dimension)
    R[0, 1] = 0.8
    distribution = Normal(
        Point(dimension, 3.0), Point(dimension, 2.0), R)
    size = 100
    sample2D = distribution.getSample(size)
    firstSample = Sample(size, 1)
    secondSample = Sample(size, 1)
    for i in range(size):
        firstSample[i] = Point(1, sample2D[i, 0])
        secondSample[i] = Point(1, sample2D[i, 1])

    sampleSampleClouds = VisualTest.DrawClouds(
        sample2D, Normal(Point(dimension, 2.0), Point(dimension, 3.0), R).getSample(size // 2))
    print("sampleSampleClouds = ", sampleSampleClouds)

    sampleDistributionClouds = VisualTest.DrawClouds(
        sample2D, Normal(Point(dimension, 2.5), Point(dimension, 1.0), R))
    print("sampleDistributionClouds = ", sampleDistributionClouds)

    # LinearModel tests
    dimension = 2
    R = CorrelationMatrix(dimension)
    R[0, 1] = 0.8
    distribution = Normal(
        Point(dimension, 3.0), Point(dimension, 2.0), R)
    size = 100
    sample2D = distribution.getSample(size)
    firstSample = Sample(size, 1)
    secondSample = Sample(size, 1)
    for i in range(size):
        firstSample[i] = Point(1, sample2D[i, 0])
        secondSample[i] = Point(1, sample2D[i, 1])

    lmtest = LinearModelAlgorithm(firstSample, secondSample).getResult()
    drawLinearModelVTest = VisualTest.DrawLinearModel(
        firstSample, secondSample, lmtest)
    print("LinearModelV = ", drawLinearModelVTest)

    drawLinearModelResidualTest = VisualTest.DrawLinearModelResidual(
        firstSample, secondSample, lmtest)
    print("LinearModelR = ", drawLinearModelResidualTest)

    # CobWeb tests
    size = 100
    inputDimension = 6
    inputSample = Normal(inputDimension).getSample(size)
    inputVar = Description(inputDimension)
    for i in range(inputDimension):
        inputVar[i] = "X" + str(i)
    formula = Description(1)
    expression = ""
    for i in range(inputDimension):
        if i > 0:
            expression += "+"
        expression += "cos(" + str(i + 1) + "*" + inputVar[i] + ")"
    formula[0] = expression
    model = SymbolicFunction(inputVar, formula)
    outputSample = model(inputSample)
    cobwebValue = VisualTest.DrawCobWeb(
        inputSample, outputSample, 2.5, 3.0, "red", False)
    print("cobwebValue = ", cobwebValue)

    cobwebQuantile = VisualTest.DrawCobWeb(
        inputSample, outputSample, 0.7, 0.9, "red", False)
    print("cobwebQuantile = ", cobwebQuantile)

    # KendallPlot tests
    size = 100
    copula1 = FrankCopula(1.5)
    copula2 = GumbelCopula(4.5)
    sample1 = copula1.getSample(size)
    sample1.setName("data 1")
    sample2 = copula2.getSample(size)
    sample2.setName("data 2")
    kendallPlot1 = VisualTest.DrawKendallPlot(sample1, copula2)
    print("KendallPlot1 = ", kendallPlot1)

    kendallPlot2 = VisualTest.DrawKendallPlot(sample2, sample1)
    print("KendallPlot2 = ", kendallPlot2)

except:
    import sys
    print("t_VisualTest_std.py", sys.exc_info()[0], sys.exc_info()[1])
