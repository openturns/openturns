#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Realization issued from a SpectralProcess
dimension = 1

# Parameters of the distribution
N = 51
t0 = 0.0
dt = 0.1
myTimeGrid = ot.RegularGrid(t0, dt, N)

# Create a Sample
# parameters of gaussien impose a few risk to get negative values
mySample = ot.Uniform().getSample(N)

# get a realization from distribution
myRealization = ot.TimeSeries(myTimeGrid, mySample)

# Create the lambda parameter
lambdaVector = ot.Point(dimension)
for index in range(dimension):
    lambdaVector[index] = (index + 2.0) * 0.1

myInverseBoxCox = ot.InverseBoxCoxTransform(lambdaVector)

print("myInverseBoxCox=", myInverseBoxCox)

# Get the input and output dimension
print("myInverseBoxCox input dimension = ", myInverseBoxCox.getInputDimension())
print("myInverseBoxCox output dimension = ", myInverseBoxCox.getOutputDimension())

# Evaluation of the InverseBoxCoxTransform on the realization
print("input time series =")
print(myRealization)
print("output time series = ")
print(myInverseBoxCox(myRealization))

print("gradient=", myInverseBoxCox.gradient([0.5]))
print("hessian=", myInverseBoxCox.hessian([0.5]))

# Call the getInverse method
myBoxCox = myInverseBoxCox.getInverse()
print("myBoxCox = ", myBoxCox)

# Get the number of calls
print("number of call(s) : ", myInverseBoxCox.getCallsNumber())

# InverseBoxCoxGradient direct coverage (no prints)
ibc = ot.InverseBoxCoxTransform([0.0, 1e-12, 0.5, 1.0])
_ = repr(ibc.getGradient())
_ = str(ibc.getGradient())
assert "InverseBoxCoxGradient" in repr(ibc.getGradient())
_ = ibc.gradient([0.5, 0.5, 0.5, 0.2])
with ott.assert_raises(Exception):
    ibc.gradient([0.5])
with ott.assert_raises(Exception):
    ibc.gradient([-30.0, 0.0, 0.0, 0.5])
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("invboxcox_tr.xml"))
study.add("t", ibc)
study.save()

# Extended coverage of InverseBoxCoxEvaluation/Gradient/Hessian (no prints)
defaultTransform = ot.InverseBoxCoxTransform()
assert defaultTransform.getInputDimension() == 0
scalarTransform = ot.InverseBoxCoxTransform(0.5)
assert scalarTransform.getInputDimension() == 1
shiftedTransform = ot.InverseBoxCoxTransform([0.5, 1.0], [0.0, 0.0])
assert shiftedTransform.getLambda() == ot.Point([0.5, 1.0])
assert shiftedTransform.getShift() == ot.Point([0.0, 0.0])
assert shiftedTransform.getInputDimension() == 2
assert shiftedTransform.getOutputDimension() == 2
assert "InverseBoxCox" in repr(shiftedTransform)
ott.assert_almost_equal(shiftedTransform([0.5, 0.0]), [1.5625, 1.0])
outSample = shiftedTransform([[0.5, 0.0], [0.0, 1.0]])
ott.assert_almost_equal(outSample[0], [1.5625, 1.0])
ott.assert_almost_equal(outSample[1], [1.0, 2.0])
_ = shiftedTransform.getCallsNumber()
_ = shiftedTransform.getEvaluationCallsNumber()
with ott.assert_raises(Exception):
    shiftedTransform([0.5])
with ott.assert_raises(Exception):
    shiftedTransform(ot.Sample(2, 1))
with ott.assert_raises(Exception):
    shiftedTransform([-10.0, 0.0])
_ = shiftedTransform.getMarginal(0)
with ott.assert_raises(Exception):
    shiftedTransform.getMarginal(5)
_ = shiftedTransform.getParameter()
_ = shiftedTransform.getParameterDescription()
shiftedTransform.setParameter(shiftedTransform.getParameter())
_ = shiftedTransform.isLinear()
_ = shiftedTransform.isLinearlyDependent(0)
shiftedGradient = shiftedTransform.getGradient()
assert "InverseBoxCoxGradient" in repr(shiftedGradient)
assert "InverseBoxCoxGradient" in str(shiftedGradient)
assert shiftedGradient.getInputDimension() == 2
assert shiftedGradient.getOutputDimension() == 2
ott.assert_almost_equal(shiftedGradient.gradient([0.5, 0.2])[0, 0], 0.5)
ott.assert_almost_equal(shiftedGradient.gradient([0.5, 0.2])[0, 1], 1.0)
ott.assert_almost_equal(
    ot.InverseBoxCoxTransform([0.0]).getGradient().gradient([0.5])[0, 0],
    1.6487212707001282,
)
with ott.assert_raises(Exception):
    shiftedGradient.gradient([0.5])
with ott.assert_raises(Exception):
    shiftedGradient.gradient([-30.0, 0.2])
assert shiftedGradient == ot.InverseBoxCoxTransform(
    [0.5, 1.0], [0.0, 0.0]
).getGradient()
assert shiftedGradient != ot.InverseBoxCoxTransform(
    [0.0, 1.0], [0.0, 0.0]
).getGradient()
