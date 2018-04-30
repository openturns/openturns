#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Dimension of the input model
    # Size of the TimeGrid
    size = 64
    dimension = 1
    timeGrid = RegularGrid(0.0, 0.1, size)
    amplitude = Point(dimension, 2.0)
    scale = Point(dimension, 1.0)
    model = CauchyModel(scale, amplitude)
    myProcess = SpectralGaussianProcess(model, timeGrid)

    # Create a Process sample
    N = 100
    sample = ProcessSample(myProcess.getSample(N))

    # Filtering Windows
    myFactory = WelchFactory()

    # Build a UserDefinedSpectralModel using the Wellch method
    mySpectralModel = myFactory.buildAsUserDefinedSpectralModel(sample)

    # Get the frequency grid of the model
    myFrequencyGrid = mySpectralModel.getFrequencyGrid()
    for i in range(dimension):
        for j in range(dimension):
            print("Spectre ", i, "-", j)
            for k in range(myFrequencyGrid.getN()):
                frequency = myFrequencyGrid.getStart(
                ) + k * myFrequencyGrid.getStep()
                estimatedValue = (
                    mySpectralModel(frequency)[i, j]).real
                modelValue = (
                    model(frequency)[i, j]).real
                print("Frequency =  %.6f" % frequency, ", evaluation = %.8f" %
                      estimatedValue, " model = %.8f" % modelValue)

    # Create a Time Series
    timeSeries = myProcess.getRealization()

    # Build a UserDefinedSpectralModel using the Wellch method
    mySpectralModel2 = myFactory.buildAsUserDefinedSpectralModel(timeSeries)

    # Get the frequency grid of the model
    myFrequencyGrid = mySpectralModel2.getFrequencyGrid()
    for i in range(dimension):
        for j in range(dimension):
            print("Spectre ", i, "-", j)
            for k in range(myFrequencyGrid.getN()):
                frequency = myFrequencyGrid.getStart(
                ) + k * myFrequencyGrid.getStep()
                estimatedValue = (
                    mySpectralModel2(frequency)[i, j]).real
                modelValue = (
                    model(frequency)[i, j]).real
                print("Frequency =  %.6f" % frequency, ", evaluation = %.8f" %
                      estimatedValue, " model = %.8f" % modelValue)

except:
    import sys
    print("t_WelchFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
