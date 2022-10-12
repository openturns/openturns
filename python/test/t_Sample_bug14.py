#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


sample1 = ot.Sample(5, ot.Point(1, 2.0))
sample2 = ot.Sample(sample1)
sample2[0] = ot.Point(1, 3.0)
print("sample1=", repr(sample1))
print("sample2=", repr(sample2))
sample1[1] = ot.Point(1, -5.0)
print("sample1=", repr(sample1))
print("sample2=", repr(sample2))
sample1[3, 0] = 6.0
print("sample1=", repr(sample1))
print("sample1[2][0]=", sample1[2][0])
print("sample1[2, 0]=", sample1[2, 0])
try:
    sample1[1, 2] = -4.0
except Exception:
    print("set item failed on purpose")
