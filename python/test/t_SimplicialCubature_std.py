#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
import math as m

v1 = [0.0, 0.0, 0.0]
v2 = [1.0, 1.0, 1.0]
v3 = [0.0, 1.0, 1.0]
v4 = [0.0, 0.0, 1.0]
# simplex
S = [v1, v2, v3, v4]
simplicies = [[0, 1, 2, 3]]
mesh = ot.Mesh(S, simplicies)
# function to integrate
f = ot.SymbolicFunction(["x1", "x2", "x3"], ["exp(x1 + x2 + x3)"])
# integral of f
algo = otexp.SimplicialCubature()
value = algo.integrate(f, mesh)[0]
print(value)
ott.assert_almost_equal(value, (m.exp(1.0) - 1.0) ** 3 / 6)
