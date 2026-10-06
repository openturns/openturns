#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Realization issued from a SpectralProcess
dimension = 1

# Parameters of the distribution
N = 101
t0 = 0.0
dt = 0.1
myTimeGrid = ot.RegularGrid(t0, dt, N)

# Create a Sample
# parameters of gaussien impose a few risk to get negative values
mySample = ot.Normal(10, 3).getSample(N)

# get a realization from distribution
myRealization = ot.TimeSeries(myTimeGrid, mySample)

# Create the lambda parameter
lambdaVector = ot.Point(dimension)
for index in range(dimension):
    lambdaVector[index] = (index + 2.0) * 0.1

myBoxCox = ot.BoxCoxTransform(lambdaVector)

print("myBoxCox=", myBoxCox)

# Get the input and output dimension
print("myBoxCox input dimension = ", myBoxCox.getInputDimension())
print("myBoxCox output dimension = ", myBoxCox.getOutputDimension())

# Evaluation of the BoxCoxTransform on the realization
print("input time series =")
print(myRealization)
print("output time series = ")
print(myBoxCox(myRealization))

print("gradient=", myBoxCox.gradient([0.5]))
print("hessian=", myBoxCox.hessian([0.5]))

# Call the getInverse method
myInverseBoxCox = myBoxCox.getInverse()
print("myInverseBoxCox = ", myInverseBoxCox)

# Get the number of calls
print("number of call(s) : ", myBoxCox.getCallsNumber())

# BoxCoxGradient direct coverage (no prints)
bc = ot.BoxCoxTransform([0.0, 1e-10, 0.5, 1.0])
_ = repr(bc.getGradient())
_ = str(bc.getGradient())
assert "BoxCoxGradient" in repr(bc.getGradient())
g = bc.gradient([1.0, 1.0, 4.0, 2.0])
ott.assert_almost_equal(g[0, 0], 1.0, 1e-8, 1e-8)
ott.assert_almost_equal(g[0, 3], 1.0, 1e-8, 1e-8)
with ott.assert_raises(Exception):
    bc.gradient([1.0])
with ott.assert_raises(Exception):
    bc.gradient([-5.0, 1.0, 1.0, 1.0])
_ = bc.getInverse().gradient([0.5, 0.1, 0.2, 0.3])
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("boxcox_tr.xml"))
study.add("t", bc)
study.save()

# Extended coverage of BoxCoxEvaluation/Gradient/Hessian (no prints)
defaultTransform = ot.BoxCoxTransform()
assert defaultTransform.getInputDimension() == 0
scalarTransform = ot.BoxCoxTransform(0.5)
assert scalarTransform.getInputDimension() == 1
shiftedTransform = ot.BoxCoxTransform([0.5, 1.0], [1.0, 2.0])
assert shiftedTransform.getLambda() == ot.Point([0.5, 1.0])
assert shiftedTransform.getShift() == ot.Point([1.0, 2.0])
assert shiftedTransform.getInputDimension() == 2
assert shiftedTransform.getOutputDimension() == 2
assert "BoxCox" in repr(shiftedTransform)
ott.assert_almost_equal(shiftedTransform([1.0, 1.0]), [0.8284271247461903, 2.0])
outSample = shiftedTransform([[1.0, 1.0], [0.0, 0.0]])
ott.assert_almost_equal(outSample[0], [0.8284271247461903, 2.0])
ott.assert_almost_equal(outSample[1], [0.0, 1.0])
_ = shiftedTransform.getCallsNumber()
_ = shiftedTransform.getEvaluationCallsNumber()
with ott.assert_raises(Exception):
    shiftedTransform([1.0])
with ott.assert_raises(Exception):
    shiftedTransform(ot.Sample(2, 1))
with ott.assert_raises(Exception):
    shiftedTransform([-1.0, -2.0])
_ = shiftedTransform.getMarginal(0)
with ott.assert_raises(Exception):
    shiftedTransform.getMarginal(5)
_ = shiftedTransform.getParameter()
_ = shiftedTransform.getParameterDescription()
shiftedTransform.setParameter(shiftedTransform.getParameter())
_ = shiftedTransform.isLinear()
_ = shiftedTransform.isLinearlyDependent(0)
shiftedGradient = shiftedTransform.getGradient()
assert "BoxCoxGradient" in repr(shiftedGradient)
assert "BoxCoxGradient" in str(shiftedGradient)
assert shiftedGradient.getInputDimension() == 2
assert shiftedGradient.getOutputDimension() == 2
ott.assert_almost_equal(shiftedGradient.gradient([1.0, 1.0])[0, 0], 0.7071067811865476)
ott.assert_almost_equal(shiftedGradient.gradient([1.0, 1.0])[0, 1], 1.0)
ott.assert_almost_equal(
    ot.BoxCoxTransform([0.0]).getGradient().gradient([1.0])[0, 0], 1.0
)
ott.assert_almost_equal(
    ot.BoxCoxTransform([1e-12]).getGradient().gradient([1.0])[0, 0], 1.0
)
with ott.assert_raises(Exception):
    shiftedGradient.gradient([1.0])
with ott.assert_raises(Exception):
    shiftedGradient.gradient([-1.0, -2.0])
assert shiftedGradient == ot.BoxCoxTransform([0.5, 1.0], [1.0, 2.0]).getGradient()
assert shiftedGradient != ot.BoxCoxTransform([0.1, 1.0], [1.0, 2.0]).getGradient()
_ = shiftedGradient.getCallsNumber()
_ = shiftedGradient.getMarginal(0)
with ott.assert_raises(Exception):
    shiftedGradient.getMarginal(5)
shiftedHessian = shiftedTransform.getHessian()
assert "BoxCoxHessian" in repr(shiftedHessian)
assert "BoxCoxHessian" in str(shiftedHessian)
assert shiftedHessian.getInputDimension() == 2
assert shiftedHessian.getOutputDimension() == 2
ott.assert_almost_equal(shiftedHessian.hessian([1.0, 1.0])[0, 0, 0], -0.1767766952966369)
ott.assert_almost_equal(shiftedHessian.hessian([1.0, 1.0])[0, 0, 1], 0.0)
ott.assert_almost_equal(
    ot.BoxCoxTransform([0.0]).getHessian().hessian([1.0])[0, 0, 0], -1.0
)
with ott.assert_raises(Exception):
    shiftedHessian.hessian([1.0])
with ott.assert_raises(Exception):
    shiftedHessian.hessian([-1.0, -2.0])
assert shiftedHessian == ot.BoxCoxTransform([0.5, 1.0], [1.0, 2.0]).getHessian()
assert shiftedHessian != ot.BoxCoxTransform([0.1, 1.0], [1.0, 2.0]).getHessian()
_ = shiftedHessian.getCallsNumber()
_ = shiftedHessian.getMarginal(0)
with ott.assert_raises(Exception):
    shiftedHessian.getMarginal(5)
