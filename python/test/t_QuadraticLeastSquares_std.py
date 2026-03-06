#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

# 1-d
X = [[1.0], [2.0], [3.0], [4.0]]
Y = [[5.0], [3.0], [4.0], [7.0]]
algo = ot.QuadraticLeastSquares(X, Y)
algo.run()
mm = algo.getResult().getMetaModel()
x = [0.0]
y2 = mm(x)
print(f"mm(x)={y2}")
ott.assert_almost_equal(mm.gradient([0.0])[0, 0], -5.55)
ott.assert_almost_equal(mm.hessian([0.0])[0, 0, 0], 2.5)

# 2-d
f = ot.SymbolicFunction(
    ["x1", "x2"], ["9*x1^2 + 6*x2^2 + 0.1*x1 * x2 + 4*x1 + 7*x2 + 5.0"]
)
X = ot.Normal([2.0, 3.0], [1.0, 1.0]).getSample(1000)
Y = f(X)
algo = ot.QuadraticLeastSquares(X, Y)
algo.run()
mm = algo.getResult().getMetaModel()
x = [2.0, 3.0]
y = f(x)
y2 = mm(x)
print(f"f(x)={y}")
print(f"mm(x)={y2}")
ott.assert_almost_equal(y, y2, 1e-2, 1e-2)
g = f.gradient(x)
g2 = mm.gradient(x)
ott.assert_almost_equal(g[0, 0], g2[0, 0], 1e-2, 1e-2)
ott.assert_almost_equal(g[1, 0], g2[1, 0], 1e-2, 1e-2)
h = f.hessian(x)
h2 = mm.hessian(x)
ott.assert_almost_equal(h[0, 0, 0], h2[0, 0, 0], 1e-2, 1e-2)
ott.assert_almost_equal(h[0, 1, 0], h2[0, 1, 0], 1e-2, 1e-2)
ott.assert_almost_equal(h[1, 1, 0], h2[1, 1, 0], 1e-2, 1e-2)
