#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Left hand side of the composition
left = ot.SymbolicFunction(
    ["x1", "x2"], ["x1*sin(x2)", "cos(x1+x2)", "(x2+1)*exp(x1-2*x2)"]
)

# Right hand side of the composition
right = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"],
    ["(x1*x1+x2^3*x1)/(2*x3*x3+x4^4+1)", "cos(x2*x2+x4)/(x1*x1+1+x3^4)"],
)

# Compositon of left and right
composed = ot.ComposedFunction(left, right)

print("right=", repr(right))
print("left=", repr(left))
print("composed=", repr(composed))

# Does it work?
x = ot.Point(right.getInputDimension(), 1.0)
y = right(x)
z = left(y)
Dy = right.gradient(x)
Dz = left.gradient(y)

print("x=", repr(x), " y=right(x)=", repr(y), " z=left(y)=", repr(z))
print("left(right(x))=", repr(composed(x)))
print("D(right)(x)=", repr(Dy), " D(left)(y)=", repr(Dz))
print(" prod=", repr(Dy * Dz))
print("D(left(right(x)))=", repr(composed.gradient(x)))
result = composed.hessian(x)
print("DD(left(right(x)))=")
for k in range(result.getNbSheets()):
    for j in range(result.getNbColumns()):
        for i in range(result.getNbRows()):
            print("%.6f" % result[i, j, k])
        print("")
    print("")

# left and right accessors, see issue #2119
f1 = ot.SymbolicFunction(["x1", "x2"], ["2 * x1 - x2"])
g1 = ot.SymbolicFunction(["x1", "x2"], ["x1 + x2", "3 * x1 * x2"])
composed = ot.ComposedFunction(f1, g1)
leftValue = composed.getLeftFunction()([1.0, 2.0])
ott.assert_almost_equal(leftValue, f1([1.0, 2.0]))
rightValue = composed.getRightFunction()([1.0, 2.0])
ott.assert_almost_equal(rightValue, g1([1.0, 2.0]))
f2 = ot.SymbolicFunction(["x1", "x2"], ["4 * x1 - x2^2"])
composed.setLeftFunction(f2)
leftValue = composed.getLeftFunction()([1.0, 2.0])
ott.assert_almost_equal(leftValue, f2([1.0, 2.0]))
ott.assert_almost_equal(composed([1.0, 2.0]), f2(g1([1.0, 2.0])))
h1 = ot.SymbolicFunction(["x1", "x2"], ["x1 - x2", "x1 + x2"])
composed.setRightFunction(h1)
rightValue = composed.getRightFunction()([1.0, 2.0])
ott.assert_almost_equal(rightValue, h1([1.0, 2.0]))
ott.assert_almost_equal(composed([1.0, 2.0]), f2(h1([1.0, 2.0])))
# gradient consistency after a setter
grad = composed.gradient([3.0, -1.0])
eps = 1e-5
fd = ot.Point(2)
for j in range(2):
    xp = [3.0, -1.0]
    xm = [3.0, -1.0]
    xp[j] += eps
    xm[j] -= eps
    fd[j] = (composed(xp)[0] - composed(xm)[0]) / (2.0 * eps)
for j in range(2):
    ott.assert_almost_equal(grad[j, 0], fd[j], 1e-4, 1e-4)

# replacing a function by one without a derivative must not leak a stale
# gradient/hessian from the previous composition
plainLeft = ot.SymbolicFunction(["x1", "x2"], ["sin(x1) + x2"])
plainLeft.setGradient(ot.NoGradient())
plainLeft.setHessian(ot.NoHessian())
composed = ot.ComposedFunction(f2, h1)
composed.setLeftFunction(plainLeft)
# the analytic derivative of the new composition falls back on finite
# differences, it must not reuse the derivative of the previous composition
x = [1.0, 2.0]
gradFD = ot.CenteredFiniteDifferenceGradient(1e-5, composed.getEvaluation()).gradient(x)
ott.assert_almost_equal(composed.gradient(x), gradFD, 1e-4, 1e-4)
hessFD = ot.CenteredFiniteDifferenceHessian(1e-5, composed.getEvaluation()).hessian(x)
ott.assert_almost_equal(composed.hessian(x), hessFD, 1e-3, 1e-4)
