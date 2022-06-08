#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(2)

m = 10
x = [[0.5 + i] for i in range(m)]

inVars = ["a", "b", "c", "x"]
formulas = ["a + b * exp(c * x)", "(a * x^2 + b) / (c + x^2)"]
g = ot.SymbolicFunction(inVars, formulas)
trueParameter = [2.8, 1.2, 0.5]
params = [0, 1, 2]
model = ot.ParametricFunction(g, params, trueParameter)
y = model(x)
y += ot.Normal([0.0] * 2, [0.05] * 2, ot.IdentityMatrix(2)).getSample(m)
candidate = [1.0] * 3
bootstrapSizes = [0, 100]
for bootstrapSize in bootstrapSizes:
    algo = ot.NonLinearLeastSquaresCalibration(model, x, y, candidate)
    algo.setBootstrapSize(bootstrapSize)
    algo.run()
    result = algo.getResult()
    # To avoid discrepance between the platforms with or without CMinpack
    print("result (Auto)=", result.getParameterMAP())
    ott.assert_almost_equal(
        result.getObservationsError().getMean(), [0.0051, -0.0028], 1e-1, 1e-3
    )
    algo.setOptimizationAlgorithm(
        ot.MultiStart(
            ot.TNC(),
            ot.LowDiscrepancyExperiment(
                ot.SobolSequence(),
                ot.Normal(
                    candidate, ot.CovarianceMatrix(ot.Point(candidate).getDimension())
                ),
                ot.ResourceMap.GetAsUnsignedInteger(
                    "NonLinearLeastSquaresCalibration-MultiStartSize"
                ),
            ).generate(),
        )
    )
    algo.run()
    result = algo.getResult()
    # To avoid discrepance between the platforms with or without CMinpack
    print("result  (TNC)=", result.getParameterMAP())
    ott.assert_almost_equal(
        result.getObservationsError().getMean(), [0.0051, -0.0028], 1e-1, 1e-3
    )
    # Draw result
    graph = result.drawParameterDistributions()
    graph = result.drawResiduals()
    graph = result.drawObservationsVsInputs()
    graph = result.drawObservationsVsPredictions()
