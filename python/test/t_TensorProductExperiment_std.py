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
    atol = 1.e-5
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
    assert size == maximumMarginalLevel ** dimension


def testTensorProductExperiment3():
    # Experiment 1 : Uniform * 2 with 3 and 2 nodes.
    marginalSizes1 = [3, 2]
    dimension1 = len(marginalSizes1)
    distribution1 = ot.ComposedDistribution([ot.Uniform()] * dimension1)
    experiment1 = ot.GaussProductExperiment(distribution1, marginalSizes1)
    # Experiment 2 : Normal * 3 with 2, 2 and 1 nodes.
    marginalSizes2 = [2, 2, 1]
    dimension2 = len(marginalSizes2)
    distribution2 = ot.ComposedDistribution([ot.Normal()] * dimension2)
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
        [-0.77459, -0.57735, -1.0, -1.0, 0.0, ],
        [-0.77459, -0.57735, -1.0, 1.0, 0.0, ],
        [-0.77459, -0.57735, 1.0, -1.0, 0.0, ],
        [-0.77459, -0.57735, 1.0, 1.0, 0.0, ],
        [-0.77459, 0.57735, -1.0, -1.0, 0.0, ],
        [-0.77459, 0.57735, -1.0, 1.0, 0.0, ],
        [-0.77459, 0.57735, 1.0, -1.0, 0.0, ],
        [-0.77459, 0.57735, 1.0, 1.0, 0.0, ],
        [0.0, -0.57735, -1.0, -1.0, 0.0, ],
        [0.0, -0.57735, -1.0, 1.0, 0.0, ],
        [0.0, -0.57735, 1.0, -1.0, 0.0, ],
        [0.0, -0.57735, 1.0, 1.0, 0.0, ],
        [0.0, 0.57735, -1.0, -1.0, 0.0, ],
        [0.0, 0.57735, -1.0, 1.0, 0.0, ],
        [0.0, 0.57735, 1.0, -1.0, 0.0, ],
        [0.0, 0.57735, 1.0, 1.0, 0.0, ],
        [0.77459, -0.57735, -1.0, -1.0, 0.0, ],
        [0.77459, -0.57735, -1.0, 1.0, 0.0, ],
        [0.77459, -0.57735, 1.0, -1.0, 0.0, ],
        [0.77459, -0.57735, 1.0, 1.0, 0.0, ],
        [0.77459, 0.57735, -1.0, -1.0, 0.0, ],
        [0.77459, 0.57735, -1.0, 1.0, 0.0, ],
        [0.77459, 0.57735, 1.0, -1.0, 0.0, ],
        [0.77459, 0.57735, 1.0, 1.0, 0.0, ],
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
    atol = 1.e-5
    ott.assert_almost_equal(nodes, nodesExact, rtol, atol)
    ott.assert_almost_equal(weights, weightsExact, rtol, atol)


# Testing
testTensorProductExperiment1()
testTensorProductExperiment2()
testTensorProductExperiment3()
