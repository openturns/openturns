#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


def sortNodesAndWeights(nodes, weights):
    """
    Sort nodes and weights of an Experiment.
    Parameters
    ----------
    nodes : ot.Sample(size, dimension)
        The sorted nodes.
    weights : ot.Point(size)
        The weights.

    Returns
    -------
    sortedNodes : ot.Sample(size, dimension)
        The nodes.
    sortedWeights : ot.Point(size)
        The sorted weights.
    """
    indices = nodes.argsort()
    size = nodes.getSize()
    if weights.getDimension() != size:
        raise ValueError(
            "Nodes have size %d, but weights have dimension %d"
            % (size, weights.getDimension())
        )
    sortedNodes = nodes[indices]
    sortedWeights = weights[indices]
    return sortedNodes, sortedWeights


def printNodesAndWeights(nodes, weights):
    """
    Print nodes and weights of an Experiment.
    Parameters
    ----------
    nodes : ot.Sample(size, dimension)
        The sorted nodes.
    weights : ot.Point(size)
        The weights.
    """
    size = nodes.getSize()
    if weights.getDimension() != size:
        raise ValueError(
            "Nodes have size %d, but weights have dimension %d"
            % (size, weights.getDimension())
        )
    dimension = nodes.getDimension()
    for i in range(size):
        row = "[%d] %.4f : " % (i, weights[i])
        for j in range(dimension):
            row += "%.4f, " % (nodes[i, j])
        print(row)
    return


def printNodes(nodes):
    """
    Print nodes of an Experiment.
    Parameters
    ----------
    nodes : ot.Sample(size, dimension)
        The sorted nodes.
    """
    size = nodes.getSize()
    dimension = nodes.getDimension()
    for i in range(size):
        row = "[%d] : " % (i)
        for j in range(dimension):
            row += "%.4f, " % (nodes[i, j])
        print(row)
    return


def roundSample(sample, numberOfDigits):
    """
    Round a sample

    Parameters
    ----------
    sample : ot.Sample(size, dimension)
        The sample.
    numberOfDigits : int
        The number of decimal digits to keep.

    Returns
    -------
    roundedSample : ot.Sample(size, dimension)
        The rounded sample.
    """
    size = sample.getSize()
    dimension = sample.getDimension()
    factor = 10.0**numberOfDigits
    for i in range(size):
        for j in range(dimension):
            rounded = (int)(factor * sample[i, j])
            sample[i, j] = rounded / factor
    return sample


def testSmolyakExperiment1():
    # Generate a Smolyak Gauss-Legendre rule in 2 dimensions.
    print("testSmolyakExperiment1:")
    experiment1 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    experiment2 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    collection = [experiment1, experiment2]
    level = 3
    smolyak = ot.SmolyakExperiment(collection, level)
    nodes, weights = smolyak.generateWithWeights()
    numberOfDigits = 14
    nodes = roundSample(nodes, numberOfDigits)
    nodes, weights = sortNodesAndWeights(nodes, weights)
    print("Computed:")
    printNodesAndWeights(nodes, weights)
    # Sort
    nodesExact = ot.Sample(
        [
            [0.112702, 0.5],
            [0.211325, 0.211325],
            [0.211325, 0.5],
            [0.211325, 0.788675],
            [0.5, 0.112702],
            [0.5, 0.211325],
            [0.5, 0.5],
            [0.5, 0.788675],
            [0.5, 0.887298],
            [0.788675, 0.211325],
            [0.788675, 0.5],
            [0.788675, 0.788675],
            [0.887298, 0.5],
        ]
    )
    weightsExact = ot.Point(
        [
            0.277778,
            0.25,
            -0.5,
            0.25,
            0.277778,
            -0.5,
            0.888888,
            -0.5,
            0.277778,
            0.25,
            -0.5,
            0.25,
            0.277778,
        ]
    )
    nodesExact, weightsExact = sortNodesAndWeights(nodesExact, weightsExact)
    print("Expected:")
    printNodesAndWeights(nodesExact, weightsExact)
    rtol = 0.0
    atol = 1.0e-5
    ott.assert_almost_equal(nodes, nodesExact, rtol, atol)
    ott.assert_almost_equal(weights, weightsExact, rtol, atol)
    #
    size = smolyak.getSize()
    print("size = ", size)
    assert size == 13
    #
    distribution = smolyak.getDistribution()
    collection = [ot.Uniform(0.0, 1.0)] * 2
    expected_distribution = ot.JointDistribution(collection)
    assert distribution == expected_distribution


def sortNodes(nodes):
    """
    Sort nodes of an Experiment.
    Parameters
    ----------
    nodes : ot.Sample(size, dimension)
        The sorted nodes.

    Returns
    -------
    sortedNodes : ot.Sample(size, dimension)
        The nodes.
    """
    indices = nodes.argsort()
    sortedNodes = nodes[indices]
    return sortedNodes


def testSmolyakExperiment2():
    # Test generate() method
    print("testSmolyakExperiment2:")
    experiment1 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    experiment2 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    collection = [experiment1, experiment2]
    level = 3
    smolyak = ot.SmolyakExperiment(collection, level)
    nodes = smolyak.generate()
    numberOfDigits = 14
    nodes = roundSample(nodes, numberOfDigits)
    nodes = sortNodes(nodes)
    print("Computed:")
    printNodes(nodes)
    nodesExact = ot.Sample(
        [
            [0.112702, 0.5],
            [0.211325, 0.211325],
            [0.211325, 0.5],
            [0.211325, 0.788675],
            [0.5, 0.112702],
            [0.5, 0.211325],
            [0.5, 0.5],
            [0.5, 0.788675],
            [0.5, 0.887298],
            [0.788675, 0.211325],
            [0.788675, 0.5],
            [0.788675, 0.788675],
            [0.887298, 0.5],
        ]
    )
    nodesExact = sortNodes(nodesExact)
    print("Exact:")
    printNodes(nodesExact)
    rtol = 0.0
    atol = 1.0e-5
    ott.assert_almost_equal(nodes, nodesExact, rtol, atol)


def testSmolyakExperiment3():
    # Generate a Smolyak Gauss-Legendre rule in 3 dimensions.
    # Each marginal elementary experiment has 6 nodes.
    print("testSmolyakExperiment3:")
    dimension = 3
    collection = []
    for i in range(dimension):
        marginalExperiment = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
        collection.append(marginalExperiment)
    level = 3
    smolyak = ot.SmolyakExperiment(collection, level)
    nodes, weights = smolyak.generateWithWeights()
    # Check size and dimension
    assert nodes.getDimension() == 3
    size = nodes.getSize()
    assert size == weights.getDimension()


def testSmolyakExperiment4():
    # Special case : Level = 1
    print("testSmolyakExperiment4:")
    ot.Log.Show(ot.Log.ALL)
    # Generate a Smolyak Gauss-Legendre rule in 2 dimensions.
    experiment1 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    experiment2 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    collection = [experiment1, experiment2]
    level = 1
    smolyak = ot.SmolyakExperiment(collection, level)
    nodes, weights = smolyak.generateWithWeights()
    numberOfDigits = 14
    nodes = roundSample(nodes, numberOfDigits)
    nodes, weights = sortNodesAndWeights(nodes, weights)
    print("Computed:")
    printNodesAndWeights(nodes, weights)
    # Sort
    nodesExact = ot.Sample([[0.5, 0.5]])
    weightsExact = ot.Point([1.0])
    nodesExact, weightsExact = sortNodesAndWeights(nodesExact, weightsExact)
    print("Expected:")
    printNodesAndWeights(nodesExact, weightsExact)
    rtol = 0.0
    atol = 1.0e-5
    ott.assert_almost_equal(nodes, nodesExact, rtol, atol)
    ott.assert_almost_equal(weights, weightsExact, rtol, atol)
    #
    size = smolyak.getSize()
    print("size = ", size)
    assert size == 1
    #
    distribution = smolyak.getDistribution()
    collection = [ot.Uniform(0.0, 1.0)] * 2
    expected_distribution = ot.JointDistribution(collection)
    assert distribution == expected_distribution


def testSmolyakExperiment5():
    # Special case : Level = 1
    print("testSmolyakExperiment5:")
    ot.Log.Show(ot.Log.ALL)
    # Generate a Smolyak Gauss-Legendre rule in 2 dimensions.
    experiment1 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    experiment2 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0))
    collection = [experiment1, experiment2]
    level = 3
    smolyak = ot.SmolyakExperiment(collection, level)
    indicesCollection = smolyak.computeCombination()
    print("indicesCollection = ", indicesCollection)
    expected = [[2, 1], [1, 2], [3, 1], [2, 2], [1, 3]]
    for i in range(len(indicesCollection)):
        assert indicesCollection[i] == expected[i]


# Testing
testSmolyakExperiment1()
testSmolyakExperiment2()
testSmolyakExperiment3()
testSmolyakExperiment4()
testSmolyakExperiment5()


# Check polynomial degree of exactness
def checkPolynomialExactness(
    marginalDegrees,
    level,
    lowerBound=0.0,
    upperBound=1.0,
    rtol=1.0e-15,
    atol=0.0,
    verbose=False,
):
    """
    Check polynomial exactness of Smolyak quadrature based on Gauss

    Parameters
    ----------
    marginalDegrees : list of int
        The polynomial degree of the marginal polynomials to integrate
    level : int
        The Smolyak level
    lowerBound : float
        The lower bound of quadrature
    upperBound : float
        The upper bound of quadrature
    rtol : float, > 0
        The relative tolerance
    atol : float, > 0
        The absolute tolerance
    verbose : bool
        Set to True to print intermediate messages

    Examples
    --------
    marginalDegrees = [5, 1]
    level = 3
    checkPolynomialExactness(marginalDegrees, level)
    """
    if lowerBound > upperBound:
        raise ValueError(f"The lower bound {lowerBound} is greater than "
                         f"the upper bound {upperBound}.")
    dimension = len(marginalDegrees)

    # Set bounds
    bounds = ot.Interval([lowerBound] * dimension, [upperBound] * dimension)

    # Create polynomial
    polynomialCollection = ot.PolynomialCollection()
    for i in range(dimension):
        coefficients = [0.0] * (1 + marginalDegrees[i])
        coefficients[-1] = 1
        polynomial = ot.UniVariatePolynomial(coefficients)
        polynomialCollection.add(polynomial)

    productPoly = ot.ProductPolynomialEvaluation(polynomialCollection)

    # Create Smolyak quadrature
    lowerBoundPoint = bounds.getLowerBound()
    upperBoundPoint = bounds.getUpperBound()
    experimentCollection = []
    for i in range(dimension):
        marginalDistribution = ot.Uniform(lowerBoundPoint[i], upperBoundPoint[i])
        marginalExperiment = ot.GaussProductExperiment(marginalDistribution)
        experimentCollection.append(marginalExperiment)
    experiment = ot.SmolyakExperiment(experimentCollection, level)

    # Evaluate integral
    nodes, weights = experiment.generateWithWeights()
    values = productPoly(nodes).asPoint()
    computedIntegral = weights.dot(values)

    # Expected integral
    expectedIntegral = 1.0
    for i in range(dimension):
        marginalIntegral = (
            upperBoundPoint[i] ** (1 + marginalDegrees[i])
            - lowerBoundPoint[i] ** (1 + marginalDegrees[i])
        ) / (1 + marginalDegrees[i])
        expectedIntegral *= marginalIntegral
    absoluteError = abs(computedIntegral - expectedIntegral)
    if verbose:
        print(
            f"Polynomial : {str(productPoly):20s}, "
            f"  integral computed  = {computedIntegral:.7f}, "
            f"  expected = {expectedIntegral:.7f}, "
            f"  absolute error = {absoluteError:.3e}"
        )
    ott.assert_almost_equal(expectedIntegral, computedIntegral, rtol, atol)


ot.Log.Show(ot.Log.NONE)
# Test different polynomials, up to the maximum
# Polynomial exactness space = P5 x P1 + P3 x P3 + P1 x P5
level = 3
marginalDegreesList = [v for v in ot.Tuples([6, 2]).generate()]
marginalDegreesList += [v for v in ot.Tuples([4, 4]).generate()]
marginalDegreesList += [v for v in ot.Tuples([2, 6]).generate()]
print(marginalDegreesList)
for i in range(len(marginalDegreesList)):
    marginalDegrees = marginalDegreesList[i]
    checkPolynomialExactness(marginalDegrees, level, verbose=True)
