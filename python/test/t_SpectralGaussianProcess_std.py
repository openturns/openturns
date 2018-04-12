#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Default dimension parameter to evaluate the model
    defaultDimension = 1

    # Amplitude values
    amplitude = Point(defaultDimension, 1.0)
    # Scale values
    scale = Point(defaultDimension, 1.0)

    # Spectral density
    myModel = CauchyModel(scale, amplitude)

    points = 8
    tMin = 0.0
    tStep = 1.0 / (points - 1)

    # RegularGrid --> Build list of frequencies using the RegularGrid
    myTimeGrid = RegularGrid(tMin, tStep, points)

    mySpectralProcess0 = SpectralGaussianProcess(myModel, myTimeGrid)

    print("mySpectralProcess0 = ", mySpectralProcess0)
    print("Realization = ", mySpectralProcess0.getRealization())

    # Constructor using maximalFrequency value and size of discretization
    maximalFrequency = 10.0
    mySpectralProcess1 = SpectralGaussianProcess(
        myModel, maximalFrequency, points)
    tg = RegularGrid(mySpectralProcess1.getTimeGrid())

    print("mySpectralProcess1 = ", mySpectralProcess1)
    print("Realization = ", mySpectralProcess1.getRealization())

    # 3D model
    highDimension = 3
    amplitude = Point(highDimension, 1.0)

    # Second order model with parameters
    mySpecModel = CauchyModel(scale, amplitude)
    print("mySpecModel = ", mySpecModel)

    mySpectralProcess2 = SpectralGaussianProcess(mySpecModel, myTimeGrid)
    print("mySpectralProcess2 = ", mySpectralProcess2)
    print("Realization = ", mySpectralProcess2.getRealization())
    mySpectralProcess3 = SpectralGaussianProcess(
        mySpecModel, maximalFrequency, points)
    print("mySpectralProcess3 = ", mySpectralProcess3)
    print("Realization = ", mySpectralProcess3.getRealization())

except:
    import sys
    print("t_SpectralGaussianProcess_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
