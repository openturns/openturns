#! /usr/bin/env python

import openturns as ot
from openturns.testing import assert_almost_equal


f = ot.SymbolicFunction("x", "x^2")
f = ot.MemoizeFunction(f)
f.disableHistory()
print(f)
size = 4
inputSample = ot.Sample(size, 1)
for i in range(size):
    inputSample[i, 0] = i
outputSample = f(inputSample)
print("Is history enabled for f? ", f.isHistoryEnabled())
print("input history=", f.getInputHistory())
print("output history=", f.getOutputHistory())
f.enableHistory()
outputSample = f(inputSample)
print("Is history enabled for f? ", f.isHistoryEnabled())
print("input history=", f.getInputHistory())
print("output history=", f.getOutputHistory())
f.clearHistory()
print("Is history enabled for f? ", f.isHistoryEnabled())
print("input history=", f.getInputHistory())
print("output history=", f.getOutputHistory())
# Perform the computation twice
outputSample = f(inputSample)
outputSample = f(inputSample)
print("input history=", f.getInputHistory())
print("output history=", f.getOutputHistory())

mem = ot.MemoizeFunction(ot.SymbolicFunction(["x1", "x2"], ["x1+x2", "3*(x1+x2)"]))
mem([1, 2])
mem2 = ot.MemoizeFunction(mem.getMarginal(1))
mem2([1, 2])
print("mem2.in", mem2.getCacheInput())
print("mem2.out", mem2.getCacheOutput())
print("mem2.hits", mem2.getCacheHits())

# check getCacheInput/getCacheOutput order
for i in range(5):
    mem([i + 2, i + 3])
assert mem(mem.getCacheInput()) == mem.getCacheOutput(), "wrong order"
print("ok")

ot.PlatformInfo.SetNumericalPrecision(20)


def py_f(X):
    return X


# Check if the meoization propagates through the finite difference gradients
# Here we use a PythonFunction as its gradient/hessian are based on finite
# differences by default
ot_f = ot.MemoizeFunction(ot.PythonFunction(3, 3, py_f))
x = [1.0, 2.0, 3.0]
n_calls_0 = ot_f.getEvaluationCallsNumber()
res_f = ot_f(x)
res_grad = ot_f.gradient(x)
res_hess = ot_f.hessian(x)
n_calls_1 = ot_f.getEvaluationCallsNumber()
# 25=1+6+18
assert_almost_equal(n_calls_1 - n_calls_0, 25, 0.0, 0.0)
# Do the computation once again
n_calls_0 = n_calls_1
res_f = ot_f(x)
res_grad = ot_f.gradient(x)
res_hess = ot_f.hessian(x)
n_calls_1 = ot_f.getEvaluationCallsNumber()
# 0=everything is reused
assert_almost_equal(n_calls_1 - n_calls_0, 0, 0.0, 0.0)
# Now, switch to noncentered gradients to reduce the calls to the minimum
eps = 1e-8
gr_f = ot.NonCenteredFiniteDifferenceGradient(eps, ot_f.getEvaluation())
ot_f.setGradient(gr_f)
x = [3, 1, 2]
n_calls_0 = n_calls_1
res_f = ot_f(x)
res_grad = ot_f.gradient(x)
res_hess = ot_f.hessian(x)
n_calls_1 = ot_f.getEvaluationCallsNumber()
# 22=1+3(3+1 reused)+18
assert_almost_equal(n_calls_1 - n_calls_0, 22, 0.0, 0.0)

# check that marginals share the same cache


def f_py(x):
    f_py.n += 1
    x0, x1 = x
    return [x0 + x1, x0 - x1]


f_py.n = 0
f_test = ot.MemoizeFunction(ot.PythonFunction(2, 2, f_py))
f1 = f_test.getMarginal(0)
f2 = f_test.getMarginal(1)
x = [1, 2]
f1(x)
f2(x)
assert f_py.n == 1, "only one eval"

# here we want f modified by ref though evals on f0
f = ot.MemoizeFunction(ot.SymbolicFunction(["x1", "x2"], ["x1+x2", "x1*x2"]))
x = ot.Normal(2).getSample(10)
f0 = ot.MemoizeFunction(f.getMarginal([0]))
f0(x)
assert f.getInputHistory().getSize() == len(x), "wrong size"
assert f.getCacheInput().getSize() == len(x), "wrong size"

# Extended cache API coverage (no prints)
defaultMemo = ot.MemoizeFunction()
assert defaultMemo.getInputDimension() == 0
assert "Function" in repr(defaultMemo)
g = ot.SymbolicFunction(["x"], ["x^2"])
memo = ot.MemoizeFunction(g)
assert memo.isCacheEnabled()
memo.disableCache()
assert not memo.isCacheEnabled()
memo.enableCache()
assert memo.isCacheEnabled()
memo([1.0])
memo([1.0])
assert memo.getCacheHits() == 1
memo.clearCache()
assert memo.getCacheHits() == 0
assert memo.getCacheInput().getSize() == 0
assert memo.getCacheOutput().getSize() == 0
memo.addCacheContent([[2.0]], [[4.0]])
assert_almost_equal(memo.getCacheInput()[0], [2.0], 0.0, 0.0)
assert_almost_equal(memo.getCacheOutput()[0], [4.0], 0.0, 0.0)
assert_almost_equal(memo([2.0]), [4.0], 0.0, 0.0)
assert memo.getCacheHits() == 1
memo.setEvaluation(ot.SymbolicFunction(["x"], ["2*x"]).getEvaluation())
assert_almost_equal(memo([3.0]), [6.0], 0.0, 0.0)
assert memo == memo
assert not (memo == ot.MemoizeFunction(g))
# constructor with an explicit history strategy
memoHS = ot.MemoizeFunction(g, ot.HistoryStrategy())
assert_almost_equal(memoHS([2.0]), [4.0], 0.0, 0.0)
# constructor rewires a non-centered finite-difference gradient
ncFunc = ot.SymbolicFunction(["x"], ["x^2"])
ncFunc.setGradient(ot.NonCenteredFiniteDifferenceGradient(1e-5, ncFunc.getEvaluation()))
memoNC = ot.MemoizeFunction(ncFunc)
assert_almost_equal(memoNC.gradient([2.0])[0, 0], 4.0, 1e-4, 1e-4)
# constructor rewires centered finite differences of a PythonFunction
memoPy = ot.MemoizeFunction(ot.PythonFunction(1, 1, lambda t: [t[0] ** 2]))
assert_almost_equal(memoPy.gradient([2.0])[0, 0], 4.0, 1e-4, 1e-4)
