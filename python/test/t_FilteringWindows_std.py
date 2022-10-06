#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Hann filter
myHanningFilter = ot.Hann()
print("myHanningFilter = ", myHanningFilter)

# Hamming filter
myHammingFilter = ot.Hamming()
print("myHammingFilter = ",  myHammingFilter)

# Evaluation of values between t=0 and t=1 using a step = 0.01
steps = 100
tMin = 0.0
tMax = 1.0
tStep = (tMax - tMin) / steps

for i in range(steps + 1):
    t = tMin + i * tStep
    print('t = %g Hann = %g Hamming = %g' %
          (t, myHanningFilter(t), myHammingFilter(t)))
