import openturns as ot
from openturns.viewer import View
from openturns.usecases import ishigami_function
import openturns.experimental as otexp

ot.RandomGenerator.SetSeed(0)
im = ishigami_function.IshigamiModel()
sampleSize = 500
inputTrain = im.inputDistribution.getSample(sampleSize)
outputTrain = im.model(inputTrain)
multivariateBasis = ot.OrthogonalProductPolynomialFactory([im.X1, im.X2, im.X3])
selectionAlgorithm = ot.PenalizedLeastSquaresAlgorithmFactory()
projectionStrategy = ot.LeastSquaresStrategy(
    inputTrain, outputTrain, selectionAlgorithm
)
totalDegree = 8
enumerateFunction = multivariateBasis.getEnumerateFunction()
basisSize = enumerateFunction.getStrataCumulatedCardinal(totalDegree)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, basisSize)
chaosalgo = ot.FunctionalChaosAlgorithm(
    inputTrain, outputTrain, im.inputDistribution, adaptiveStrategy, projectionStrategy
)
chaosalgo.run()
result = chaosalgo.getResult()
validation = otexp.FunctionalChaosValidation(result)
graph = validation.drawValidation()
View(graph)
