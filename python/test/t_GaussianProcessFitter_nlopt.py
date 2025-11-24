#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Data & estimation
inputDimension = 1
X = ot.Normal().getSample(100)
X = X.sortAccordingToAComponent(0)
covarianceModel = ot.SquaredExponential([1.0], [1.0])
model = ot.SymbolicFunction(["x"], ["x - 0.6 * cos(x/3)"])
Y = model(X)
basis = ot.QuadraticBasisFactory(inputDimension).build()
algo = ot.GaussianProcessFitter(X, Y, covarianceModel, basis)
algo.setKeepCholeskyFactor(False)
algo.setOptimizationAlgorithm(ot.NLopt("LN_NELDERMEAD"))
algo.run()

# perform an evaluation
result = algo.getResult()
metaModel = result.getMetaModel()
conditionalCovariance = result.getCovarianceModel()
residual = metaModel(X) - Y
ott.assert_almost_equal(residual.computeCentralMoment(2), [1.06e-05], 1e-5, 1e-5)
print(conditionalCovariance.getParameter())
ott.assert_almost_equal(
    conditionalCovariance.getParameter(), [0.620868, 0.000941836], 5e-3, 1e-3
)
likelihood = algo.getReducedLogLikelihoodFunction()
assert likelihood.getInputDimension() == 1, "likelihood dim"
