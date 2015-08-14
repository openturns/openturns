#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Hanning filter
    myHanningFilter = Hanning()
    print("myHanningFilter = ", myHanningFilter)

    # Hamming filter
    myHammingFilter = Hamming()
    print("myHammingFilter = ",  myHammingFilter)

    # Evaluation of values between t=0 and t=1 using a step = 0.01
    steps = 100
    tMin = 0.0
    tMax = 1.0
    tStep = (tMax - tMin) / steps

    for i in range(steps + 1):
        t = tMin + i * tStep
        print('t = %g Hanning = %g Hamming = %g' %
              (t, myHanningFilter(t), myHammingFilter(t)))


except:
    import sys
    print("t_FilteringWindows_std.py", sys.exc_info()[0], sys.exc_info()[1])
