#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# We create an empty Sample
N = 100
sample = ot.Sample(N, 2)
print("Sample size=", sample.getSize())

# We populate the empty sample with numerous data
for i in range(N):
    point = ot.Point(2)
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
