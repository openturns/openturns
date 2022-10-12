#! /usr/bin/env python

import openturns as ot

ref = ot.SymbolicFunction("x", "sin(x)")
size = 12
locations = ot.Point(size)
values = ot.Point(size)
# Build locations/values with non-increasing locations
for i in range(size):
    locations[i] = 10.0 * i * i / (size - 1.0) / (size - 1.0)
    values[i] = ref([locations[i]])[0]

evaluation = ot.PiecewiseLinearEvaluation(locations, values)
print("evaluation=", evaluation)
# Check the values
for i in range(2 * size):
    x = [-1.0 + 12.0 * i / (2.0 * size - 1.0)]
    print("f( %.12g )=" % x[0], evaluation(x), ", ref=", ref(x))
