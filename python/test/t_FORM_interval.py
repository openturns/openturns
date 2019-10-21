#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Tests
ot.ResourceMap.SetAsUnsignedInteger(
    'SimulationAlgorithm-DefaultBlockSize', 100)
ot.ResourceMap.SetAsUnsignedInteger(
    'SimulationAlgorithm-DefaultMaximumOuterSampling', 100)
ot.ResourceMap.SetAsScalar(
    'SimulationAlgorithm-DefaultMaximumCoefficientOfVariation', 0.0)
ot.ResourceMap.SetAsScalar(
    'SimulationAlgorithm-DefaultMaximumStandardDeviation', 0.0)
ot.ResourceMap.SetAsScalar(
    'RootStrategyImplementation-DefaultStepSize', 0.1)

algorithms = ['MonteCarlo',
              'LHS',
              'QuasiMonteCarlo',
              'DirectionalSampling']

inDim = 4
X = ot.RandomVector(ot.Normal(inDim))
inVars = ot.Description.BuildDefault(inDim, 'x')

low = 1.0
up = 2.0
intervals = [ot.Interval([low], [up], [True],  [False]),
             ot.Interval([low], [up], [False], [True]),
             ot.Interval([low], [up], [True],  [True]),
             ot.Interval([low], [up], [False], [False]),
             ot.Interval([low] * 2, [up] * 2, [True, True],   [True, True]),
             ot.Interval([low] * 2, [up] * 2, [True, True],   [True, False]),
             ot.Interval([low] * 2, [up] * 2, [True, True],   [False, True]),
             ot.Interval([low] * 2, [up] * 2, [True, True],   [False, False]),
             ot.Interval([low] * 2, [up] * 2, [True, False],  [True, True]),
             ot.Interval([low] * 2, [up] * 2, [True, False],  [True, False]),
             ot.Interval([low] * 2, [up] * 2, [True, False],  [False, True]),
             ot.Interval([low] * 2, [up] * 2, [True, False],  [False, False]),
             ot.Interval([low] * 2, [up] * 2, [False, True],  [True, True]),
             ot.Interval([low] * 2, [up] * 2, [False, True],  [True, False]),
             ot.Interval([low] * 2, [up] * 2, [False, True],  [False, True]),
             ot.Interval([low] * 2, [up] * 2, [False, True],  [False, False]),
             ot.Interval([low] * 2, [up] * 2, [False, False], [True, True]),
             ot.Interval([low] * 2, [up] * 2, [False, False], [True, False]),
             ot.Interval([low] * 2, [up] * 2, [False, False], [False, True]),
             ot.Interval([low] * 2, [up] * 2, [False, False], [False, False])]

for domain in intervals:
    print('#' * 50)
    print('domain=\n', domain)
    outDim = domain.getDimension()
    f = ot.SymbolicFunction(inVars, inVars[0:outDim])
    Y = ot.CompositeRandomVector(f, X)
    event = ot.ThresholdEvent(Y, domain)

    ot.RandomGenerator.SetSeed(0)
    # algo = getattr(openturns, algoName)(event)
    algo = ot.ProbabilitySimulationAlgorithm(event, ot.MonteCarloExperiment())
    algo.run()
    res = algo.getResult().getProbabilityEstimate()
    print('MC p=%.6g' % res)

    ot.RandomGenerator.SetSeed(0)
    # algo = getattr(openturns, algoName)(event)
    algo = ot.FORM(ot.Cobyla(), event, X.getMean())
    algo.run()
    res = algo.getResult().getEventProbability()
    print('FORM p=%.2f' % res)
