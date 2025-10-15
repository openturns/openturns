#! /usr/bin/env python

import openturns as ot

myFunc = ot.SymbolicFunction(
    ["x1", "x2"], ["x1*sin(x2)", "cos(x1+x2)", "(x2+1)*exp(x1-2*x2)"]
)
data = ot.Sample(9, myFunc.getInputDimension())
point = ot.Point(myFunc.getInputDimension())
point[0] = 0.5
point[1] = 0.5
data[0] = point
point[0] = -0.5
point[1] = -0.5
data[1] = point
point[0] = -0.5
point[1] = 0.5
data[2] = point
point[0] = 0.5
point[1] = -0.5
data[3] = point
point[0] = 0.5
point[1] = 0.5
data[4] = point
point[0] = -0.25
point[1] = -0.25
data[5] = point
point[0] = -0.25
point[1] = 0.25
data[6] = point
point[0] = 0.25
point[1] = -0.25
data[7] = point
point[0] = 0.25
point[1] = 0.25
data[8] = point
dataOut = myFunc(data)
algo = ot.LinearLeastSquares(data, dataOut)
algo.run()
responseSurface = algo.getResult().getMetaModel()
print("algo=", repr(algo))
print("responseSurface=", repr(responseSurface))
inPoint = [0.1] * 2
print("myFunc(", repr(inPoint), ")=", repr(myFunc(inPoint)))
print("responseSurface(", repr(inPoint), ")=", repr(responseSurface(inPoint)))
