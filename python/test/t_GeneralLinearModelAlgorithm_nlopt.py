#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Set precision
ot.PlatformInfo.SetNumericalPrecision(3)

# Calibration of default optimizer
ot.ResourceMap.SetAsScalar(
    'GeneralLinearModelAlgorithm-DefaultOptimizationLowerBound', 1.0e-5)
ot.ResourceMap.SetAsScalar(
    'GeneralLinearModelAlgorithm-DefaultOptimizationUpperBound', 100)
# Data & estimation
inputDimension = 1
X = ot.Normal().getSample(100)
X = X.sortAccordingToAComponent(0)
covarianceModel = ot.SquaredExponential([1.0], [1.0])
model = ot.SymbolicFunction(["x"], ["x - 0.6 * cos(x/3)"])
Y = model(X)
basis = ot.QuadraticBasisFactory(inputDimension).build()
algo = ot.GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis, True)
algo.setOptimizationAlgorithm(ot.NLopt('LN_NELDERMEAD'))
algo.run()

# perform an evaluation
result = algo.getResult()
metaModel = result.getMetaModel()
conditionalCovariance = result.getCovarianceModel()
residual = metaModel(X) - Y
ott.assert_almost_equal(residual.computeCentralMoment(2),
                        [1.06e-05], 1e-5, 1e-5)
ott.assert_almost_equal(conditionalCovariance.getParameter(),
                        [0.619144, 0.000937], 5e-3, 1e-3)
likelihood = algo.getObjectiveFunction()
assert likelihood.getInputDimension() == 1, "likelihood dim"
print("ok")
