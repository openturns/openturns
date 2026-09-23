#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Create an instance
myFunc = ot.PointToPointEvaluation(
    ot.SymbolicFunction("x", "sin(x)"), ot.SymbolicFunction("x", "cos(x)")
)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Evaluation on a point
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
print("called ", myFunc.getCallsNumber(), " times")
# Evaluation on a sample
sample = [[1.0] * myFunc.getInputDimension()] * 10
print("sample=", sample)
print("myFunc(sample)=", myFunc(sample))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")

# Construction based on a PointToFieldFunction followed by a FieldToPointFunction
# Create a KarhunenLoeveResult
mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
# Create a PointToFieldFunction
lifting = ot.KarhunenLoeveLifting(result)
# Create a FieldToPointFunction
projection = ot.KarhunenLoeveProjection(result)
# Create an instance
myFunc = ot.PointToPointEvaluation(projection, lifting)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Evaluation on a point
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")

# PointToPointEvaluation extra coverage (no prints)
evFF = ot.PointToPointEvaluation(ot.SymbolicFunction("x", "sin(x)"), ot.SymbolicFunction("x", "cos(x)"))
_ = repr(evFF)
_ = str(evFF)
assert evFF.getInputDimension() == 1
_ = evFF.getParameter()
_ = evFF.getParameterDescription()
evFF.setParameter(evFF.getParameter())
evFF.setParameterDescription(evFF.getParameterDescription())
with ott.assert_raises(Exception):
    evFF.setParameter([0.0, 1.0])
with ott.assert_raises(Exception):
    evFF.setParameterDescription(["a", "b"])
_ = evFF.getMarginal(0)
_ = evFF.getMarginal([0])
_ = evFF.getLeftFunction()
_ = evFF.getRightFunction()
_ = evFF.getPointToFieldFunction()
_ = evFF.getFieldToPointFunction()
ott.assert_almost_equal(evFF(ot.Sample([[0.5], [1.0]])), ot.Function(evFF)(ot.Sample([[0.5], [1.0]])), 1e-14, 1e-14)
with ott.assert_raises(Exception):
    evFF([1.0, 2.0])
with ott.assert_raises(Exception):
    evFF.getMarginal(5)
with ott.assert_raises(Exception):
    ot.PointToPointEvaluation(ot.SymbolicFunction(["x", "y"], ["x+y"]), ot.SymbolicFunction("x", "cos(x)"))
# field/function composition branch: sample eval on field-based instance
_ = myFunc(ot.Sample([[1.0] * myFunc.getInputDimension()] * 3))
with ott.assert_raises(Exception):
    myFunc.getMarginal(0)
with ott.assert_raises(Exception):
    myFunc.getMarginal(myFunc.getOutputDimension() + 1)
with ott.assert_raises(Exception):
    myFunc.getParameter()
with ott.assert_raises(Exception):
    myFunc.setParameter([0.0])
with ott.assert_raises(Exception):
    myFunc.getParameterDescription()
with ott.assert_raises(Exception):
    myFunc.setParameterDescription(["a"])
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("ptp_eval.xml"))
study.add("f", ot.Function(evFF))
study.save()
