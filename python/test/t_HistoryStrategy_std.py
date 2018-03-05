#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    size = 10
    dimension = 2
    sample = Sample(size, dimension)
    # Fill-in the sample
    for i in range(size):
        p = Point(dimension)
        for j in range(dimension):
            p[j] = i + j * 1.0 / dimension
        sample[i] = p
    print("sample=", repr(sample))
    # History using the Null strategy
    nullStrategy = Null()
    for i in range(size):
        nullStrategy.store(sample[i])
    print("Null strategy sample=", repr(nullStrategy.getSample()))
    # History using the Full strategy
    fullStrategy = Full()
    fullStrategy.setDimension(dimension)
    for i in range(size):
        fullStrategy.store(sample[i])
    print("Full strategy sample=", repr(fullStrategy.getSample()))
    # History using the Last strategy, large storage
    lastStrategy = Last(3 * size)
    lastStrategy.setDimension(dimension)
    for i in range(size):
        lastStrategy.store(sample[i])
    print("Last strategy sample (large storage)=",
          repr(lastStrategy.getSample()))
    lastStrategy = Last(size // 3)
    lastStrategy.setDimension(dimension)
    # History using the Last strategy, small storage
    for i in range(size):
        lastStrategy.store(sample[i])
    print("Last strategy sample (small storage)=",
          repr(lastStrategy.getSample()))
    # History using the Compact strategy, large storage
    compactStrategy = Compact(3 * size)
    compactStrategy.setDimension(dimension)
    for i in range(size):
        compactStrategy.store(sample[i])
    print("Compact strategy sample (large storage)=",
          repr(compactStrategy.getSample()))
    compactStrategy = Compact(size // 3)
    compactStrategy.setDimension(dimension)
    # History using the Compact strategy, small storage
    for i in range(size):
        compactStrategy.store(sample[i])
    print("Compact strategy sample (small storage)=",
          repr(compactStrategy.getSample()))

except:
    import sys
    print("t_HistoryStrategy_std.py", sys.exc_info()[0], sys.exc_info()[1])
