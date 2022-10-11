import openturns as ot
from math import pi
from openturns.viewer import View

# Create the function
ot.RandomGenerator.SetSeed(0)
formula = ["sin(X1) + 7. * sin(X2)^2 + 0.1 * X3^4 * sin(X1)"]
input_names = ["X1", "X2", "X3"]
g = ot.SymbolicFunction(input_names, formula)
# Create the probabilistic model
distributionList = [ot.Uniform(-pi, pi)] * 3
distribution = ot.ComposedDistribution(distributionList)
# Create a training sample
N = 500
inputTrain = distribution.getSample(N)
outputTrain = g(inputTrain)
# Create the chaos
multivariateBasis = ot.OrthogonalProductPolynomialFactory(distributionList)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(
    inputTrain, outputTrain, selectionAlgorithm
)
totalDegree = 8
enumfunc = multivariateBasis.getEnumerateFunction()
P = enumfunc.getStrataCumulatedCardinal(totalDegree)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, P)
chaosalgo = ot.FunctionalChaosAlgorithm(
    inputTrain, outputTrain, distribution, adaptiveStrategy, projectionStrategy
)
chaosalgo.run()
result = chaosalgo.getResult()
# Draw Sobol' indices
chaosSI = ot.FunctionalChaosSobolIndices(result)
dim_input = distribution.getDimension()
first_order = [chaosSI.getSobolIndex(i) for i in range(dim_input)]
total_order = [chaosSI.getSobolTotalIndex(i) for i in range(dim_input)]
input_names = g.getInputDescription()
View(ot.SobolIndicesAlgorithm.DrawSobolIndices(input_names, first_order, total_order))
