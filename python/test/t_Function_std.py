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
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("product_func.xml"))
study.add("prod", prod)
study.save()
