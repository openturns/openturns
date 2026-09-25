#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

size = 100
xuniform = ot.Uniform(0.9, 1.1)
x = xuniform.getSample(size)
yuniform = ot.Uniform(1.9, 2.1)
y = yuniform.getSample(size)
w = [1.0] * size
f = ot.SymbolicFunction(["x"], ["2.0*x"])
basis = [f]
indices = list(range(len(basis)))

fittingAlgo = ot.KFold()

print("algo =", fittingAlgo)

result = fittingAlgo.run(x, y, w, basis, indices)

print("result = %g" % result)
# illustrate the other usages
proxy = ot.DesignProxy(x, basis)
method = ot.SVDMethod(proxy, indices)
# use the other run methods to cover them
result2 = fittingAlgo.run(y, w, indices, proxy)
ott.assert_almost_equal(result, result2, 1e-5, 1e-5)
result3 = fittingAlgo.run(y, indices, proxy)
ott.assert_almost_equal(result, result3, 1e-5, 1e-5)
result4 = fittingAlgo.run(method, y)
ott.assert_almost_equal(result, result4, 1e-5, 1e-5)

# Non-uniform weights: each fold must use the weights of its own rows
k = 5
wNonUniform = ot.Point([0.5 + (i % 4) * 0.25 for i in range(size)])
kfold = ot.KFold(k)
proxyW = ot.DesignProxy(x, basis)
methodW = ot.SVDMethod(proxyW, wNonUniform, indices)
resultW = kfold.run(methodW, y)
# Brute-force reference with the same fold partition and fresh solves
testSize = size // k
quadraticResidual = 0.0
totalTestWeight = 0.0
for i in range(k):
    trainX = ot.Sample(0, 1)
    trainW = []
    trainY = []
    testX = ot.Sample(0, 1)
    testY = []
    testW = []
    for j in range(k * testSize):
        if j % k != i:
            trainX.add(x[j])
            trainW.append(wNonUniform[j])
            trainY.append(y[j, 0])
        else:
            testX.add(x[j])
            testY.append(y[j, 0])
            testW.append(wNonUniform[j])
    trainProxy = ot.DesignProxy(trainX, basis)
    trainMethod = ot.SVDMethod(trainProxy, ot.Point(trainW), indices)
    coefficients = trainMethod.solve(ot.Point(trainY))
    testDesign = ot.DesignProxy(testX, basis).computeDesign(indices)
    prediction = testDesign * coefficients
    for t in range(len(testY)):
        quadraticResidual += testW[t] * (testY[t] - prediction[t]) ** 2
        totalTestWeight += testW[t]
wSumW = sum(wNonUniform[s] for s in range(size))
wMeanW = sum(wNonUniform[s] * y[s, 0] for s in range(size)) / wSumW
varianceW = sum(wNonUniform[s] * (y[s, 0] - wMeanW) ** 2 for s in range(size)) / wSumW
ott.assert_almost_equal(
    resultW, quadraticResidual / totalTestWeight / varianceW, 1e-8, 1e-10
)
