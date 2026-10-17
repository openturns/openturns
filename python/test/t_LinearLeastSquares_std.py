#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

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

# The linear term follows the LinearFunction convention:
# outputDimension rows, inputDimension columns
center = algo.getCenter()
constant = algo.getConstant()
linear = algo.getLinear()
assert center.getDimension() == 2
assert constant.getDimension() == 3
assert linear.getNbRows() == 3
assert linear.getNbColumns() == 2
ott.assert_almost_equal(center, [0.0555556, 0.0555556])
ott.assert_almost_equal(constant, [0.0266348, 0.819564, 1.03714])
expectedLinear = ot.Matrix(3, 2)
expectedLinear[0, 0] = 0.0628755
expectedLinear[1, 0] = -0.082405
expectedLinear[2, 0] = 0.951099
expectedLinear[0, 1] = 0.0628755
expectedLinear[1, 1] = -0.082405
expectedLinear[2, 1] = -0.972737
ott.assert_almost_equal(linear, expectedLinear)

# The metamodel matches the LinearFunction definition:
# the evaluation stores the linear term as is,
# the gradient stores its transpose
responseSurface = algo.getResult().getMetaModel()
ott.assert_almost_equal(
    responseSurface.getEvaluation().getImplementation().getLinear(), linear
)
ott.assert_almost_equal(responseSurface.gradient(center), linear.transpose())
ott.assert_almost_equal(
    responseSurface.hessian(center), ot.SymmetricTensor(2, 3)
)
inPoint = [0.1] * 2
ott.assert_almost_equal(myFunc(inPoint), [0.00998334, 0.980067, 0.995321])
ott.assert_almost_equal(responseSurface(inPoint), [0.0322237, 0.812239, 1.03617])

# Data accessors
ott.assert_almost_equal(algo.getInputSample(), data)
ott.assert_almost_equal(algo.getDataIn(), data)
ott.assert_almost_equal(algo.getOutputSample(), dataOut)
ott.assert_almost_equal(algo.getDataOut(), dataOut)
with ott.assert_raises(Exception):
    ot.LinearLeastSquares(data, ot.Sample(5, 3))
with ott.assert_raises(Exception):
    algo.setDataOut(ot.Sample(5, 3))
