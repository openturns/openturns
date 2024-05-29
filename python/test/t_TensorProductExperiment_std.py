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


def testTensorProductExperiment1():
    # Generate a tensorized Gauss-Legendre rule in 2 dimensions.
    # The first marginal has 3 nodes.
    # The first marginal has 5 nodes.
    experiment1 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0), [3])
    experiment2 = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0), [5])
    collection = [experiment1, experiment2]
    multivariateExperiment = ot.TensorProductExperiment(collection)
    nodes, weights = multivariateExperiment.generateWithWeights()
    # Sort
    nodes, weights = sortNodesAndWeights(nodes, weights)
    nodesExact = [
        [0.11270, 0.04691],
        [0.11270, 0.23076],
        [0.11270, 0.5],
        [0.11270, 0.76923],
        [0.11270, 0.95309],
        [0.5, 0.04691],
        [0.5, 0.23076],
        [0.5, 0.5],
        [0.5, 0.76923],
        [0.5, 0.95309],
        [0.88729, 0.04691],
        [0.88729, 0.23076],
        [0.88729, 0.5],
        [0.88729, 0.76923],
        [0.88729, 0.95309],
    ]
    weightsExact = [
        0.0329065,
        0.0664762,
        0.0790123,
        0.0664762,
        0.0329065,
        0.0526504,
        0.106362,
        0.12642,
        0.106362,
        0.0526504,
        0.0329065,
        0.0664762,
        0.0790123,
        0.0664762,
        0.0329065,
    ]
    rtol = 0.0
    atol = 1.0e-5
    ott.assert_almost_equal(nodes, nodesExact, rtol, atol)
    ott.assert_almost_equal(weights, weightsExact, rtol, atol)
    #
    size = multivariateExperiment.getSize()
    assert size == 15
    #
    distribution = multivariateExperiment.getDistribution()
    collection = [ot.Uniform(0.0, 1.0), ot.Uniform(0.0, 1.0)]
    expected_distribution = ot.BlockIndependentDistribution(collection)
    assert distribution == expected_distribution


def testTensorProductExperiment2():
    # Generate a tensorized Gauss-Legendre rule in 3 dimensions.
    # Each marginal elementary experiment has 6 nodes.
    dimension = 3
    maximumMarginalLevel = 6
    marginalLevels = [maximumMarginalLevel] * dimension
    collection = []
    for i in range(dimension):
        marginalExperiment = ot.GaussProductExperiment(
            ot.Uniform(0.0, 1.0), [marginalLevels[i]]
        )
        collection.append(marginalExperiment)
    multivariateExperiment = ot.TensorProductExperiment(collection)
    nodes, weights = multivariateExperiment.generateWithWeights()
    assert nodes.getDimension() == 3
    size = nodes.getSize()
    assert size == weights.getDimension()
    assert size == maximumMarginalLevel**dimension


def testTensorProductExperiment3():
    # Experiment 1 : Uniform * 2 with 3 and 2 nodes.
    marginalSizes1 = [3, 2]
    dimension1 = len(marginalSizes1)
    distribution1 = ot.JointDistribution([ot.Uniform()] * dimension1)
    experiment1 = ot.GaussProductExperiment(distribution1, marginalSizes1)
    # Experiment 2 : Normal * 3 with 2, 2 and 1 nodes.
    marginalSizes2 = [2, 2, 1]
    dimension2 = len(marginalSizes2)
    distribution2 = ot.JointDistribution([ot.Normal()] * dimension2)
    experiment2 = ot.GaussProductExperiment(distribution2, marginalSizes2)
    # Tensor product
    collection = [experiment1, experiment2]
    multivariateExperiment = ot.TensorProductExperiment(collection)
    nodes, weights = multivariateExperiment.generateWithWeights()
    # Sort
    nodes, weights = sortNodesAndWeights(nodes, weights)
    assert nodes.getDimension() == 5
    assert nodes.getSize() == 24
    assert weights.getSize() == 24

    nodesExact = [
        [
            -0.77459,
            -0.57735,
            -1.0,
            -1.0,
            0.0,
        ],
        [
            -0.77459,
            -0.57735,
            -1.0,
            1.0,
            0.0,
        ],
        [
            -0.77459,
            -0.57735,
            1.0,
            -1.0,
            0.0,
        ],
        [
            -0.77459,
            -0.57735,
            1.0,
            1.0,
            0.0,
        ],
        [
            -0.77459,
            0.57735,
            -1.0,
            -1.0,
            0.0,
        ],
        [
            -0.77459,
            0.57735,
            -1.0,
            1.0,
            0.0,
        ],
        [
            -0.77459,
            0.57735,
            1.0,
            -1.0,
            0.0,
        ],
        [
            -0.77459,
            0.57735,
            1.0,
            1.0,
            0.0,
        ],
        [
            0.0,
            -0.57735,
            -1.0,
            -1.0,
            0.0,
        ],
        [
            0.0,
            -0.57735,
            -1.0,
            1.0,
            0.0,
        ],
        [
            0.0,
            -0.57735,
            1.0,
            -1.0,
            0.0,
        ],
        [
            0.0,
            -0.57735,
            1.0,
            1.0,
            0.0,
        ],
        [
            0.0,
            0.57735,
            -1.0,
            -1.0,
            0.0,
        ],
        [
            0.0,
            0.57735,
            -1.0,
            1.0,
            0.0,
        ],
        [
            0.0,
            0.57735,
            1.0,
            -1.0,
            0.0,
        ],
        [
            0.0,
            0.57735,
            1.0,
            1.0,
            0.0,
        ],
        [
            0.77459,
            -0.57735,
            -1.0,
            -1.0,
            0.0,
        ],
        [
            0.77459,
            -0.57735,
            -1.0,
            1.0,
            0.0,
        ],
        [
            0.77459,
            -0.57735,
            1.0,
            -1.0,
            0.0,
        ],
        [
            0.77459,
            -0.57735,
            1.0,
            1.0,
            0.0,
        ],
        [
            0.77459,
            0.57735,
            -1.0,
            -1.0,
            0.0,
        ],
        [
            0.77459,
            0.57735,
            -1.0,
            1.0,
            0.0,
        ],
        [
            0.77459,
            0.57735,
            1.0,
            -1.0,
            0.0,
        ],
        [
            0.77459,
            0.57735,
            1.0,
            1.0,
            0.0,
        ],
    ]
    weightsExact = [
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0555556,
        0.0555556,
        0.0555556,
        0.0555556,
        0.0555556,
        0.0555556,
        0.0555556,
        0.0555556,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
        0.0347222,
    ]
    rtol = 0.0
    atol = 1.0e-5
    ott.assert_almost_equal(nodes, nodesExact, rtol, atol)
    ott.assert_almost_equal(weights, weightsExact, rtol, atol)


# Testing
testTensorProductExperiment1()
testTensorProductExperiment2()
testTensorProductExperiment3()


# Check polynomial degree of exactness
def checkPolynomialExactness(
    marginalDegrees,
    marginalSizes,
    lowerBound=0.0,
    upperBound=1.0,
    rtol=1.0e-15,
    atol=0.0,
    verbose=False,
):
    """
    Check polynomial exactness of Gauss tensor product quadrature

    Parameters
    ----------
    marginalDegrees : list of int
        The polynomial degree of the marginal polynomials to integrate
    marginalSizes : list of int
        The number of nodes on each marginal axis.

    Examples
    --------
    marginalDegrees = [5, 3, 7]
    marginalSizes = [3, 2, 4]  # Polynomial exactness space = P5 x P3 x P7
    checkPolynomialExactness(marginalDegrees, marginalSizes)
    """
    dimension = len(marginalDegrees)
    if len(marginalSizes) != dimension:
        raise ValueError(
            f"Number of marginal degrees is {dimension} "
            f"but number of marginal sizes is {len(marginalSizes)}"
        )

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

    # Create Gauss tensor product quadrature
    lowerBound = bounds.getLowerBound()
    upperBound = bounds.getUpperBound()
    collection = [
        ot.GaussProductExperiment(ot.Uniform(lowerBound[i], upperBound[i]))
        for i in range(dimension)
    ]
    experiment = ot.TensorProductExperiment(collection)

    # Evaluate integral
    nodes, weights = experiment.generateWithWeights()
    values = productPoly(nodes).asPoint()
    computedIntegral = weights.dot(values)

    # Expected integral
    expectedIntegral = 1.0
    for i in range(dimension):
        marginalIntegral = (
            upperBound[i] ** (1 + marginalDegrees[i])
            - lowerBound[i] ** (1 + marginalDegrees[i])
        ) / (1 + marginalDegrees[i])
        expectedIntegral *= marginalIntegral
    absoluteError = abs(computedIntegral - expectedIntegral)
    relativeError = abs(computedIntegral - expectedIntegral) / abs(expectedIntegral)
    if verbose:
        print(
            f"Polynomial : {str(productPoly):20s}, "
            f"  integral computed  = {computedIntegral:.7f}, "
            f"  expected = {expectedIntegral:.7f}, "
            f"  abs. err. = {absoluteError:.3e}"
            f"  rel. err. = {relativeError:.3e}"
        )
    ott.assert_almost_equal(expectedIntegral, computedIntegral, rtol, atol)
    return


# Test different polynomials, up to the maximum
marginalSizes = [3, 2, 4]  # Polynomial exactness space = P5 x P3 x P7
maximumMarginalDegrees = [2 * n for n in marginalSizes]
experiment = ot.Tuples(maximumMarginalDegrees)
marginalDegreesList = experiment.generate()
for i in range(marginalDegreesList.getSize()):
    marginalDegrees = marginalDegreesList[i]
    checkPolynomialExactness(
        marginalDegrees, marginalSizes, rtol=1.0e-14, verbose=False
    )
