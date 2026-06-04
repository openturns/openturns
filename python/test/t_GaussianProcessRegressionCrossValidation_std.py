#! /usr/bin/env python

import openturns as ot
import openturns.experimental as ote
from openturns.testing import assert_almost_equal


ot.TESTPREAMBLE()


g = ot.SymbolicFunction(["x"], ["sin(x)"])
x_train = ot.Sample([[x] for x in [1.0, 3.0, 4.0, 6.0, 7.9, 11.0, 11.5]])
y_train = g(x_train)

basis = ot.ConstantBasisFactory(1).build()
covarianceModel = ot.MaternModel([1.0], 1.5)

fitter_algo = ot.GaussianProcessFitter(x_train, y_train, covarianceModel, basis)
fitter_algo.run()
fitter_result = fitter_algo.getResult()

gpr_algo = ot.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()


# Naive cross-validation
cv_naive_residuals = ot.Sample(x_train.getSize(), 1)
fitted_covariance_model = gpr_result.getCovarianceModel()
fitted_covariance_model.setActiveParameter([])  # no parameter optimization in CV
for i in range(x_train.getSize()):
    # Create the training sample without the i-th point
    x_train_cv = ot.Sample(x_train)
    x_train_cv.erase(i)
    y_train_cv = ot.Sample(y_train)
    y_train_cv.erase(i)
    # Build the GPR model
    fitter_algo_cv = ot.GaussianProcessFitter(
        x_train_cv, y_train_cv, fitted_covariance_model, basis
    )
    fitter_algo_cv.run()
    fitter_result_cv = fitter_algo_cv.getResult()
    gpr_algo_cv = ot.GaussianProcessRegression(fitter_result_cv)
    gpr_algo_cv.run()
    gpr_result_cv = gpr_algo_cv.getResult()
    gpr_predictor_cv = gpr_result_cv.getMetaModel()
    # Evaluate the residual at the i-th point
    cv_naive_residuals[i, 0] = y_train[i, 0] - gpr_predictor_cv(x_train[i])[0]

# OT cross-validation
cv = ote.GaussianProcessRegressionCrossValidation(
    gpr_result, ot.LeaveOneOutSplitter(x_train.getSize())
)
cv_ot_residuals = cv.getResidualSample()

# Check that both implementations give the same result
assert_almost_equal(cv_ot_residuals, cv_naive_residuals, 1.0e-12, 0.0)
