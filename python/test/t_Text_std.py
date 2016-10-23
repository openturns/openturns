#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    dim = 2
    meanPoint = NumericalPoint(dim, 1.0)
    meanPoint[0] = 0.5
    meanPoint[1] = -0.5
    sigma = NumericalPoint(dim, 1.0)
    sigma[0] = 2.0
    sigma[1] = 3.0
    R = CorrelationMatrix(dim)
    for i in range(1, dim):
        R[i, i - 1] = 0.5

    distribution1 = Normal(meanPoint, sigma, R)

    # Instanciate another distribution object
    meanPoint[0] = -1.5
    meanPoint[1] = 0.5
    sigma[0] = 4.0
    sigma[1] = 1.0
    for i in range(1, dim):
        R[i, i - 1] = -0.25

    distribution2 = Normal(meanPoint, sigma, R)

    # Test for sampling
    size = 30
    sample = distribution1.getSample(size)

    # Create an empty graph
    myGraph = Graph("Normal sample", "x1", "x2", True, "topright")

    # Create the first cloud
    myCloud = Cloud(sample, "blue", "fsquare", "First Cloud")

    # Then, draw it
    myGraph.add(myCloud)

    # Display extrema indices
    x1 = [ x[0] for x in sample[:,0]]
    x2 = [ x[0] for x in sample[:,1]]
    idx = [0] * 4
    idx[0] = x1.index(min(x1))
    idx[1] = x1.index(max(x1))
    idx[2] = x2.index(min(x2))
    idx[3] = x2.index(max(x2))

    labels = Description(sample.getSize())
    for i in range(4):
      labels[idx[i]] = str(idx[i])

    position = Description(sample.getSize(), "top")
    position[idx[0]] = "right"
    position[idx[1]] = "left"
    position[idx[2]] = "top"
    position[idx[3]] = "bottom"

    myText = Text(sample, labels)
    myText.setColor("red")
    myText.setTextPositions(position)

    myGraph.add(myText)
    myGraph.draw("Graph_Text_a_OT", 640, 480)

except:
    import sys
    print("t_Text_std.py", sys.exc_info()[0], sys.exc_info()[1])
