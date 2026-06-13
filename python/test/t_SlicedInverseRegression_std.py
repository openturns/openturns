#!/usr/bin/env python

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
