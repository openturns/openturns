#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal
from openturns.usecases import four_branch_function

# Load the Four Branch function use case (standard benchmark: p=6, standard normal inputs)
fb = four_branch_function.FourBranchModel()

# Definition of input variable PDF (standard benchmark uses 2D standard normal)
dim_inputs = 2
dist_x = fb.distribution
inputVector = ot.RandomVector(dist_x)

# Determination of reference probability
# MonteCarlo experiment

n_MC = int(1e6)

# Creation of event
ot.RandomGenerator.SetSeed(1)

vect = ot.RandomVector(dist_x)
G = ot.CompositeRandomVector(fb.model, vect)
event = ot.ThresholdEvent(G, ot.Less(), 0.0)

# create a Monte Carlo algorithm
experiment = ot.MonteCarloExperiment()
algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
algo.setMaximumOuterSampling(int(n_MC))
algo.run()

# retrieve results
result = algo.getResult()
probability = result.getProbabilityEstimate()
# Reference probability for p=6 from literature (Echard et al. 2011)
# Use rtol=5% to account for Monte Carlo variability
assert_almost_equal(probability, 4.416e-3, 0.05)

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

assert_almost_equal(NAIS_result.getProbabilityEstimate(), 4.416e-3, 0.05)

# check that the event sample is right
stepsNumber = Nais_algo.getStepsNumber()
inputEventSample = Nais_algo.getInputSample(stepsNumber - 1, Nais_algo.EVENT1)
outputEventSample = Nais_algo.getOutputSample(stepsNumber - 1, Nais_algo.EVENT1)
outputG = fb.model(inputEventSample)
diffSample = outputG - outputEventSample
assert_almost_equal(diffSample.computeMean(), [0.0])

# from intersection
paraboloid = ot.SymbolicFunction(
    ["u1", "u2", "u3", "u4", "u5"], ["- u5 + u1^2 + u2^2 + u3^2 + u4^2"]
)
b = 3.5
db = 0.02
dim = 5
U = ot.Normal(dim)
X = ot.RandomVector(U)
Y = ot.CompositeRandomVector(paraboloid, X)
E1 = ot.ThresholdEvent(Y, ot.Less(), -b)
E3 = ot.ThresholdEvent(Y, ot.Greater(), -(b + db))
E4 = ot.IntersectionEvent([E1, E3])
algo = ot.NAIS(E4, 0.35)

# Test setter of event
ot.RandomGenerator.SetSeed(0)
X = ot.RandomVector(ot.Normal())
Y = ot.CompositeRandomVector(ot.SymbolicFunction(["X"], ["X"]), X)
event = ot.ThresholdEvent(Y, ot.Less(), -2.0)
algo = ot.NAIS(event, 0.2)
# NOTE: keep this budget: the estimates below use a 1e-2 *relative*
# tolerance, which already needs ~10000 samples to pass
algo.setMaximumOuterSampling(10000)
algo.run()
result = algo.getResult()
assert_almost_equal(
    result.getProbabilityEstimate(), ot.Normal().computeCDF(-2), 1.0e-2, 0.0
)
ot.RandomGenerator.SetSeed(0)
Y2 = ot.CompositeRandomVector(ot.SymbolicFunction(["X"], ["2*X"]), X)
event2 = ot.ThresholdEvent(Y2, ot.Less(), -2.0)
algo.setEvent(event2)
algo.run()
result2 = algo.getResult()
assert_almost_equal(
    result2.getProbabilityEstimate(), ot.Normal().computeCDF(-1), 1.0e-2, 0.0
)
