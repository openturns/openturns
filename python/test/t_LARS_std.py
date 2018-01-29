#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    # Log.Show( Log.Flags() | Log.INFO )

    # Problem parameters
    dimension = 3
    a = 7.0
    b = 0.1
    # Reference analytical values
    meanTh = a / 2
    covTh = (b ** 2 * pi ** 8) / 18.0 + \
        (b * pi ** 4) / 5.0 + (a ** 2) / 8.0 + 1.0 / 2.0
    sob_1 = [(b * pi ** 4 / 5.0 + b ** 2 * pi ** 8 / 50.0 + 1.0 / 2.0)
             / covTh, (a ** 2 / 8.0) / covTh, 0.0]
    sob_2 = [
        0.0, (b ** 2 * pi ** 8 / 18.0 - b ** 2 * pi ** 8 / 50.0) / covTh, 0.0]
    sob_3 = [0.0]
    sob_T1 = [sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0], sob_1[1] + sob_2[0]
              + sob_2[2] + sob_3[0], sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0]]
    sob_T2 = [sob_2[0] + sob_2[1] + sob_3[0], sob_2[0]
              + sob_2[2] + sob_3[0], sob_2[1] + sob_2[2] + sob_3[0]]
    sob_T3 = [sob_3[0]]
    # Create the Ishigami function
    model = SymbolicFunction(['xi1', 'xi2', 'xi3'], [
        "sin(xi1) + (" + str(a) + ") * (sin(xi2)) ^ 2 + (" + str(b) + ") * xi3^4 * sin(xi1)"])

    # Create the input distribution
    marginalX = DistributionCollection(dimension)
    for i in range(dimension):
        marginalX[i] = Uniform(-pi, pi)
    distribution = Distribution(ComposedDistribution(marginalX))

    # Create the orthogonal basis
    polynomialCollection = PolynomialFamilyCollection(dimension)
    for i in range(dimension):
        polynomialCollection[
            i] = OrthogonalUniVariatePolynomialFamily(LegendreFactory())
    enumerateFunction = LinearEnumerateFunction(dimension)
    productBasis = OrthogonalBasis(
        OrthogonalProductPolynomialFactory(polynomialCollection, enumerateFunction))

    # design experiment
    samplingSize = 75
    experiment = Experiment(LowDiscrepancyExperiment(
        SobolSequence(dimension), distribution, samplingSize))

    # iso transfo
    algo = FunctionalChaosAlgorithm(model, distribution, AdaptiveStrategy(
        FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(1))))
    algo.run()
    result = algo.getResult()
    xToU = result.getTransformation()

    # generate samples
    x = experiment.generate()
    u = xToU(x)
    y = model(x)

    # build basis
    degree = 10
    basisSize = enumerateFunction.getStrataCumulatedCardinal(degree)
    basis = [productBasis.build(i) for i in range(basisSize)]

    # run algorithm
    factory = BasisSequenceFactory(LARS())
    factory.setVerbose(True)
    print("factory = ", factory)

    seq = factory.build(u, y, basis, list(range(basisSize)))

    first = 20
    if (seq.getSize() >= first):
        print("first ", first, " indices = ", seq.getIndices(first - 1))

except:
    import sys
    print("t_LARS.py", sys.exc_info()[0], sys.exc_info()[1])
