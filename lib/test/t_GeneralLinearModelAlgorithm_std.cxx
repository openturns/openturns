//                                               -*- C++ -*-
/**
 *  @brief The test file of GeneralLinearModelAlgorithm class
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
  setRandomGenerator();

  try
  {
    // Set Numerical precision to 4
    PlatformInfo::SetNumericalPrecision(4);
    UnsignedInteger sampleSize = 40;
    UnsignedInteger inputDimension = 1;

    // Create the function to estimate
    SymbolicFunction model("x0", "x0");

    Sample X(sampleSize, inputDimension);
    for (UnsignedInteger i = 0; i < sampleSize; ++ i)
      X(i, 0) = 3.0 + (8.0 * i) / sampleSize;
    Sample Y = model(X);

    // Add a small noise to data
    Y += GaussianProcess(AbsoluteExponential(Point(1, 0.1), Point(1, 0.2)), Mesh(X)).getRealization().getValues();

    Basis basis = LinearBasisFactory(inputDimension).build();
    // Case of a misspecified covariance model
    DiracCovarianceModel covarianceModel(inputDimension);
    GeneralLinearModelAlgorithm algo(X, Y, covarianceModel, basis);
    algo.run();

    GeneralLinearModelResult result = algo.getResult();
    Point ref = {0.1957};
    assert_almost_equal(result.getCovarianceModel().getParameter(), ref, 1e-4, 1e-4);
    ref = {-0.1109, 1.015};
    assert_almost_equal(result.getTrendCoefficients()[0], ref, 1e-4, 1e-4);
    // Now without estimating covariance parameters
    basis = LinearBasisFactory(inputDimension).build();
    covarianceModel = DiracCovarianceModel(inputDimension);
    algo = GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis, true);
    algo.setOptimizeParameters(false);
    algo.run();
    result = algo.getResult();
    ref = {1.0};
    assert_almost_equal(result.getCovarianceModel().getParameter(), ref, 1e-4, 1e-4);
    ref = {-0.1109, 1.015};
    assert_almost_equal(result.getTrendCoefficients()[0], ref, 1e-4, 1e-4);
    // Case of a well specified covariance model
    // Test the optimization when the amplitude is deduced analytically from the scale
    {
      AbsoluteExponential covarianceModel2(inputDimension);
      algo = GeneralLinearModelAlgorithm(X, Y, covarianceModel2, basis);
      algo.run();
      result = algo.getResult();
      ref = {0.1328, 0.1956};
      assert_almost_equal(result.getCovarianceModel().getParameter(), ref, 1e-4, 1e-4);
      ref = {-0.1034, 1.014};
      assert_almost_equal(result.getTrendCoefficients()[0], ref, 1e-4, 1e-4);
      ResourceMap::SetAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance", false);
      algo = GeneralLinearModelAlgorithm(X, Y, covarianceModel2, basis);
      algo.run();
      result = algo.getResult();
      ref = {0.1328, 0.1907};
      assert_almost_equal(result.getCovarianceModel().getParameter(), ref, 1e-4, 1e-4);
      ref = {-0.1034, 1.014};
      assert_almost_equal(result.getTrendCoefficients()[0], ref, 1e-4, 1e-4);
      ResourceMap::SetAsBool("GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", false);
      algo = GeneralLinearModelAlgorithm(X, Y, covarianceModel2, basis);
      algo.run();
      result = algo.getResult();
      ref = {0.01, 0.1908};
      assert_almost_equal(result.getCovarianceModel().getParameter(), ref, 1e-2, 1e-2);
      ref = {-0.111, 1.015};
      assert_almost_equal(result.getTrendCoefficients()[0], ref, 1e-4, 1e-4);
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
