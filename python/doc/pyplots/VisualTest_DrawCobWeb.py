import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

size = 100
inputDimension = 6
inputSample = ot.Normal(inputDimension).getSample(size)
inputVar = ot.Description(inputDimension)
for i in range(inputDimension):
    inputVar[i] = 'X' + str(i)
inputSample.setDescription(inputVar)
formula = ot.Description(1)
expression = ''
for i in range(inputDimension):
    if i > 0:
        expression += '+'
    expression += 'cos(' + str(i + 1) + '*' + inputVar[i] + ')'
formula[0] = expression
outputVar = ot.Description(1)
outputVar[0] = 'y'
model = ot.SymbolicFunction(inputVar, outputVar, formula)
outputSample = model(inputSample)

cobweb = ot.VisualTest_DrawCobWeb(
    inputSample, outputSample, 2.5, 3.0, 'red', False)

View(cobweb, figure_kwargs={'figsize': (10, 6)},
     legend_kwargs={'loc': 'lower right'})
