#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # size of timeGrid
    size = 6
    dimension = 1
    sample = Sample(size, dimension)
    for i in range(size):
        for j in range(dimension):
            sample[i, j] = i + j + 1

    # TimeGrid
    timeGrid = RegularGrid(0.0, 1.0 / (size - 1), size)

    # TimeSeries
    timeSeries = TimeSeries(timeGrid, sample)

    # We create an empty ProcessSample with default constructor
    psample0 = ProcessSample()
    psample0.setName("PSample0")
    print("Default constructor")
    print("psample0=", psample0)

    # We create an empty ProcessSample with timeGrid, size and dimension
    # arguments
    psample1 = ProcessSample(timeGrid, 4, dimension)
    psample1.setName("PSample1")
    print("Constructor based on size, dimension and timeGrid")
    print("psample1=", psample1)

    # change the first component using operator []
    psample1[0] = timeSeries
    print("changing psample1[0] with []")
    print("psample1[0]=", (psample1[0]))

    # We create a ProcessSample with size and timeSeries arguments
    psample2 = ProcessSample(3, timeSeries)
    psample2.setName("PSample2")
    print("Constructor based on size / timeSeries")
    print("psample2=", psample2)

    # TimeSeriesCollection
    # collection = TimeSeriesCollection()
    # collection.add(timeSeries)

    # sample *= [0.1]
    # newtimeSeries = TimeSeries(timeGrid, sample)
    # collection.add(newtimeSeries)

    # We create a ProcessSample with collection arguments arguments
    # psample3 = ProcessSample(collection)

    # psample3.setName("PSample3")
    # print "Constructor based on collection "
    # print "psample3=", psample3

    # ts = psample3[1]
    # print "last element of psample3=", (ts)
    # print "psample3 mean=", psample3.computeMean()
    # print "psample3 temporal mean=", psample3.computeTemporalMean()


except:
    import sys
    print("t_ProcessSample_std.py", sys.exc_info()[0], sys.exc_info()[1])
