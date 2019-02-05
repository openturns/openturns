import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

size = 100
inputDimension = 6
inputSample = ot.Normal(inputDimension).getSample(size)
inputVar = ['X' + str(i) for i in range(inputDimension)]
inputSample.setDescription(inputVar)
expression = ''
for i in range(inputDimension):
    if i > 0:
        expression += '+'
    expression += 'cos(' + str(i + 1) + '*' + inputVar[i] + ')'
model = ot.SymbolicFunction(inputVar, [expression])
outputSample = model(inputSample)

cobweb = ot.VisualTest.DrawCobWeb(
    inputSample, outputSample, 2.5, 3.0, 'red', False)

View(cobweb, figure_kwargs={'figsize': (10, 6)},
     legend_kwargs={'loc': 'lower right'})
