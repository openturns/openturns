#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Create a function
inputVar = ot.Description(2)
inputVar[0] = "x"
inputVar[1] = "y"
formula = ot.Description(1)
formula[0] = "exp(-sin(cos(y)^2*x^2+sin(x)^2*y^2))"
f = ot.SymbolicFunction(inputVar, formula)

# Generate the data for the curves to be drawn
discretization = ot.Point(2)
nX = 75
nY = 75
discretization[0] = nX
discretization[1] = nY
inputData = ot.Box(discretization).generate()
inputData *= [10.0] * 2
inputData += [-5.0] * 2
data = f(inputData)
size = 7
levels = ot.Point(size)
for i in range(size):
    levels[i] = (0.5 + i) / size
# Create an empty graph
myGraph = ot.Graph("Complex iso lines", "u1", "u2", True, "topright")

# Create the first cloud
myContour = ot.Contour(nX + 2, nY + 2, data)
myContour.setLevels(levels)
myContour.setColor("red")
print("contour=", myContour)

# Then, draw it
myGraph.add(myContour)
