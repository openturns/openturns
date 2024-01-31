#! /usr/bin/env python

import math
import openturns as ot
from openturns.testing import assert_almost_equal

# Definition of limit state function


def four_branch(x):
    x1 = x[0]
    x2 = x[1]
    k = x[2]

    g1 = 3 + 0.1 * (x1 - x2) ** 2 - (x1 + x2) / math.sqrt(2)
    g2 = 3 + 0.1 * (x1 - x2) ** 2 + (x1 + x2) / math.sqrt(2)
    g3 = (x1 - x2) + k / math.sqrt(2)
    g4 = (x2 - x1) + k / math.sqrt(2)

    return [min((g1, g2, g3, g4))]


# Definition of a pythonfunction
my_four_branch = ot.PythonFunction(3, 1, four_branch)

# Transformation of pythonfunction to parametricfunction fixing parameter k
index_frozen = [2]
my_four_branch_6 = ot.ParametricFunction(my_four_branch, index_frozen, [6])
my_four_branch_7 = ot.ParametricFunction(my_four_branch, index_frozen, [7])

# Definition of input variable PDF

dim_inputs = 2
dist_x = ot.Normal([0.0, 0.0], [1.0, 1.0], ot.CorrelationMatrix(dim_inputs))
inputVector = ot.RandomVector(dist_x)

# Determination of reference probability
# MonteCarlo experiment

n_MC = int(1e6)

# Creation of event
ot.RandomGenerator.SetSeed(1)

vect = ot.RandomVector(dist_x)
G = ot.CompositeRandomVector(my_four_branch_7, vect)
event = ot.ThresholdEvent(G, ot.Less(), 0.0)

# create a Monte Carlo algorithm
experiment = ot.MonteCarloExperiment()
algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
algo.setMaximumOuterSampling(int(n_MC))
algo.run()

# retrieve results
result = algo.getResult()
probability = result.getProbabilityEstimate()
assert_almost_equal(probability, 0.0023828813801648916)

# Hyperparameters of the algorithm
numberSamples = 10000  # Number of samples at each iteration
blockSize = 1
# Quantile determining the percentage of failure samples in the current population
quantileLevel = 0.25

# Definition of the algorithm
Nais_algo = ot.NAIS(event, quantileLevel)
Nais_algo.setMaximumOuterSampling(numberSamples)
Nais_algo.setBlockSize(blockSize)
Nais_algo.setKeepSample(True)

# Run of the algorithm
Nais_algo.run()
NAIS_result = Nais_algo.getResult()

assert_almost_equal(NAIS_result.getProbabilityEstimate(), 0.00215688)


# check that the event sample is right
stepsNumber = Nais_algo.getStepsNumber()
inputEventSample = Nais_algo.getInputSample(stepsNumber - 1, Nais_algo.EVENT1)
outputEventSample = Nais_algo.getOutputSample(stepsNumber - 1, Nais_algo.EVENT1)
outputG = my_four_branch_7(inputEventSample)
diffSample = outputG - outputEventSample
assert_almost_equal(diffSample.computeMean(), [0.0])




