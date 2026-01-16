//                                               -*- C++ -*-
/**
 *  @brief The test file of GaussianProcessRegressionCrossValidation class
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Define the symbolic function
    SymbolicFunction g(Description({"x"}), Description({"sin(x)"}));
    
    // Create training sample
    Sample x_train(7, 1);
    x_train[0] = Point({1.0});
    x_train[1] = Point({3.0});
    x_train[2] = Point({4.0});
    x_train[3] = Point({6.0});
    x_train[4] = Point({7.9});
    x_train[5] = Point({11.0});
    x_train[6] = Point({11.5});
    Sample y_train = g(x_train);
    
    // Setup basis and covariance model
    Basis basis = ConstantBasisFactory(1).build();
    MaternModel covarianceModel(Point(1, 1.0), 1.5);
    
    // Fit the model
    GaussianProcessFitter fitter_algo(x_train, y_train, covarianceModel, basis);
    fitter_algo.run();
    GaussianProcessFitterResult fitter_result = fitter_algo.getResult();
    
    GaussianProcessRegression gpr_algo(fitter_result);
    gpr_algo.run();
    GaussianProcessRegressionResult gpr_result = gpr_algo.getResult();
    
    // Naive cross-validation
    Sample cv_naive_residuals(x_train.getSize(), 1);
    CovarianceModel fitted_covariance_model = gpr_result.getCovarianceModel();
    fitted_covariance_model.setActiveParameter(Indices()); // no parameter optimization in CV
    
    for (UnsignedInteger i = 0; i < x_train.getSize(); ++i)
    {
        Sample x_train_cv = Sample(x_train);
        x_train_cv.erase(i);
        Sample y_train_cv = Sample(y_train);
        y_train_cv.erase(i);
        
        GaussianProcessFitter fitter_algo_cv(x_train_cv, y_train_cv, fitted_covariance_model, basis);
        fitter_algo_cv.run();
        GaussianProcessFitterResult fitter_result_cv = fitter_algo_cv.getResult();
        
        GaussianProcessRegression gpr_algo_cv(fitter_result_cv);
        gpr_algo_cv.run();
        GaussianProcessRegressionResult gpr_result_cv = gpr_algo_cv.getResult();
        
        Function gpr_predictor_cv = gpr_result_cv.getMetaModel();
        cv_naive_residuals[i][0] = y_train[i][0] - gpr_predictor_cv(x_train[i])[0];
    }
    
    // OT cross-validation
    GaussianProcessRegressionCrossValidation cv(gpr_result, LeaveOneOutSplitter(x_train.getSize()));
    Sample cv_ot_residuals = cv.getResidualSample();
    
    // Check that both implementations give the same result
    assert_almost_equal(cv_ot_residuals, cv_naive_residuals, 1.0e-12, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
