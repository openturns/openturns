#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instantiate one distribution object
    dim = 2
    meanPoint = Point(dim, 1.0)
    meanPoint[0] = 12.0
    meanPoint[1] = 5.0
    sigma = Point(dim, 1.0)
    sigma[0] = 3.0
    sigma[1] = 1.0
    R = CorrelationMatrix(dim)
    for i in range(1, dim):
        R[i, i - 1] = 0.75

    distribution = Normal(meanPoint, sigma, R)

    # Test for sampling
    size = 200
    sample = distribution.getSample(size)
    tmp = Sample(2, Point(2))
    tmp[0, 0] = sample.getMin()[0]
    tmp[0, 1] = meanPoint[1] - sigma[1] / sigma[0] * (meanPoint[0] - tmp[0, 0])
    tmp[1, 0] = sample.getMax()[0]
    tmp[1, 1] = meanPoint[1] - sigma[1] / sigma[0] * (meanPoint[0] - tmp[1, 0])

    # Create the cloud
    myCloud = Cloud(sample, "blue", "fsquare", "Normal Cloud")

    # Create the line
    myLine = Curve(tmp, "red", "solid", 2, "Line")

    data = Point(0)

    palette = Description()
    labels = Description()

    data.add(5665855)
    labels.add("Candidate1")
    palette.add("blue")

    data.add(4804713)
    labels.add("Candidate2")
    palette.add("white")

    data.add(4610113)
    labels.add("Candidate3")
    palette.add("pink")

    data.add(1949170)
    labels.add("Candidate4")
    palette.add("yellow")

    data.add(1630045)
    labels.add("Candidate5")
    palette.add("red")

    data.add(1518528)
    labels.add("Candidate6")
    palette.add("ivory")

    data.add(1495724)
    labels.add("Candidate7")
    palette.add("green")

    data.add(1210562)
    labels.add("Candidate8")
    palette.add("orange")

    data.add(1204689)
    labels.add("Candidate9")
    palette.add("maroon")

    data.add(1113484)
    labels.add("Candidate10")
    palette.add("violet")

    data.add(3295588)
    labels.add("cumul candidates < 3.5% ")
    palette.add("purple")

    centre = Point(2)
    centre[0] = 7.0
    centre[1] = 10.0

    # Create the piechart
    myPie = Pie(data, labels, centre, 4, palette)

    # Create an empty graph
    myGraph = Graph("Cloud/line/pie", "x1", "x2", True, "topright", 1.0)

    # Then, draw it
    myGraph.add(myCloud)
    myGraph.add(myLine)
    myGraph.add(myPie)
    myGraph.draw("Graph_mix_OT.png")

    # Set new colors
    myGraph.setColors(["red", "green", "#120345"])

    # Test empty new colors
    myGraph.setColors([])

    # Set new legends
    myGraph.setLegends(["item1", "item2", "item3"])

    # Test empty new legends
    myGraph.setLegends([])

except:
    import sys
    print("t_Graph_mix.py", sys.exc_info()[0], sys.exc_info()[1])
