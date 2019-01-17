#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ref = ot.SymbolicFunction("x", "sin(x)")
size = 12
locations = [0.0] * size
values = [0.0] * size
derivatives = [0.0] * size
# Build locations/values/derivatives with non-increasing locations
for i in range(size):
    locations[i] = 10.0 * i * i / (size - 1.0) / (size - 1.0)
    values[i] = ref([locations[i]])[0]
    derivatives[i] = ref.gradient([locations[i]])[0, 0]

evaluation = ot.PiecewiseHermiteEvaluation(
    locations, values, derivatives)
print("evaluation=", evaluation)
# Check the values
X = [[-1.0 + 12.0 * i / (2.0 * size - 1.0)] for i in range(2 * size)]
for x in X:
    print("f( %.12g )=" % x[0], evaluation(x), ", ref=", ref(x))
Y = evaluation(X)
print(Y)
