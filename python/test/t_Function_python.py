#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import sys

ot.TESTPREAMBLE()


class FUNC(ot.OpenTURNSPythonFunction):
    def __init__(self):
        super(FUNC, self).__init__(2, 1)
        self.setInputDescription(["R", "S"])
        self.setOutputDescription(["T"])

    def _exec(self, X):
        Y = [X[0] + X[1]]
        return Y


F = FUNC()
print(
    ("in_dim=" + str(F.getInputDimension()) + " out_dim=" + str(F.getOutputDimension()))
)
y = F((10, 5))
assert y == [15.0]
y = F(((10, 5), (6, 7)))
assert y == [[15.0], [13.0]]

# Instance creation
myFunc = ot.Function(F)

# Copy constructor
newFunc = ot.Function(myFunc)

assert myFunc.getInputDimension() == 2
assert myFunc.getOutputDimension() == 1

inPt = ot.Point(2, 2.0)
outPt = myFunc(inPt)
ott.assert_almost_equal(outPt, [4.0])

outPt = myFunc((10.0, 11.0))
ott.assert_almost_equal(outPt, [21.0])

inSample = ot.Sample(10, 2)
for i in range(10):
    inSample[i] = ot.Point((i, i))

outSample = myFunc(inSample)
print((repr(outSample)))
ott.assert_almost_equal(outSample, [[2 * i] for i in range(10)])

outSample = myFunc(((100.0, 100.0), (101.0, 101.0), (102.0, 102.0)))
print((repr(outSample)))
ott.assert_almost_equal(outSample, [[200], [202], [204]])

# test PythonFunction


def a_exec(X):
    Y = [0]
    Y[0] = X[0] + X[1]
    return Y


def a_exec_sample(Xs):
    Ys = []
    for X in Xs:
        Ys.append([X[0] + X[1]])
    return Ys


a_sample = ((100.0, 100.0), (101.0, 101.0), (102.0, 102.0))

print("exec")
myFunc = ot.PythonFunction(2, 1, a_exec)
outSample = myFunc(a_sample)
print(outSample)
ott.assert_almost_equal(outSample, [[200], [202], [204]])

print("exec + exec_sample")
myFunc = ot.PythonFunction(2, 1, a_exec, a_exec_sample)
outSample = myFunc(a_sample)
print(outSample)
ott.assert_almost_equal(outSample, [[200], [202], [204]])

print("exec_sample only on a point")
myFunc = ot.PythonFunction(2, 1, func_sample=a_exec_sample)
outSample = myFunc([100.0, 100.0])
print(outSample)
ott.assert_almost_equal(outSample, [200])

print("exec_sample only on a sample")
myFunc = ot.PythonFunction(2, 1, func_sample=a_exec_sample)
outSample = myFunc(a_sample)
print(outSample)
ott.assert_almost_equal(outSample, [[200], [202], [204]])

# multiprocessing spawn method on win (& osx for py>38) duplicates the output
cpus = -1 if sys.platform.startswith("linux") else None
print("distributed exec only on a point")
myFunc = ot.PythonFunction(2, 1, a_exec, n_cpus=cpus)
outSample = myFunc([100.0, 100.0])
print(outSample)
ott.assert_almost_equal(outSample, [200])

print("distributed exec only on a sample")
myFunc = ot.PythonFunction(2, 1, a_exec, n_cpus=cpus)
outSample = myFunc(a_sample)
print(outSample)
ott.assert_almost_equal(outSample, [[200], [202], [204]])


def a_grad(X):
    # wrong values but allows one to verify
    dY = [[1.0], [-1.0]]
    return dY


print("gradient")
myFunc = ot.PythonFunction(2, 1, a_exec, gradient=a_grad)
grad = myFunc.gradient([100.0, 100.0])
print(grad)


def wrong_dim_grad(X):
    # wrong dim: 3 rows, 1 column
    dY = [[1.0, -1.0, 42.0]]
    return dY


print("gradient with wrong dim")
myFunc = ot.PythonFunction(2, 1, a_exec, gradient=wrong_dim_grad)
with ott.assert_raises(RuntimeError):
    grad = myFunc.gradient([100.0, 100.0])


def a_hess(X):
    # wrong values but allows one to verify
    d2Y = [[[0.1], [0.3]], [[0.3], [0.1]]]
    return d2Y


print("hessian")
myFunc = ot.PythonFunction(2, 1, a_exec, hessian=a_hess)
hess = myFunc.hessian([100.0, 100.0])
print(hess)

print("no func")
try:
    myFunc = ot.PythonFunction(2, 1)
    outSample = myFunc(a_sample)
except Exception:
    # must raise exception
    print("no function detected : ok.")
else:
    raise Exception("no function not detected!")


def a_exec2(X):
    Y = [0]
    if X[0] == 0.0:
        raise RuntimeError("Oups")
    elif X[0] == 1.0:
        "2" + 2
    return Y


for n in range(2):
    myFunc = ot.PythonFunction(1, 1, a_exec2)
    try:
        X = ot.Point(1, n)
        myFunc(X)
    except Exception:
        # print exc
        print("exception handling: ok")


def a_exec3(X):
    X[8000]  # index error
    return [X[0] + X[1]]


myFunc = ot.PythonFunction(2, 1, a_exec3)
with ott.assert_raises(RuntimeError):
    myFunc([5, 6])

f = ot.PythonFunction(0, 3, lambda x: [42.0] * 3)
x = []
y = f(x)
print("y=", y)
ott.assert_almost_equal(y, [42.0] * 3)


class BFunction(ot.OpenTURNSPythonFunction):
    def __init__(self):
        # super().__init__(3, 1)
        pass

    def _exec(self, X):
        return [X[0] + X[1]]


bModel = BFunction()
with ott.assert_raises(RuntimeError):
    # when super is not called in init, should not crash
    bFunction = ot.Function(bModel)
