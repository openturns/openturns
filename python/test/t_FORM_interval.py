#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Build the event 'vector takes its values in interval'
# using a comparison operator and a threshold in order
# to be compatible with more algorithms than Monte Carlo


def buildEvent(vector, interval):
    dimension = vector.getDimension()
    if dimension != interval.getDimension():
        raise Exception('Dimensions not compatible')
    finiteLowerBound = interval.getFiniteLowerBound()
    finiteUpperBound = interval.getFiniteUpperBound()
    lowerBound = interval.getLowerBound()
    upperBound = interval.getUpperBound()
    # Easy case: 1D interval
    if (dimension == 1):
        if finiteLowerBound[0] and not finiteUpperBound[0]:
            print('case 1')
            return ot.Event(vector, Greater(), lowerBound[0])
        if not finiteLowerBound[0] and finiteUpperBound[0]:
            print('case 2')
            return ot.Event(vector, Less(), upperBound[0])
        if finiteLowerBound[0] and finiteUpperBound[0]:
            print('case 3')
            testFunction = ot.NumericalMathFunction(
                'x', 'min(x-(' + str(lowerBound[0]) + '), (' + str(upperBound[0]) + ') - x)')
            newVector = ot.RandomVector(ot.NumericalMathFunction(
                testFunction, vector.getFunction()), vector.getAntecedent())
            return ot.Event(newVector, Greater(), 0.0)
        # Here we build an event that is always true and much cheaper to
        # compute
        print('case 4')
        inputDimension = vector.getFunction().getInputDimension()
        return ot.Event(ot.RandomVector(ot.NumericalMathFunction(ot.Description.BuildDefault(inputDimension, 'x'), ['0.0']), vector.getAntecedent()), Less(), 1.0)
    # General case
    numConstraints = 0
    inVars = ot.Description.BuildDefault(dimension, 'y')
    slacks = ot.Description(0)
    for i in range(dimension):
        if finiteLowerBound[i]:
            slacks.add(inVars[i] + '-(' + str(lowerBound[i]) + ')')
        if finiteUpperBound[i]:
            slacks.add('(' + str(upperBound[i]) + ')-' + inVars[i])
    # No constraint
    if slacks.getSize() == 0:
        # Here we build an event that is always true and much cheaper to
        # compute
        inputDimension = vector.getFunction().getInputDimension()
        return ot.Event(ot.RandomVector(ot.NumericalMathFunction(ot.Description.BuildDefault(inputDimension, 'x'), ['0.0']), vector.getAntecedent()), Less(), 1.0)
    # Only one constraint
    if slacks.getSize() == 1:
        print('case 6')
        testFunction = ot.NumericalMathFunction(inVars, [slacks[0]])
    # Several constraints
    else:
        print('case 7')
        formula = 'min(' + slacks[0]
        for i in range(1, slacks.getSize()):
            formula += ',' + slacks[i]
        formula += ')'
        testFunction = ot.NumericalMathFunction(inVars, [formula])
    newVector = ot.RandomVector(ot.NumericalMathFunction(
        testFunction, vector.getFunction()), vector.getAntecedent())
    return ot.Event(newVector, Greater(), 0.0)

# Tests
ot.ResourceMap.SetAsUnsignedInteger('Simulation-DefaultBlockSize', 100)
ot.ResourceMap.SetAsUnsignedInteger(
    'Simulation-DefaultMaximumOuterSampling', 100)
ot.ResourceMap.SetAsNumericalScalar(
    'Simulation-DefaultMaximumCoefficientOfVariation', 0.0)
ot.ResourceMap.SetAsNumericalScalar(
    'Simulation-DefaultMaximumStandardDeviation', 0.0)
ot.ResourceMap.SetAsNumericalScalar(
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
    f = ot.NumericalMathFunction(inVars, inVars[0:outDim])
    Y = ot.RandomVector(f, X)
    #event = buildEvent(Y, domain)
    event = ot.Event(Y, domain)

    ot.RandomGenerator.SetSeed(0)
    #algo = getattr(openturns, algoName)(event)
    algo = ot.MonteCarlo(event)
    algo.run()
    res = algo.getResult().getProbabilityEstimate()
    print('MC p=%.6g' % res)

    ot.RandomGenerator.SetSeed(0)
    #algo = getattr(openturns, algoName)(event)
    algo = ot.FORM(ot.Cobyla(), event, X.getMean())
    algo.run()
    res = algo.getResult().getEventProbability()
    print('FORM p=%.2f' % res)
