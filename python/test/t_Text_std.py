#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Instantiate one distribution object
dim = 2
meanPoint = ot.Point(dim, 1.0)
meanPoint[0] = 0.5
meanPoint[1] = -0.5
sigma = ot.Point(dim, 1.0)
sigma[0] = 2.0
sigma[1] = 3.0
R = ot.CorrelationMatrix(dim)
for i in range(1, dim):
    R[i, i - 1] = 0.5

distribution = ot.Normal(meanPoint, sigma, R)

# Test for sampling
size = 30
sample = distribution.getSample(size)

# Create an empty graph
myGraph = ot.Graph("Normal sample", "x1", "x2", True, "topright")

# Display extrema indices
x1 = [x[0] for x in sample[:, 0]]
x2 = [x[0] for x in sample[:, 1]]
idx = [0] * 4
idx[0] = x1.index(min(x1))
idx[1] = x1.index(max(x1))
idx[2] = x2.index(min(x2))
idx[3] = x2.index(max(x2))

sample.add(distribution.getSample(5))
labels = ot.Description(sample.getSize(), ".")
for i in range(4):
    labels[idx[i]] = str(idx[i])

position = ot.Description(sample.getSize(), "top")
position[idx[0]] = "right"
position[idx[1]] = "left"
position[idx[2]] = "top"
position[idx[3]] = "bottom"

myText = ot.Text(sample, labels)
myText.setColor("red")
myText.setTextPositions(position)
myText.setTextSize(1.0)

myGraph.add(myText)
