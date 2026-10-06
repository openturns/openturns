#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instance creation
myFunc = ot.SymbolicFunction(
    ["x1", "x2"], ["x1*sin(x2)", "cos(x1+x2)", "(x2+1)*exp(x1-2*x2)"]
)

# Copy constructor
newFunc = ot.Function(myFunc)

print("myFunc=" + repr(myFunc))
print("myFunc input parameter(s)=")
for i in range(myFunc.getInputDimension()):
    print(myFunc.getInputDescription()[i])
print("myFunc output parameter(s) and marginal(s)=")
for i in range(myFunc.getOutputDimension()):
    print(myFunc.getOutputDescription()[i])
    print("Marginal function", i, "=", repr(myFunc.getMarginal(i)))

# FunctionCollection slicing
collection = ot.FunctionCollection()
for i in range(5):
    collection.add(ot.SymbolicFunction(["x"], [str(i) + " * x"]))
sliced = collection[1:3]
assert isinstance(sliced, ot.FunctionCollection), "slice type"
assert sliced.getSize() == 2, "slice size"
ott.assert_almost_equal(sliced[0]([2.0])[0], 2.0)
stepped = collection[0:5:2]
assert stepped.getSize() == 3, "stepped slice size"
ott.assert_almost_equal(stepped[2]([2.0])[0], 8.0)
negative = collection[-2:]
assert negative.getSize() == 2, "negative slice size"
selected = collection[[4, 0]]
assert selected.getSize() == 2, "sequence size"
ott.assert_almost_equal(selected[0]([2.0])[0], 8.0)
ott.assert_almost_equal(selected[1]([2.0])[0], 0.0)
single = collection[2]
ott.assert_almost_equal(single([2.0])[0], 4.0)
collection[1:3] = ot.FunctionCollection([ot.SymbolicFunction(["x"], ["9*x"]), ot.SymbolicFunction(["x"], ["8*x"])])
ott.assert_almost_equal(collection[1]([2.0])[0], 18.0)
collection[[0, 4]] = ot.FunctionCollection([ot.SymbolicFunction(["x"], ["7*x"]), ot.SymbolicFunction(["x"], ["6*x"])])
ott.assert_almost_equal(collection[0]([1.0])[0], 7.0)

# ProductEvaluation / ProductGradient coverage
left = ot.SymbolicFunction(["x1", "x2"], ["x1 + x2"])
right = ot.SymbolicFunction(["x1", "x2"], ["x1 - x2", "x1 * x2"])
x = [1.0, 2.0]
peval = ot.ProductEvaluation(left.getEvaluation(), right.getEvaluation())
prod = ot.Function(peval)
ott.assert_almost_equal(prod(x), [-3.0, 6.0], 1e-14, 1e-14)
ott.assert_almost_equal(prod(ot.Sample([x, x])), [[-3.0, 6.0], [-3.0, 6.0]], 1e-14, 1e-14)
pgrad = ot.ProductGradient(left.getEvaluation(), left.getGradient(), right.getEvaluation(), right.getGradient())
g = pgrad.gradient(x)
ott.assert_almost_equal(g[0, 0], 2.0, 1e-12, 1e-12)
ott.assert_almost_equal(g[0, 1], 8.0, 1e-12, 1e-12)
ott.assert_almost_equal(g[1, 0], -4.0, 1e-12, 1e-12)
ott.assert_almost_equal(g[1, 1], 5.0, 1e-12, 1e-12)
_ = repr(peval)
_ = str(peval)
_ = repr(pgrad)
assert pgrad.getInputDimension() == 2
assert pgrad.getOutputDimension() == 2
_ = peval.getParameter()
_ = peval.getParameterDescription()
peval.setParameter(peval.getParameter())
# swapped ctor branch: right has output dim 1
peval2 = ot.ProductEvaluation(right.getEvaluation(), left.getEvaluation())
ott.assert_almost_equal(ot.Function(peval2)(x), prod(x), 1e-14, 1e-14)
with ott.assert_raises(Exception):
    prod([1.0])
with ott.assert_raises(Exception):
    pgrad.gradient([1.0])
with ott.assert_raises(Exception):
    ot.ProductEvaluation(ot.SymbolicFunction(["x"], ["x", "2*x"]).getEvaluation(), ot.SymbolicFunction(["x"], ["x", "2*x"]).getEvaluation())
with ott.assert_raises(Exception):
    ot.ProductEvaluation(left.getEvaluation(), ot.SymbolicFunction(["y"], ["y"]).getEvaluation())
with ott.assert_raises(Exception):
    ot.ProductGradient(right.getEvaluation(), right.getGradient(), right.getEvaluation(), right.getGradient())
# MarginalEvaluation / MarginalGradient / MarginalHessian coverage
baseFunc = ot.SymbolicFunction(["x1", "x2"], ["x1+x2", "x1*x2", "x1-x2"])
marginalEval = ot.MarginalEvaluation(
    baseFunc.getEvaluation().getImplementation(), [0, 2]
)
marginalFunc = ot.Function(marginalEval)
assert list(marginalEval.getIndices()) == [0, 2]
assert marginalEval.getInputDimension() == 2
assert marginalEval.getOutputDimension() == 2
assert marginalEval.getEvaluation().getClassName() == "SymbolicEvaluation"
ott.assert_almost_equal(marginalFunc(x), [3.0, -1.0], 1e-14, 1e-14)
ott.assert_almost_equal(
    marginalFunc(ot.Sample([x, x])), [[3.0, -1.0], [3.0, -1.0]], 1e-14, 1e-14
)
ott.assert_almost_equal(
    marginalFunc.getGradient().gradient(x),
    ot.Matrix([[1.0, 1.0], [1.0, -1.0]]),
    1e-7,
    1e-7,
)
assert list(marginalEval.getParameter()) == []
marginalEval.setParameter(marginalEval.getParameter())
marginalEval.setParameterDescription(marginalEval.getParameterDescription())
assert marginalEval.parameterGradient(x).getNbRows() == 0
marginalEval.setCheckOutput(True)
assert marginalEval.getCheckOutput()
_ = repr(marginalEval)
_ = str(marginalFunc)
assert marginalEval == marginalEval
assert marginalEval != ot.MarginalEvaluation(
    baseFunc.getEvaluation().getImplementation(), [0, 1]
)
assert marginalFunc.getEvaluationCallsNumber() > 0
_ = marginalEval.getCallsNumber()
marginalGrad = ot.MarginalGradient(
    baseFunc.getGradient().getImplementation(), [2, 0]
)
assert list(marginalGrad.getIndices()) == [2, 0]
assert marginalGrad.getGradient().getClassName() == "SymbolicGradient"
assert marginalGrad.getInputDimension() == 2
assert marginalGrad.getOutputDimension() == 2
ott.assert_almost_equal(
    marginalGrad.gradient(x),
    ot.Matrix([[1.0, 1.0], [-1.0, 1.0]]),
    1e-12,
    1e-12,
)
_ = repr(marginalGrad)
assert marginalGrad == marginalGrad
assert marginalGrad != ot.MarginalGradient(
    baseFunc.getGradient().getImplementation(), [0]
)
_ = marginalGrad.getCallsNumber()
marginalHess = ot.MarginalHessian(
    baseFunc.getHessian().getImplementation(), [1]
)
assert marginalHess.getInputDimension() == 2
assert marginalHess.getOutputDimension() == 1
ott.assert_almost_equal(
    marginalHess.hessian(x).getSheet(0),
    ot.Matrix([[0.0, 1.0], [1.0, 0.0]]),
    1e-12,
    1e-12,
)
_ = repr(marginalHess)
assert marginalHess == marginalHess
assert marginalHess != ot.MarginalHessian(
    baseFunc.getHessian().getImplementation(), [0]
)
_ = marginalHess.getCallsNumber()
with ott.assert_raises(Exception):
    ot.MarginalEvaluation(
        baseFunc.getEvaluation().getImplementation(), [0, 0]
    )
with ott.assert_raises(Exception):
    ot.MarginalEvaluation(
        baseFunc.getEvaluation().getImplementation(), [0, 5]
    )
with ott.assert_raises(Exception):
    marginalEval([1.0])
with ott.assert_raises(Exception):
    marginalEval([[1.0], [2.0]])
with ott.assert_raises(Exception):
    marginalFunc([1.0])
with ott.assert_raises(Exception):
    marginalFunc([[1.0], [2.0]])
with ott.assert_raises(Exception):
    marginalFunc.getGradient().gradient([1.0])
with ott.assert_raises(Exception):
    marginalFunc.getHessian().hessian([1.0])
with ott.assert_raises(Exception):
    ot.MarginalGradient(baseFunc.getGradient().getImplementation(), [1, 1])
with ott.assert_raises(Exception):
    ot.MarginalGradient(baseFunc.getGradient().getImplementation(), [7])
with ott.assert_raises(Exception):
    ot.MarginalGradient(
        baseFunc.getGradient().getImplementation(), [0]
    ).gradient([1.0])
with ott.assert_raises(Exception):
    ot.MarginalHessian(baseFunc.getHessian().getImplementation(), [2, 2])
with ott.assert_raises(Exception):
    ot.MarginalHessian(
        baseFunc.getHessian().getImplementation(), [0]
    ).hessian([1.0])
with ott.assert_raises(Exception):
    baseFunc.getMarginal(5)
with ott.assert_raises(Exception):
    baseFunc.getMarginal([0, 0])
with ott.assert_raises(Exception):
    baseFunc.getMarginal([0, 7])

# ProductHessian coverage
solo = ot.SymbolicFunction(["x"], ["x"])
phess = ot.ProductHessian(
    left.getEvaluation(),
    left.getGradient(),
    left.getHessian(),
    right.getEvaluation(),
    right.getGradient(),
    right.getHessian(),
)
assert phess.getInputDimension() == 2
assert phess.getOutputDimension() == 2
ott.assert_almost_equal(
    phess.hessian(x).getSheet(0),
    ot.Matrix([[2.0, 0.0], [0.0, -2.0]]),
    1e-12,
    1e-12,
)
ott.assert_almost_equal(
    phess.hessian(x).getSheet(1),
    ot.Matrix([[4.0, 6.0], [6.0, 2.0]]),
    1e-12,
    1e-12,
)
_ = repr(phess)
assert phess == phess
assert phess == ot.ProductHessian(
    left.getEvaluation(),
    left.getGradient(),
    left.getHessian(),
    right.getEvaluation(),
    right.getGradient(),
    right.getHessian(),
)
assert phess != ot.ProductHessian(
    solo.getEvaluation(),
    solo.getGradient(),
    solo.getHessian(),
    solo.getEvaluation(),
    solo.getGradient(),
    solo.getHessian(),
)
_ = phess.getCallsNumber()
prod.setGradient(pgrad)
prod.setHessian(phess)
ott.assert_almost_equal(
    prod.getHessian().hessian(x).getSheet(1),
    ot.Matrix([[4.0, 6.0], [6.0, 2.0]]),
    1e-12,
    1e-12,
)
assert prod.getHessianCallsNumber() > 0
with ott.assert_raises(Exception):
    phess.hessian([1.0])
with ott.assert_raises(Exception):
    ot.ProductHessian(
        right.getEvaluation(),
        right.getGradient(),
        right.getHessian(),
        right.getEvaluation(),
        right.getGradient(),
        right.getHessian(),
    )
with ott.assert_raises(Exception):
    ot.ProductHessian(
        left.getEvaluation(),
        left.getGradient(),
        left.getHessian(),
        solo.getEvaluation(),
        solo.getGradient(),
        solo.getHessian(),
    )
with ott.assert_raises(Exception):
    ot.ProductHessian(
        left.getEvaluation(),
        right.getGradient(),
        left.getHessian(),
        right.getEvaluation(),
        right.getGradient(),
        right.getHessian(),
    )
with ott.assert_raises(Exception):
    ot.ProductHessian(
        left.getEvaluation(),
        left.getGradient(),
        right.getHessian(),
        right.getEvaluation(),
        right.getGradient(),
        right.getHessian(),
    )

# ProductEvaluation / ProductGradient extra coverage
assert peval == peval
assert peval == peval2
assert peval.getInputDimension() == 2
assert peval.getOutputDimension() == 2
_ = peval.getCallsNumber()
assert pgrad == pgrad
_ = pgrad.getCallsNumber()
with ott.assert_raises(Exception):
    prod([[1.0], [2.0]])
with ott.assert_raises(Exception):
    ot.ProductGradient(
        left.getEvaluation(),
        left.getGradient(),
        solo.getEvaluation(),
        solo.getGradient(),
    )
with ott.assert_raises(Exception):
    ot.ProductGradient(
        left.getEvaluation(),
        solo.getGradient(),
        left.getEvaluation(),
        left.getGradient(),
    )

# ProductFunction coverage
productFunc = left * right
assert productFunc.getInputDimension() == 2
assert productFunc.getOutputDimension() == 2
ott.assert_almost_equal(productFunc(x), [-3.0, 6.0], 1e-14, 1e-14)
ott.assert_almost_equal(
    productFunc(ot.Sample([x, x])), [[-3.0, 6.0], [-3.0, 6.0]], 1e-14, 1e-14
)
ott.assert_almost_equal(
    productFunc.getGradient().gradient(x),
    ot.Matrix([[2.0, 8.0], [-4.0, 5.0]]),
    1e-12,
    1e-12,
)
ott.assert_almost_equal(
    productFunc.getHessian().hessian(x).getSheet(0),
    ot.Matrix([[2.0, 0.0], [0.0, -2.0]]),
    1e-12,
    1e-12,
)
_ = str(productFunc)
_ = repr(productFunc)
assert productFunc == left * right
ott.assert_almost_equal(
    productFunc.getMarginal([0])(x), [-3.0], 1e-14, 1e-14
)
_ = productFunc.parameterGradient(x)
assert productFunc.getCallsNumber() > 0
assert productFunc.getEvaluationCallsNumber() > 0
assert productFunc.getGradientCallsNumber() > 0
assert productFunc.getHessianCallsNumber() > 0
with ott.assert_raises(Exception):
    productFunc([1.0])
with ott.assert_raises(Exception):
    productFunc([[1.0], [2.0]])
with ott.assert_raises(Exception):
    productFunc.getGradient().gradient([1.0])
with ott.assert_raises(Exception):
    productFunc.getHessian().hessian([1.0])
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("product_func.xml"))
study.add("prod", prod)
study.save()
