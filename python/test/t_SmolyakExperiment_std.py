#! /usr/bin/env python

from __future__ import print_function
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
    factor = 10.0 ** numberOfDigits
    for i in range(size):
        for j in range(dimension):
            rounded = (int)(factor * sample[i, j])
            sample[i, j] = rounded / factor
    return sample


def testSmolyakExperiment1():
    # Generate a Smolyak Gauss-Legendre rule in 2 dimensions.
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
    collection = [ot.Uniform(0.0, 1.0), ot.Uniform(0.0, 1.0)]
    expected_distribution = ot.BlockIndependentDistribution(collection)
    assert distribution == expected_distribution


def testSmolyakExperiment2():
    # Generate a Smolyak Gauss-Legendre rule in 3 dimensions.
    # Each marginal elementary experiment has 6 nodes.
    dimension = 3
    marginalLevels = [maximumMarginalLevel] * dimension
    collection = []
    for i in range(dimension):
        marginalExperiment = ot.GaussProductExperiment(ot.Uniform(0.0, 1.0),)
        collection.append(marginalExperiment)
    level = 3
    smolyak = ot.SmolyakExperiment(collection, level)
    nodes, weights = smolyak.generateWithWeights()
    # Check size and dimension
    assert nodes.getDimension() == 3
    size = nodes.getSize()
    assert size == weights.getDimension()
    assert size == maximumMarginalLevel ** dimension


def testSmolyakExperiment3():
    # Experiment 1 : Uniform * 2 with 3 and 2 nodes.
    dimension1 = 2
    distribution1 = ot.ComposedDistribution([ot.Uniform()] * dimension1)
    experiment1 = ot.GaussProductExperiment(distribution1)
    # Experiment 2 : Normal * 3 with 2, 2 and 1 nodes.
    dimension2 = 3
    distribution2 = ot.ComposedDistribution([ot.Normal()] * dimension2)
    experiment2 = ot.GaussProductExperiment(distribution2)
    # Tensor product
    collection = [experiment1, experiment2]
    level = 3
    smolyak = ot.SmolyakExperiment(collection, level)
    nodes, weights = smolyak.generateWithWeights()
    # Sort
    nodes, weights = sortNodesAndWeights(nodes, weights)
    assert nodes.getDimension() == 5
    assert nodes.getSize() == 24
    assert weights.getSize() == 24

    nodesExact = [
        [-0.77459, -0.57735, -1.0, -1.0, 0.0,],
        [-0.77459, -0.57735, -1.0, 1.0, 0.0,],
        [-0.77459, -0.57735, 1.0, -1.0, 0.0,],
        [-0.77459, -0.57735, 1.0, 1.0, 0.0,],
        [-0.77459, 0.57735, -1.0, -1.0, 0.0,],
        [-0.77459, 0.57735, -1.0, 1.0, 0.0,],
        [-0.77459, 0.57735, 1.0, -1.0, 0.0,],
        [-0.77459, 0.57735, 1.0, 1.0, 0.0,],
        [0.0, -0.57735, -1.0, -1.0, 0.0,],
        [0.0, -0.57735, -1.0, 1.0, 0.0,],
        [0.0, -0.57735, 1.0, -1.0, 0.0,],
        [0.0, -0.57735, 1.0, 1.0, 0.0,],
        [0.0, 0.57735, -1.0, -1.0, 0.0,],
        [0.0, 0.57735, -1.0, 1.0, 0.0,],
        [0.0, 0.57735, 1.0, -1.0, 0.0,],
        [0.0, 0.57735, 1.0, 1.0, 0.0,],
        [0.77459, -0.57735, -1.0, -1.0, 0.0,],
        [0.77459, -0.57735, -1.0, 1.0, 0.0,],
        [0.77459, -0.57735, 1.0, -1.0, 0.0,],
        [0.77459, -0.57735, 1.0, 1.0, 0.0,],
        [0.77459, 0.57735, -1.0, -1.0, 0.0,],
        [0.77459, 0.57735, -1.0, 1.0, 0.0,],
        [0.77459, 0.57735, 1.0, -1.0, 0.0,],
        [0.77459, 0.57735, 1.0, 1.0, 0.0,],
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
testSmolyakExperiment1()
testSmolyakExperiment2()
testSmolyakExperiment3()
