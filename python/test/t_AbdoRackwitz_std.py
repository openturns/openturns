#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

levelFunction = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["x1+2*x2-3*x3+4*x4"])
# Add a finite difference gradient to the function, as Abdo Rackwitz algorithm
# needs it
myGradient = ot.NonCenteredFiniteDifferenceGradient(1e-7, levelFunction.getEvaluation())
print("myGradient = ", repr(myGradient))
# Substitute the gradient
levelFunction.setGradient(ot.NonCenteredFiniteDifferenceGradient(myGradient))
startingPoint = [0.0] * 4
algo = ot.AbdoRackwitz(ot.NearestPointProblem(levelFunction, 3.0))
algo.setStartingPoint(startingPoint)
algo.run()
print("result = ", algo.getResult())
ott.assert_almost_equal(algo.getResult().getOptimalValue(), [3.0])
levelFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"]
)
# Add a finite difference gradient to the function, as Abdo Rackwitz algorithm
# needs it
myGradient = ot.NonCenteredFiniteDifferenceGradient(1e-7, levelFunction.getEvaluation())
print("myGradient = ", repr(myGradient))
# Substitute the gradient
levelFunction.setGradient(ot.NonCenteredFiniteDifferenceGradient(myGradient))
startingPoint = [0.0] * 4
algo = ot.AbdoRackwitz(ot.NearestPointProblem(levelFunction, -0.5))
algo.setStartingPoint(startingPoint)
print("myalgorithm=", repr(algo))
algo.run()
print("result = ", algo.getResult())
ott.assert_almost_equal(algo.getResult().getOptimalValue(), [-0.5])
print("evaluation calls number=", levelFunction.getEvaluationCallsNumber())
print("gradient   calls number=", levelFunction.getGradientCallsNumber())
print("hessian    calls number=", levelFunction.getHessianCallsNumber())

# The maximum absolute/relative error criteria are honored, see issue #1841
levelFunction = ot.SymbolicFunction(["x1", "x2"], ["x1*x2+x2*x2-10"])
levelFunction.setGradient(
    ot.NonCenteredFiniteDifferenceGradient(1e-7, levelFunction.getEvaluation())
)
algo = ot.AbdoRackwitz(ot.NearestPointProblem(levelFunction, 0.0))
algo.setStartingPoint([1.0, 1.0])
algo.setMaximumIterationNumber(100)
algo.setMaximumAbsoluteError(1e-2)
algo.setMaximumRelativeError(1e-2)
algo.setMaximumResidualError(1e-12)
algo.setMaximumConstraintError(1e-12)
algo.run()
result = algo.getResult()
assert result.getStatus() == ot.OptimizationResult.SUCCESS
assert "absolute error" in result.getStatusMessage(), result.getStatusMessage()
# A run hitting the maximum iteration number is reported as a failure
algo = ot.AbdoRackwitz(ot.NearestPointProblem(levelFunction, 0.0))
algo.setStartingPoint([1.0, 1.0])
algo.setMaximumIterationNumber(1)
algo.setCheckStatus(False)
algo.run()
result = algo.getResult()
assert result.getStatus() == ot.OptimizationResult.FAILURE
assert "maximum iteration number" in result.getStatusMessage(), result.getStatusMessage()
