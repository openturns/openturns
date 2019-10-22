#! /usr/bin/env python

from __future__ import print_function
import sys
import openturns as ot

ot.TESTPREAMBLE()


def progress(percent):
    sys.stderr.write('-- progress=' + str(percent) + '%\n')


def stop():
    sys.stderr.write('-- stop?\n')
    return False


# We create a numerical math function
myFunction = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['-F*L^3/(3*E*I)'])

dim = myFunction.getInputDimension()

# We create a normal distribution point of dimension 1
mean = [0.0] * dim
# E
mean[0] = 50.0
# F
mean[1] = 1.0
# L
mean[2] = 10.0
# I
mean[3] = 5.0
sigma = [1.0] * dim
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
output = ot.CompositeRandomVector(myFunction, vect)

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Less(), -3.0)

# Monte Carlo
experiments = [ot.MonteCarloExperiment()]
# Quasi Monte Carlo
experiments.append(ot.LowDiscrepancyExperiment())
# Randomized Quasi Monte Carlo
experiment = ot.LowDiscrepancyExperiment()
experiment.setRandomize(True)
experiments.append(experiment)
# Importance sampling
mean[0] = 4.99689645939288809018e+01
mean[1] = 1.84194175946153282375e+00
mean[2] = 1.04454036676956398821e+01
mean[3] = 4.66776215562709406726e+00
myImportance = ot.Normal(mean, sigma, R)
experiments.append(ot.ImportanceSamplingExperiment(myImportance))
# Randomized LHS
experiment = ot.LHSExperiment()
experiment.setAlwaysShuffle(True)
experiments.append(experiment)

for experiment in experiments:

    ot.RandomGenerator.SetSeed(0)

    myAlgo = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
    myAlgo.setMaximumOuterSampling(250)
    myAlgo.setBlockSize(4)
    myAlgo.setMaximumCoefficientOfVariation(0.1)

    print('algo=', myAlgo)

    # Perform the simulation
    myAlgo.run()

    # Stream out the result
    print('algo result=', myAlgo.getResult())
    print('probability distribution=',
          myAlgo.getResult().getProbabilityDistribution())

    # Use the standard deviation as a stoping rule
    experiment = ot.MonteCarloExperiment()
    myAlgo = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
    myAlgo.setMaximumOuterSampling(250)
    myAlgo.setBlockSize(4)
    myAlgo.setMaximumCoefficientOfVariation(0.0)
    myAlgo.setMaximumStandardDeviation(0.1)
    myAlgo.setProgressCallback(progress)
    myAlgo.setStopCallback(stop)

    print('algo=', myAlgo)

    # Perform the simulation
    myAlgo.run()

    # Stream out the result
    print('algo result=', myAlgo.getResult())
    print('probability distribution=',
          myAlgo.getResult().getProbabilityDistribution())

print('-' * 32)
ot.RandomGenerator.SetSeed(0)
description = ot.Description()
description.add('composite vector/comparison event')
dim = 2
distribution = ot.Normal(dim)
Xvector = ot.RandomVector(distribution)
f = ot.SymbolicFunction(['x0', 'x1'], ['x0+x1'])
Yvector = ot.CompositeRandomVector(f, Xvector)
s = 1.0
event1 = ot.ThresholdEvent(Yvector, ot.Greater(), s)
description.add('composite vector/domain event')
domain1D = ot.LevelSet(ot.SymbolicFunction(
    ['x0'], ['sin(x0)']), ot.LessOrEqual(), -0.5)
event2 = ot.DomainEvent(Yvector, domain1D)
description.add('composite vector/interval event')
interval = ot.Interval(0.5, 1.5)
event3 = ot.ThresholdEvent(Yvector, interval)
description.add('process/domain event')
Xprocess = ot.WhiteNoise(distribution, ot.RegularGrid(0.0, 0.1, 10))
domain2D = ot.LevelSet(
    ot.SymbolicFunction(['x0', 'x1'], ['(x0-1)^2+x1^2']), ot.LessOrEqual(), 1.0)
event4 = ot.ProcessEvent(Xprocess, domain2D)
all_events = [event1, event2, event3, event4]
for i, event in enumerate(all_events):
    print(description[i])
    if event.isComposite():
        experiment = ot.MonteCarloExperiment()
        myAlgo = ot.ProbabilitySimulationAlgorithm(event, experiment)
    else:
        myAlgo = ot.ProbabilitySimulationAlgorithm(event)
    myAlgo.setMaximumOuterSampling(250)
    myAlgo.setBlockSize(4)
    myAlgo.setMaximumCoefficientOfVariation(0.1)
    myAlgo.run()
    print('MonteCarlo result=', myAlgo.getResult())
    print('probability distribution=',
          myAlgo.getResult().getProbabilityDistribution())
