import openturns as ot
from openturns.viewer import View
from openturns.usecases import ishigami_function

im = ishigami_function.IshigamiModel()
sampleSize = 500
inputTrain = im.distributionX.getSample(sampleSize)
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
    inputTrain, outputTrain, im.distributionX, adaptiveStrategy, projectionStrategy
)
chaosalgo.run()
result = chaosalgo.getResult()
validation = ot.FunctionalChaosValidation(result)
graph = validation.drawValidation()
View(graph)
