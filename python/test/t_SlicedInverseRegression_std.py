#!/usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

# Make 2D dataset
ot.RandomGenerator.SetSeed(0)
N = 100
X = ot.Normal([0.0] * 2, [0.1] * 2).getSample(N)
X += [[-i / (N - 1), 2 * i / (N - 1)] for i in range(N)]
X = X - X.computeMean()
f = ot.SymbolicFunction(["x1", "x2"], ["4*(x1+2*x2)+2"])
Y = f(X) + ot.Normal(0.0, 0.2).getSample(N)

# full rank
algo = otexp.SlicedInverseRegression(X, Y)
algo.run()
result = algo.getResult()
print(f"ev={result.getEigenvalues()}")
x0 = X[0]
u0 = result.getTransformation()(x0)
print(x0, u0)
ott.assert_almost_equal([abs(u0[0]), abs(u0[1])], [0.885348, 0.0235555])
z0 = result.getInverseTransformation()(u0)
print(u0, z0)
ott.assert_almost_equal(z0, x0)

# rank=1
algo.setNbModes(1)
algo.run()
result = algo.getResult()
dir0 = result.getDirections()
u0 = result.getTransformation()(x0)
print(x0, u0)
ott.assert_almost_equal(u0[0], sum(dir0[i, 0] * x0[i] for i in range(2)))
z0 = result.getInverseTransformation()(u0)
print(u0, z0)
# z0 is the projection: dir0 * u0
ott.assert_almost_equal(z0, [dir0[0, 0] * u0[0], dir0[1, 0] * u0[0]])

# Poisson single-index model (mirrors statsmodels test_dimred.test_poisson):
# a 5-d collinear design drives a Poisson response through a single linear
# index b. SIR must concentrate the spectrum in one direction and recover b.
ot.RandomGenerator.SetSeed(0)
N = 500
Z = ot.Normal(5).getSample(N)
c = math.sqrt(1.0 - 0.5 ** 2)
X = ot.Sample([[z[0], 0.5 * z[0] + c * z[1], z[2], 0.5 * z[2] + c * z[3], z[4]]
               for z in Z])
b = [0.0, 1.0, -1.0, 0.0, 0.5]
linearIndex = ot.SymbolicFunction(["x0", "x1", "x2", "x3", "x4"],
                                  ["x1 - x2 + 0.5*x4"])
rates = ot.SymbolicFunction(["t"], ["exp(t)"])(linearIndex(X))
Y = ot.Sample(N, 1)
for i in range(N):
    Y[i, 0] = ot.Poisson(rates[i, 0]).getRealization()[0]

# 25 slices (500 obs / 20 per slice)
algo = otexp.SlicedInverseRegression(X, Y, 25)
algo.run()
result = algo.getResult()
eigs = result.getEigenvalues()
print(f"ev={eigs}")
ott.assert_almost_equal(eigs, [0.6947743, 0.0626249, 0.0428527, 0.0315240, 0.0233695])

# The response depends on X only through x.b: the spectrum concentrates
# in one direction and the first EDR direction aligns with b.
assert abs(eigs[0] / eigs[1]) > 5.0, "spectrum not concentrated in one direction"
dir0 = result.getDirections()
d0 = [dir0[i, 0] for i in range(5)]
norm_d0 = math.sqrt(sum(v * v for v in d0))
norm_b = math.sqrt(sum(v * v for v in b))
alignment = abs(sum(d0[i] * b[i] for i in range(5))) / (norm_d0 * norm_b)
assert alignment > 0.95, "first EDR direction does not align with the true index"
