import openturns as ot
from openturns.viewer import View

func = ot.SymbolicFunction(
    ['x1', 'x2', 'x3'],
    ['x1 + x2 + sin(x2 * 2 * pi_) / 5 + 1e-3 * x3^2']
)

dimension = 3

distribution = ot.JointDistribution([ot.Normal()] * dimension)
sampleSize = 50
inputSample = distribution.getSample(sampleSize)
outputSample = func(inputSample)
algo = ot.LinearModelAlgorithm(inputSample, outputSample)
algo.run()
result = algo.getResult()
splitter = ot.LeaveOneOutSplitter(sampleSize)
validation = ot.LinearModelValidation(result, splitter)
graph = validation.drawValidation()
View(graph)
