#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

# 1-d
X = [[1.0], [2.0], [3.0], [4.0]]
Y = [[5.0], [3.0], [4.0], [7.0]]
algo = ot.QuadraticLeastSquares(X, Y)
algo.run()
mm = algo.getMetaModel()
x = [0.0]
y2 = mm(x)
print(f"mm(x)={y2}")
print(f"constant={algo.getConstant()}")
print(f"linear={algo.getLinear()}")
print(f"quadratic={algo.getQuadratic()}")
ott.assert_almost_equal(mm.gradient([0.0])[0, 0], -5.55)
ott.assert_almost_equal(mm.hessian([0.0])[0, 0, 0], 2.5)

# 2-d
f = ot.SymbolicFunction(["x1", "x2"], ["9*x1^2 + 6*x2^2 + 0.1*x1 * x2 + 4*x1 + 7*x2 + 5.0"])
X = ot.Normal([2.0, 3.0], [1.0, 1.0]).getSample(1000)
Y = f(X)
algo = ot.QuadraticLeastSquares(X, Y)
algo.run()
mm = algo.getMetaModel()
x = [2.0, 3.0]
y = f(x)
y2 = mm(x)
print(f"f(x)={y}")
print(f"mm(x)={y2}")
print(f"constant={algo.getConstant()}")
print(f"linear={algo.getLinear()}")
print(f"quadratic={algo.getQuadratic()}")
ott.assert_almost_equal(y, y2, 1e-2, 1e-2)
linear = algo.getLinear()
ott.assert_almost_equal(linear[0, 0], 4.0, 1e-2, 1e-2)
ott.assert_almost_equal(linear[1, 0], 7.0, 1e-2, 1e-2)
quadratic = algo.getQuadratic()
ott.assert_almost_equal(0.5 * quadratic[0, 0, 0], 9.0, 1e-2, 1e-2)
ott.assert_almost_equal(0.5 * quadratic[0, 1, 0], 0.1, 1e-2, 1e-2)
ott.assert_almost_equal(0.5 * quadratic[1, 1, 0], 6.0, 1e-2, 1e-2)
