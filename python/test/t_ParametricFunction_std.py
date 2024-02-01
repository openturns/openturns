#!/usr/bin/env python

import openturns as ot

f = ot.SymbolicFunction(
    ot.Description.BuildDefault(5, "x"),
    [
        "sin(x0) + x1 * cos(x2) / exp(x3)",
        "-2.0 * x0 + x1 * x2^2 + cos(x3)",
        "x0 / (abs(x1) * x2^2 + x3 + 1.0)",
    ],
)
pset = [3, 1]  # x3, x1
parametersSet = True
x = [1.0, 2.0, 0.0]  # x0, x2, x4

# with reference point
referencePoint = [0.85] * 2  # x3, x1
g2 = ot.ParametricFunction(f, pset, referencePoint, parametersSet)
print("g2=", g2)
print("g2(x)=", g2(x))

# check output description
outputDesc = ot.Description.BuildDefault(3, "g2_")
g2.setOutputDescription(outputDesc)
X = ot.Normal(3).getSample(10)
Y = g2(X)
assert Y.getDescription() == outputDesc, "wrong output description"

# with complementary parameter set
referencePoint = [1.0, 2.0, 0.0]  # x0, x2, x4
x = [0.85] * 2  # x3, x1
parametersSet = False
g3 = ot.ParametricFunction(f, pset, referencePoint, parametersSet)
print("g3=", g3)
print("g3(x)=", g3(x))

# From ticket #1092
modelFunc = ot.SymbolicFunction(["s", "r", "c"], ["r + c"])
s = [1.0]
r = 1e9
c = 1e8
theta = ot.Point([r, c])
calibratedIndices = [1, 2]
mycf = ot.ParametricFunction(modelFunc, calibratedIndices, theta)

print(modelFunc.gradient([s[0], r, c]).transpose())
print(mycf.parameterGradient(s).transpose())

# Check if parametric functions and memoize functions work well together

n_calls = 0


def py_f(X):
    global n_calls
    n_calls += 1
    return X


ot_f = ot.MemoizeFunction(ot.PythonFunction(3, 3, py_f))
param_f = ot.ParametricFunction(ot_f, [0, 1], [1.0, 2.0])

x = [3.0]
y = [1.5]
n_calls_0 = ot_f.getEvaluationCallsNumber()
par_grad = param_f.parameterGradient(x)
n_calls_1 = ot_f.getEvaluationCallsNumber()
assert (
    n_calls_1 - n_calls_0 == 4
), "Expected n_calls_1 - n_calls_0 == 4, here n_calls_1 - n_calls_0 == " + str(
    n_calls_1 - n_calls_0
)
assert n_calls == 4, "Expected n_calls == 4, here n_calls == " + str(n_calls)

n_calls = 0
n_calls_0 = ot_f.getEvaluationCallsNumber()
f_grad = param_f.gradient(y)
n_calls_1 = ot_f.getEvaluationCallsNumber()
assert (
    n_calls_1 - n_calls_0 == 2
), "Expected n_calls_1 - n_calls_0 == 2, here n_calls_1 - n_calls_0 == " + str(
    n_calls_1 - n_calls_0
)
assert n_calls == 2, "Expected n_calls == 2, here n_calls == " + str(n_calls)

eps = 1e-7
gr_f = ot.NonCenteredFiniteDifferenceGradient(eps, ot_f.getEvaluation())
ot_f.setGradient(gr_f)
n_calls = 0
param_f = ot.ParametricFunction(ot_f, [0, 1], [1.0, 2.0])

x = [3.5]
y = [2.5]
n_calls_0 = ot_f.getEvaluationCallsNumber()
par_grad = param_f.parameterGradient(x)
n_calls_1 = ot_f.getEvaluationCallsNumber()
assert (
    n_calls_1 - n_calls_0 == 3
), "Expected n_calls_1 - n_calls_0 == 3, here n_calls_1 - n_calls_0 == " + str(
    n_calls_1 - n_calls_0
)
assert n_calls == 3, "Expected n_calls == 3, here n_calls == " + str(n_calls)

n_calls = 0
n_calls_0 = ot_f.getEvaluationCallsNumber()
f_grad = param_f.gradient(y)
n_calls_1 = ot_f.getEvaluationCallsNumber()
assert (
    n_calls_1 - n_calls_0 == 2
), "Expected n_calls_1 - n_calls_0 == 2, here n_calls_1 - n_calls_0 == " + str(
    n_calls_1 - n_calls_0
)
assert n_calls == 2, "Expected n_calls == 2, here n_calls == " + str(n_calls)
