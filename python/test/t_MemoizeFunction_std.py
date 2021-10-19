#! /usr/bin/env python

from __future__ import print_function
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

mem = ot.MemoizeFunction(ot.SymbolicFunction(
    ['x1', 'x2'], ['x1+x2', '3*(x1+x2)']))
mem([1, 2])
mem2 = ot.MemoizeFunction(mem.getMarginal(1))
mem2([1, 2])
print('mem2.in', mem2.getCacheInput())
print('mem2.out', mem2.getCacheOutput())
print('mem2.hits', mem2.getCacheHits())

# check getCacheInput/getCacheOutput order
for i in range(5):
    mem([i+2, i+3])
assert mem(mem.getCacheInput()) == mem.getCacheOutput(), "wrong order"
print('ok')

ot.PlatformInfo.SetNumericalPrecision(20)


def py_f(X):
    return X


# Check if the meoization propagates through the finite difference gradients
# Here we use a PythonFunction as its gradient/hessian are based on finite
# differences by default
ot_f = ot.MemoizeFunction(ot.PythonFunction(3, 3, py_f))
x = [1.0, 2.0, 3.0]
n_calls_0 = ot_f.getCallsNumber()
res_f = ot_f(x)
res_grad = ot_f.gradient(x)
res_hess = ot_f.hessian(x)
n_calls_1 = ot_f.getCallsNumber()
# 25=1+6+18
assert_almost_equal(n_calls_1 - n_calls_0, 25, 0.0, 0.0)
# Do the computation once again
n_calls_0 = n_calls_1
res_f = ot_f(x)
res_grad = ot_f.gradient(x)
res_hess = ot_f.hessian(x)
n_calls_1 = ot_f.getCallsNumber()
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
n_calls_1 = ot_f.getCallsNumber()
# 22=1+3(3+1 reused)+18
assert_almost_equal(n_calls_1 - n_calls_0, 22, 0.0, 0.0)
