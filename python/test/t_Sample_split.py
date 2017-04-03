#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # We create an empty Sample
    N = 100
    sample = Sample(N, 2)
    print("Sample size=", sample.getSize())

    # We populate the empty sample with numerous data
    for i in range(N):
        point = Point(2)
        point[0] = 1000 + i
        point[1] = 2000 + i
        sample[i] = point
    print("sample=", repr(sample))

    # We try to split the sample
    sample90 = sample.split(90)
    sample10 = sample.split(10)
    print("sample  =", repr(sample))
    print("sample10=", repr(sample10))
    print("sample90=", repr(sample90))

# except TestFailed, ex :
except:
    import sys
    print("t_Sample_split.py", sys.exc_info()[0], sys.exc_info()[1])
