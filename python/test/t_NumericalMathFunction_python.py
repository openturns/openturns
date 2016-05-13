#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()


class FUNC(OpenTURNSPythonFunction):

    def __init__(self):
        super(FUNC, self).__init__(2, 1)
        self.setInputDescription(['R', 'S'])
        self.setOutputDescription(['T'])

    def _exec(self, X):

        Y = [X[0] + X[1]]
        return Y

F = FUNC()
print(('in_dim=' + str(F.getInputDimension())
       + ' out_dim=' + str(F.getOutputDimension())))

print((F((10, 5))))

print((F(((10, 5), (6, 7)))))


# Instance creation
myFunc = NumericalMathFunction(F)

# Copy constructor
newFunc = NumericalMathFunction(myFunc)

print(('myFunc input dimension= ' + str(myFunc.getInputDimension())))
print(('myFunc output dimension= ' + str(myFunc.getOutputDimension())))

inPt = NumericalPoint(2, 2.)
print((repr(inPt)))

outPt = myFunc(inPt)
print((repr(outPt)))

outPt = myFunc((10., 11.))
print((repr(outPt)))

inSample = NumericalSample(10, 2)
for i in range(10):
    inSample[i] = NumericalPoint((i, i))
print((repr(inSample)))

outSample = myFunc(inSample)
print((repr(outSample)))

outSample = myFunc(((100., 100.), (101., 101.), (102., 102.)))
print((repr(outSample)))

# Test cache behavior
myFunc.enableCache()
print(('calls = ' + str(myFunc.getEvaluationCallsNumber())
       + ' hits = ' + str(myFunc.getCacheHits())))
outPt = myFunc(inPt)
print(('T = ' + repr(outPt)))
print(('calls = ' + str(myFunc.getEvaluationCallsNumber())
       + ' hits = ' + str(myFunc.getCacheHits())))
outPt = myFunc(inPt)
print(('T = ' + repr(outPt)))
print(('calls = ' + str(myFunc.getEvaluationCallsNumber())
       + ' hits = ' + str(myFunc.getCacheHits())))

# duplicate one value
inSample[4] = inSample[5]

outSample = myFunc(inSample)
print(('T = ' + repr(outSample)))
print(('calls = ' + str(myFunc.getEvaluationCallsNumber())
       + ' hits = ' + str(myFunc.getCacheHits())))
outSample = myFunc(inSample)
print(('T = ' + repr(outSample)))
print(('calls = ' + str(myFunc.getEvaluationCallsNumber())
       + ' hits = ' + str(myFunc.getCacheHits())))

myFunc.clearCache()
outSample = myFunc(inSample)
print(('T = ' + repr(outSample)))
print(('calls = ' + str(myFunc.getEvaluationCallsNumber())
       + ' hits = ' + str(myFunc.getCacheHits())))

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

a_sample = ((100., 100.), (101., 101.), (102., 102.))

print('exec')
myFunc = PythonFunction(2, 1, a_exec)
outSample = myFunc(a_sample)
print(outSample)

print('exec + exec_sample')
myFunc = PythonFunction(2, 1, a_exec, a_exec_sample)
outSample = myFunc(a_sample)
print(outSample)

print('exec_sample only on a point')
myFunc = PythonFunction(2, 1, func_sample=a_exec_sample)
outSample = myFunc([100., 100.])
print(outSample)

print('exec_sample only on a sample')
myFunc = PythonFunction(2, 1, func_sample=a_exec_sample)
outSample = myFunc(a_sample)
print(outSample)


def a_grad(X):
    # wrong but allows to verify
    dY = [[1.], [-1.]]
    return dY

print('gradient')
myFunc = PythonFunction(2, 1, a_exec, gradient=a_grad)
grad = myFunc.gradient([100., 100.])
print(grad)


def a_hess(X):
    # wrong but allows to verify
    d2Y = [[[0.1], [0.3]], [[0.3], [0.1]]]
    return d2Y

print('hessian')
myFunc = PythonFunction(2, 1, a_exec, hessian=a_hess)
hess = myFunc.hessian([100., 100.])
print(hess)

print('no func')
try:
    myFunc = PythonFunction(2, 1)
    outSample = myFunc(a_sample)
except:
    # must raise exception
    print('no function detected : ok.')
else:
    raise Exception('no function not detected!')



def a_exec(X):
    Y = [0]
    if X[0] == 0.0:
        raise RuntimeError('Oups')
    elif X[0] == 1.0:
        '2' + 2
    return Y

for n in range(2):
    myFunc = PythonFunction(1, 1, a_exec)
    try:
        X = NumericalPoint(1, n)
        myFunc(X)
    except Exception as exc:
        # print exc
        print('exception handling: ok')


class FUNC(OpenTURNSPythonFunction):
    def __init__(self):
        super(FUNC, self).__init__(2, 1)
        self.setInputDescription(['R', 'S'])
        self.setOutputDescription(['T'])
    def _exec(self, X):
        theta = self.getParameter()
        Y = [X[0] + X[1]*theta[0]]
        return Y

myFunc = NumericalMathFunction(FUNC())
myFunc.setParameterDescription(['theta'])
x = [4.0, 5.0]
theta = [1.2]
print('x=', x, 'theta=', theta, 'f(x, theta)=', myFunc(x, theta))
print(myFunc.getParameter(), myFunc.getParameterDescription())