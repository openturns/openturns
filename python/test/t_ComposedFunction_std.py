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

# ComposedEvaluation extra coverage (no prints to keep expout stable)
ev = ot.ComposedEvaluation(f2.getEvaluation(), h1.getEvaluation())
_ = repr(ev)
_ = str(ev)
assert ev.getInputDimension() == h1.getInputDimension()
assert ev.getOutputDimension() == f2.getOutputDimension()
_ = ev.getParameter()
_ = ev.getParameterDescription()
ev.setParameter(ev.getParameter())
ev.setParameterDescription(ev.getParameterDescription())
with ott.assert_raises(Exception):
    ev.setParameter([0.0])
with ott.assert_raises(Exception):
    ev.setParameterDescription(["x"])
_ = ev.getMarginal(0)
_ = ev.getMarginal([0])
_ = ev.getLeftEvaluation()
_ = ev.getRightEvaluation()
assert ev.isLinear() == (f2.isLinear() and h1.isLinear())
_ = ev.isLinearlyDependent(0)
ott.assert_almost_equal(ev(x), ot.ComposedFunction(f2, h1)(x), 1e-14, 1e-14)
ott.assert_almost_equal(ev(ot.Sample([x, x])), ot.ComposedFunction(f2, h1)(ot.Sample([x, x])), 1e-14, 1e-14)
with ott.assert_raises(Exception):
    ev([1.0])
with ott.assert_raises(Exception):
    ev.getMarginal(10)
with ott.assert_raises(Exception):
    ot.ComposedEvaluation(f2.getEvaluation(), ot.SymbolicFunction(["a"], ["a", "2*a", "3*a"]).getEvaluation())
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("composed_eval.xml"))
study.add("ev", ot.Function(ev))
study.save()

# ComposedGradient direct coverage
cfRef = ot.ComposedFunction(f2, h1)
cg = ot.ComposedGradient(f2.getGradient(), h1.getEvaluation(), h1.getGradient())
_ = repr(cg)
_ = str(cg)
assert cg.getInputDimension() == h1.getInputDimension()
assert cg.getOutputDimension() == f2.getOutputDimension()
assert cg.isActualImplementation()
_ = cg.getParameter()
_ = cg.getCallsNumber()
ott.assert_almost_equal(cg.gradient(x), cfRef.gradient(x), 1e-12, 1e-12)
ott.assert_almost_equal(cfRef.getGradient().gradient(x), cfRef.gradient(x), 1e-12, 1e-12)
_ = str(cfRef.getGradient())
assert cfRef.getGradient() == cfRef.getGradient()
assert not (cfRef.getGradient() != cfRef.getGradient())
_ = cfRef.getGradient().getCallsNumber()
with ott.assert_raises(Exception):
    cg.gradient(ot.Point([1.0]))
with ott.assert_raises(Exception):
    ot.ComposedGradient(ot.SymbolicFunction(["a", "b", "c"], ["a"]).getGradient(), h1.getEvaluation(), h1.getGradient())
with ott.assert_raises(Exception):
    ot.ComposedGradient(f2.getGradient(), ot.SymbolicFunction(["a", "b", "c"], ["a", "b"]).getEvaluation(), h1.getGradient())
with ott.assert_raises(Exception):
    ot.ComposedGradient(f2.getGradient(), ot.SymbolicFunction(["x1", "x2"], ["a", "b", "c"]).getEvaluation(), h1.getGradient())

# ComposedHessian direct coverage
ch = ot.ComposedHessian(f2.getGradient(), f2.getHessian(), h1.getEvaluation(), h1.getGradient(), h1.getHessian())
_ = repr(ch)
_ = str(ch)
assert ch.getInputDimension() == h1.getInputDimension()
assert ch.getOutputDimension() == f2.getOutputDimension()
assert ch.isActualImplementation()
_ = ch.getParameter()
_ = ch.getCallsNumber()
ott.assert_almost_equal(ch.hessian(x), cfRef.hessian(x), 1e-12, 1e-12)
ott.assert_almost_equal(cfRef.getHessian().hessian(x), cfRef.hessian(x), 1e-12, 1e-12)
_ = str(cfRef.getHessian())
assert cfRef.getHessian() == cfRef.getHessian()
assert not (cfRef.getHessian() != cfRef.getHessian())
_ = cfRef.getHessian().getCallsNumber()
with ott.assert_raises(Exception):
    ch.hessian(ot.Point([1.0]))
badRight = ot.SymbolicFunction(["a", "b", "c"], ["a", "b"]).getEvaluation()
with ott.assert_raises(Exception):
    ot.ComposedHessian(f2.getGradient(), f2.getHessian(), badRight, h1.getGradient(), h1.getHessian())
badLeftGrad = ot.SymbolicFunction(["a", "b", "c"], ["a"]).getGradient()
with ott.assert_raises(Exception):
    ot.ComposedHessian(badLeftGrad, f2.getHessian(), h1.getEvaluation(), h1.getGradient(), h1.getHessian())
badLeftHess = ot.SymbolicFunction(["a", "b", "c"], ["a"]).getHessian()
with ott.assert_raises(Exception):
    ot.ComposedHessian(badLeftGrad, badLeftHess, h1.getEvaluation(), h1.getGradient(), h1.getHessian())

# ComposedFunction indirect paths
_ = repr(cfRef)
_ = str(cfRef)
assert cfRef == cfRef
assert not (cfRef != cfRef)
_ = cfRef.getCallsNumber()
_ = cfRef.getEvaluation().getCallsNumber()
ott.assert_almost_equal(cfRef(x), f2(h1(x)), 1e-12, 1e-12)
ott.assert_almost_equal(cfRef(ot.Sample([x, x])), ot.Sample([cfRef(x), cfRef(x)]), 1e-12, 1e-12)
ott.assert_almost_equal(cfRef.getEvaluation()(x), cfRef(x), 1e-12, 1e-12)
ott.assert_almost_equal(cfRef.getEvaluation()(ot.Sample([x, x])), cfRef(ot.Sample([x, x])), 1e-12, 1e-12)
_ = cfRef.getMarginal(0)(x)
with ott.assert_raises(Exception):
    cfRef(ot.Point([1.0]))
with ott.assert_raises(Exception):
    cfRef.getEvaluation()(ot.Point([1.0]))
with ott.assert_raises(Exception):
    ot.ComposedFunction(f2, ot.SymbolicFunction(["a"], ["a", "2*a", "3*a"]))

# ComposedEvaluation extra paths
_ = ev._repr_html_()
assert ev.isActualImplementation()
_ = ev.getCallsNumber()
_ = ev.parameterGradient(x)
_ = ev.getMarginal([])
assert ev == ot.ComposedEvaluation(f2.getEvaluation(), h1.getEvaluation())
assert not (ev != ot.ComposedEvaluation(f2.getEvaluation(), h1.getEvaluation()))
with ott.assert_raises(Exception):
    ev.getMarginal([0, 0])
with ott.assert_raises(Exception):
    ev.getMarginal([0, 5])
with ott.assert_raises(Exception):
    ev(ot.Sample([[1.0]]))

# NoEvaluation / NoGradient / NoHessian direct coverage
ne = ot.NoEvaluation()
_ = repr(ne)
_ = str(ne)
assert ne.getInputDimension() == 0
assert ne.getOutputDimension() == 0
assert not ne.isActualImplementation()
assert ne == ot.NoEvaluation()
assert not (ne != ot.NoEvaluation())
_ = ne.getCallsNumber()
_ = ne.getParameter()
_ = ne.getParameterDescription()
_ = ne(ot.Point())
with ott.assert_raises(Exception):
    ne(ot.Point([1.0]))
with ott.assert_raises(Exception):
    ne.getMarginal(0)
ng = ot.NoGradient()
_ = repr(ng)
_ = str(ng)
assert ng.getInputDimension() == 0
assert ng.getOutputDimension() == 0
assert not ng.isActualImplementation()
assert ng == ot.NoGradient()
assert not (ng != ot.NoGradient())
_ = ng.getCallsNumber()
with ott.assert_raises(Exception):
    ng.gradient(ot.Point())
nh = ot.NoHessian()
_ = repr(nh)
_ = str(nh)
assert nh.getInputDimension() == 0
assert nh.getOutputDimension() == 0
assert not nh.isActualImplementation()
assert nh == ot.NoHessian()
assert not (nh != ot.NoHessian())
_ = nh.getCallsNumber()
with ott.assert_raises(Exception):
    nh.hessian(ot.Point())
