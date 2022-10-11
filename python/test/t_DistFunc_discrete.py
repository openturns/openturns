#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Discrete related functions
# rDiscrete
p = [0.1, 0.2, 0.3, 0.4]
print("rDiscrete(", ot.Point(p), ")=", ot.DistFunc.rDiscrete(p))
print("rDiscrete(", ot.Point(p), ", 100)=", ot.DistFunc.rDiscrete(p, 100))
