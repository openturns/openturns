#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# We create a numerical math function
func = ot.SymbolicFunction(("E", "F", "L", "I"), ("-F*L^3/(3.*E*I)",))

dim = func.getInputDimension()

# We create a normal distribution point of dimension 1
# E, F, L, I
mean = [50.0, 1.0, 10.0, 5.0]
sigma = [1.0] * dim
dist = ot.Normal(mean, sigma)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(dist)

# We create a composite random vector
output = ot.CompositeRandomVector(func, vect)

# We create an Event from this RandomVector
event = ot.ThresholdEvent(output, ot.Less(), -3.0)

# We create a Monte Carlo algorithm
# event.isComposite() --> automatically add a MonteCarloExperiment
algo = ot.ProbabilitySimulationAlgorithm(event)
algo.setMaximumOuterSampling(500)
algo.setBlockSize(10)
algo.setKeepSample(True)

# Perform the simulation
algo.run()

# input / output sampls
input_sample = algo.getInputSample()
output_sample = algo.getInputSample()

assert len(input_sample) == 680
ott.assert_almost_equal(algo.getResult().getProbabilityEstimate(), 0.13970588235294112, 1e-3, 1e-3)
