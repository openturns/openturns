#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import *

ot.TESTPREAMBLE()


# Problem parameters
inputDimension = 2
outputDimension = 1
rho = 0.3
a = 4.
b = 5.

# Reference analytical values
covTh = a * a + b * b + 2 * a * b * rho
Si = [[(a * a + a * b * rho) / covTh, a * a / covTh],
      [(b * b + a * b * rho) / covTh, b * b / covTh]]

# Model
inputName = ["X1", "X2", "a", "b"]
formula = ["a * X1 + b * X2"]

full = ot.SymbolicFunction(inputName, formula)
model = ot.ParametricFunction(full, [2, 3], [a, b])

# Input distribution
distribution = ot.ComposedDistribution([ot.Normal()] * inputDimension)

# Correlated input distribution
S = ot.CorrelationMatrix(inputDimension)
S[1, 0] = 0.3
R = ot.NormalCopula().GetCorrelationFromSpearmanCorrelation(S)
myCopula = ot.NormalCopula(R)
myCorrelatedInputDistribution = ot.ComposedDistribution(
    [ot.Normal()] * inputDimension, myCopula)

sample = myCorrelatedInputDistribution.getSample(2000)

# Orthogonal basis
enumerateFunction = ot.LinearEnumerateFunction(inputDimension)
productBasis = ot.OrthogonalProductPolynomialFactory(
    [ot.HermiteFactory()] * inputDimension, enumerateFunction)
# Adaptive strategy
adaptiveStrategy = ot.FixedStrategy(
    productBasis, enumerateFunction.getStrataCumulatedCardinal(4))
# Projection strategy
samplingSize = 250
projectionStrategy = ot.LeastSquaresStrategy(
    ot.MonteCarloExperiment(samplingSize))

# Polynomial chaos algorithm
algo = ot.FunctionalChaosAlgorithm(
    model, distribution, adaptiveStrategy, projectionStrategy)
algo.run()

# Post-process the results
result = ot.FunctionalChaosResult(algo.getResult())
ancova = ot.ANCOVA(result, sample)
indices = ancova.getIndices()
uncorrelatedIndices = ancova.getUncorrelatedIndices()

for i in range(inputDimension):
    value = indices[i]
    print("ANCOVA index", i, "= %.6g" %
          value, "absolute error=%.6g" % fabs(value - Si[i][0]))
    value = uncorrelatedIndices[i]
    print("ANCOVA uncorrelated index", i, "= %.8f" %
          value, "absolute error=%.6g" % fabs(value - Si[i][1]))

# Compare full/sparse
ot.RandomGenerator.SetSeed(323)

model = ot.SymbolicFunction(['x1', 'x2', 'x3'], ['x1 + 0.56*x2 + 0.9*x3'])
distribution = ot.Normal(3)
distribution.setDescription(['x1', 'x2', 'x3'])

n_sample = 10
input_sample = distribution.getSample(n_sample)
output_sample = model(input_sample)

dim = 3
enumerateFunction = ot.LinearEnumerateFunction(dim)
polyCol = [0.]*dim
for i in range(dim):
    polyCol[i] = ot.StandardDistributionPolynomialFactory(
        distribution.getMarginal(i))

####### Chaos definition ######
multivariateBasis = ot.OrthogonalProductPolynomialFactory(
    polyCol, enumerateFunction)
indexMax = enumerateFunction.getStrataCumulatedCardinal(1)
strategy = ot.FixedStrategy(multivariateBasis, indexMax)

approximation_algorithm = ot.LeastSquaresMetaModelSelectionFactory(ot.LARS(),
                                                                   ot.CorrectedLeaveOneOut())
evaluationStrategy_sparse = ot.LeastSquaresStrategy(approximation_algorithm)
evaluationStrategy = ot.LeastSquaresStrategy()

# sparse and not sparse
chaos = ot.FunctionalChaosAlgorithm(input_sample, output_sample, distribution,
                                    strategy, evaluationStrategy)
chaos.run()
chaos_sparse = ot.FunctionalChaosAlgorithm(input_sample, output_sample, distribution,
                                           strategy, evaluationStrategy_sparse)
chaos_sparse.run()
print('indices/full=', chaos.getResult().getIndices())
print('indices/sparse=', chaos_sparse.getResult().getIndices())

ancova = ot.ANCOVA(chaos.getResult(), input_sample)
ancova_sparse = ot.ANCOVA(chaos_sparse.getResult(), input_sample)
print('Indice ancova, chaos normal : {:0.3f} {:0.3f} {:0.3f}'.format(
    *ancova.getIndices()))
print('Indice ancova, chaos sparse : {:0.3f} {:0.3f} {:0.3f}'.format(
    *ancova_sparse.getIndices()))
