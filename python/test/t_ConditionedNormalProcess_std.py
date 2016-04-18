#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import cos, sin

ot.TESTPREAMBLE()

try:

    # Fix numerical precision
    ot.PlatformInfo.SetNumericalPrecision(2)

    # Kriging use case
    spatialDimension = 2

    # Learning data
    levels = [8, 5]
    box = ot.Box(levels)
    inputSample = box.generate()
    # Scale each direction
    inputSample *= 10

    # Define model
    model = ot.NumericalMathFunction(
        ['x', 'y'], ['z'], ['cos(0.5*x) + sin(y)'])
    outputSample = model(inputSample)

    # 2) Definition of exponential model
    covarianceModel = ot.SquaredExponential([1.98824, 0.924731], [3.15352])

    # 3) Basis definition
    basisCollection = ot.BasisCollection(
        1, ot.ConstantBasisFactory(spatialDimension).build())

    # Kriring algorithm
    algo = ot.KrigingAlgorithm(
        inputSample, outputSample, basisCollection, covarianceModel)
    algo.run()
    result = algo.getResult()

    vertices = [[1.0, 0.0], [2.0, 0.0], [2.0, 1.0],
                [1.0, 1.0], [1.5, 0.5]]
    simplicies = [[0, 1, 4], [1, 2, 4], [2, 3, 4], [3, 0, 4]]

    mesh2D = ot.Mesh(vertices, simplicies)
    process = ot.ConditionedNormalProcess(result, mesh2D)

    # Get a realization of the process
    realization = process.getRealization()
    print("realization = ", repr(realization))

    # Get a sample & compare it to expectation
    sample = process.getSample(5000)
    mean = sample.computeMean()
    print("Mean over 5000 realizations = ", repr(mean))

except:
    import sys
    print("t_ConditionedNormalProcess_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
