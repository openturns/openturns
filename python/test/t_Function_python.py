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

print((F((10, 5))))

print((F(((10, 5), (6, 7)))))


# Instance creation
myFunc = ot.Function(F)

# Copy constructor
newFunc = ot.Function(myFunc)

print(("myFunc input dimension= " + str(myFunc.getInputDimension())))
print(("myFunc output dimension= " + str(myFunc.getOutputDimension())))

inPt = ot.Point(2, 2.0)
print((repr(inPt)))

outPt = myFunc(inPt)
print((repr(outPt)))

outPt = myFunc((10.0, 11.0))
print((repr(outPt)))

inSample = ot.Sample(10, 2)
for i in range(10):
    inSample[i] = ot.Point((i, i))
print((repr(inSample)))

outSample = myFunc(inSample)
print((repr(outSample)))

outSample = myFunc(((100.0, 100.0), (101.0, 101.0), (102.0, 102.0)))
print((repr(outSample)))

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

print("exec + exec_sample")
myFunc = ot.PythonFunction(2, 1, a_exec, a_exec_sample)
outSample = myFunc(a_sample)
print(outSample)

print("exec_sample only on a point")
myFunc = ot.PythonFunction(2, 1, func_sample=a_exec_sample)
outSample = myFunc([100.0, 100.0])
print(outSample)

print("exec_sample only on a sample")
myFunc = ot.PythonFunction(2, 1, func_sample=a_exec_sample)
outSample = myFunc(a_sample)
print(outSample)

# multiprocessing spawn method on win (& osx for py>38) duplicates the output
cpus = -1 if sys.platform.startswith("linux") else None
print("distributed exec only on a point")
myFunc = ot.PythonFunction(2, 1, a_exec, n_cpus=cpus)
outSample = myFunc([100.0, 100.0])
print(outSample)

print("distributed exec only on a sample")
myFunc = ot.PythonFunction(2, 1, a_exec, n_cpus=cpus)
outSample = myFunc(a_sample)
print(outSample)


def a_grad(X):
    # wrong but allows one to verify
    dY = [[1.0], [-1.0]]
    return dY


print("gradient")
myFunc = ot.PythonFunction(2, 1, a_exec, gradient=a_grad)
grad = myFunc.gradient([100.0, 100.0])
print(grad)


def a_hess(X):
    # wrong but allows one to verify
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


def a_exec(X):
    Y = [0]
    if X[0] == 0.0:
        raise RuntimeError("Oups")
    elif X[0] == 1.0:
        "2" + 2
    return Y


for n in range(2):
    myFunc = ot.PythonFunction(1, 1, a_exec)
    try:
        X = ot.Point(1, n)
        myFunc(X)
    except Exception:
        # print exc
        print("exception handling: ok")


def a_exec(X):
    X[8000]  # index error
    return [X[0] + X[1]]


myFunc = ot.PythonFunction(2, 1, a_exec)
with ott.assert_raises(RuntimeError):
    myFunc([5, 6])

f = ot.PythonFunction(0, 3, lambda x: [42.0] * 3)
x = []
y = f(x)
print("y=", y)


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
