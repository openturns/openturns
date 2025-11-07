import openturns as ot
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

size = 100
inputDimension = 6
inputSample = ot.Normal(inputDimension).getSample(size)
inputVar = ["X" + str(i) for i in range(inputDimension)]
inputSample.setDescription(inputVar)
expression = ""
for i in range(inputDimension):
    if i > 0:
        expression += "+"
    expression += "cos(" + str(i + 1) + "*" + inputVar[i] + ")"
model = ot.SymbolicFunction(inputVar, [expression])
outputSample = model(inputSample)

cobweb = ot.VisualTest.DrawParallelCoordinates(
    inputSample, outputSample, 2.5, 3.0, "red", False
)

cobweb.setLegendCorner([1.0, 0.0])
cobweb.setLegendPosition("lower right")
otv.View(cobweb, figure_kw={"figsize": (9, 5)})
