#! /usr/bin/env python

import openturns as ot
import openturns.experimental as ote
from openturns.testing import assert_almost_equal

ot.ResourceMap.SetAsString("GaussianProcessFitter-LinearAlgebra", "HMAT")
ot.ResourceMap.SetAsScalar("HMatrix-RegularizationEpsilon", 1e-16)

ot.TESTPREAMBLE()


def fit_gpr_model(x_train, y_train, covariance_model, basis, method):
    """
    Fit a Gaussian Process Regression model on the given training sample.
    Parameters
    ----------
    x_train : Sample
        The training input sample.
    y_train : Sample
        The training output sample.
    covariance_model : CovarianceModel
        The covariance model to use for fitting the GPR model.
    basis : Basis
        The basis to use for fitting the GPR model.
    method : int
        The linear algebra method to use for the GPR fitting (HMAT or CHOL).
    Returns
    -------
    gpr_result : GaussianProcessRegressionResult
        The result of fitting the GPR model on the training sample.
    """
    fitter_algo = ot.GaussianProcessFitter(x_train, y_train, covariance_model, basis)
    fitter_algo.setMethod(method)
    fitter_algo.run()
    fitter_result = fitter_algo.getResult()
    gpr_algo = ot.GaussianProcessRegression(fitter_result)
    gpr_algo.run()
    gpr_result = gpr_algo.getResult()
    return gpr_result


def compute_cv_naive_residuals(x_train, y_train, fitted_covariance_model, basis, method):
    """
    Compute cross-validation residuals using the naive approach.
    At each iteration, the GPR model is trained on the training sample without the i-th point, and the residual is evaluated at the i-th point.
    This is used to check that the OT implementation of cross-validation gives the same result as the naive approach.
    Parameters
    ----------
    x_train : Sample
        The training input sample.
    y_train : Sample
        The training output sample.
    fitted_covariance_model : CovarianceModel
        The covariance model fitted on the whole training sample, with fixed parameters (no optimization in CV
    basis : Basis
        The basis used for the GPR model.
    method : int
        The linear algebra method to use for the GPR fitting (HMAT or CHOL).
    Returns
    -------
    cv_naive_residuals : Sample
        The cross-validation residuals computed using the naive approach.
    """
    # Naive cross-validation
    # Fix the covariance model parameters to the values fitted on the whole training sample (no optimization in CV)
    fitted_covariance_model.setActiveParameter([])
    cv_naive_residuals = ot.Sample(x_train.getSize(), 1)
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
        fitter_algo_cv.setMethod(method)
        fitter_algo_cv.run()
        fitter_result_cv = fitter_algo_cv.getResult()
        gpr_algo_cv = ot.GaussianProcessRegression(fitter_result_cv)
        gpr_algo_cv.run()
        gpr_result_cv = gpr_algo_cv.getResult()
        gpr_predictor_cv = gpr_result_cv.getMetaModel()
        # Evaluate the residual at the i-th point
        cv_naive_residuals[i, 0] = y_train[i, 0] - gpr_predictor_cv(x_train[i])[0]
    return cv_naive_residuals


if __name__ == "__main__":

    # linear algebra method to use for the GPR fitting (HMAT or CHOL)
    method = ot.GaussianProcessFitterResult.HMAT
    g = ot.SymbolicFunction(["x"], ["sin(x)"])
    x_train = ot.Sample([[x] for x in [1.0, 3.0, 4.0, 6.0, 7.9, 11.0, 11.5]])
    y_train = g(x_train)

    basis = ot.ConstantBasisFactory(1).build()
    covarianceModel = ot.MaternModel([1.0], 1.5)

    gpr_result = fit_gpr_model(x_train, y_train, covarianceModel, basis, method)
    # Compute the naive cross-validation residuals
    cv_naive_residuals = compute_cv_naive_residuals(x_train, y_train, gpr_result.getCovarianceModel(), basis, method)

    # OT cross-validation
    cv = ote.GaussianProcessRegressionCrossValidation(
        gpr_result, ot.LeaveOneOutSplitter(x_train.getSize())
    )
    cv_ot_residuals = cv.getResidualSample()

    # Check that both implementations give the same result
    assert_almost_equal(cv_ot_residuals, cv_naive_residuals, 1.0e-12, 0.0)
