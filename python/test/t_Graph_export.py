#! /usr/bin/env python

from openturns import *
from math import *
import os

TESTPREAMBLE()

try:
    maxPoint = 101
    xMin = -2.0 * pi
    xMax = 2.0 * pi
    data = Sample(maxPoint, 2)
    for i in range(maxPoint):
        x = xMin + (xMax - xMin) * i / (maxPoint - 1.0)
        data[i, 0] = x
        data[i, 1] = sin(x)
    sinGraph = Graph('Sin function', 'x', 'y', True, 'topright')
    sinGraph.add(Curve(data, 'red', 'solid', 2, 'sin'))
    # Test export to all formats
    sinGraph.draw('sinGraph_ALL', 640, 480, GraphImplementation.ALL)
    [os.remove('sinGraph_ALL' + ext)
     for ext in ['.eps', '.png', '.fig', '.pdf']]
    # Test export to PNG format
    sinGraph.draw('sinGraph.png', 640, 480, GraphImplementation.PNG)
    os.remove('sinGraph.png')
    # Test export to EPS format
    sinGraph.draw('sinGraph_EPS_PDF', 640, 480,
                  GraphImplementation.EPS | GraphImplementation.PDF)
    os.remove('sinGraph_EPS_PDF.eps')
    os.remove('sinGraph_EPS_PDF.pdf')
    # Test export to FIG format
    sinGraph.draw('sinGraph', 640, 480, GraphImplementation.FIG)
    os.remove('sinGraph.fig')
except:
    import sys
    print('t_Graph_export.py', sys.exc_info()[0], sys.exc_info()[1])
