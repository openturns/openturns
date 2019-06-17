#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import assert_almost_equal

ot.TESTPREAMBLE()

try:

    # Set precision
    ot.PlatformInfo.SetNumericalPrecision(3)

    print("================")
    print("Test using NLOpt")
    print("================")
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
    assert_almost_equal(residual.computeCenteredMoment(2),
                        [1.06e-05], 1e-5, 1e-5)
    assert_almost_equal(conditionalCovariance.getParameter(), [
                        0.702138], 5e-3, 1e-3)
    print("Test Ok")

except:
    import sys
    print("t_GeneralLinearModelAlgorithm_nlopt.py",
          sys.exc_info()[0], sys.exc_info()[1])
