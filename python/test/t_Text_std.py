#! /usr/bin/env python

from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instantiate one distribution object
    dim = 2
    meanPoint = Point(dim, 1.0)
    meanPoint[0] = 0.5
    meanPoint[1] = -0.5
    sigma = Point(dim, 1.0)
    sigma[0] = 2.0
    sigma[1] = 3.0
    R = CorrelationMatrix(dim)
    for i in range(1, dim):
        R[i, i - 1] = 0.5

    distribution = Normal(meanPoint, sigma, R)

    # Test for sampling
    size = 30
    sample = distribution.getSample(size)

    # Create an empty graph
    myGraph = Graph("Normal sample", "x1", "x2", True, "topright")

    # Display extrema indices
    x1 = [x[0] for x in sample[:, 0]]
    x2 = [x[0] for x in sample[:, 1]]
    idx = [0] * 4
    idx[0] = x1.index(min(x1))
    idx[1] = x1.index(max(x1))
    idx[2] = x2.index(min(x2))
    idx[3] = x2.index(max(x2))

    sample.add(distribution.getSample(5))
    labels = Description(sample.getSize(), ".")
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
    myText.setTextSize(1.0)

    myGraph.add(myText)

except:
    import sys
    print("t_Text_std.py", sys.exc_info()[0], sys.exc_info()[1])
