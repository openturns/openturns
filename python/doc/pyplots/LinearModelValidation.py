import openturns as ot
from openturns.viewer import View

func = ot.SymbolicFunction(
    ['x1', 'x2', 'x3'],
    ['x1 + x2 + sin(x2 * 2 * pi_) / 5 + 1e-3 * x3^2']
)

dimension = 3

distribution = ot.ComposedDistribution([ot.Normal()] * dimension)
inputSample = distribution.getSample(50)
outputSample = func(inputSample)
algo = ot.LinearModelAlgorithm(inputSample, outputSample)
algo.run()
result = algo.getResult()
validation = ot.LinearModelValidation(result, ot.LinearModelValidation.LEAVEONEOUT)
graph = validation.drawValidation()
View(graph)
