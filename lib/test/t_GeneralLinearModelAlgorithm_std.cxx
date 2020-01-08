//                                               -*- C++ -*-
/**
 *  @brief The test file of GeneralLinearModelAlgorithm class
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
      X[i][0] = 3.0 + (8.0 * i) / sampleSize;
    Sample Y = model(X);

    // Add a small noise to data
    Y += GaussianProcess(AbsoluteExponential(Point(1, 0.1), Point(1, 0.2)), Mesh(X)).getRealization().getValues();

    Basis basis = LinearBasisFactory(inputDimension).build();
    // Case of a misspecified covariance model
    DiracCovarianceModel covarianceModel(inputDimension);
    fullprint << "===================================================\n" << std::endl;
    GeneralLinearModelAlgorithm algo(X, Y, covarianceModel, basis);
    algo.run();

    GeneralLinearModelResult result = algo.getResult();
    fullprint << "\ncovariance (dirac, optimized)=" << result.getCovarianceModel() << std::endl;
    fullprint << "trend (dirac, optimized)=" << result.getTrendCoefficients() << std::endl;
    fullprint << "===================================================\n" << std::endl;
    // Now without estimating covariance parameters
    basis = LinearBasisFactory(inputDimension).build();
    covarianceModel = DiracCovarianceModel(inputDimension);
    algo = GeneralLinearModelAlgorithm(X, Y, covarianceModel, basis, true, true);
    algo.setOptimizeParameters(false);
    algo.run();
    result = algo.getResult();
    fullprint << "\ncovariance (dirac, not optimized)=" << result.getCovarianceModel() << std::endl;
    fullprint << "trend (dirac, not optimized)=" << result.getTrendCoefficients() << std::endl;
    fullprint << "===================================================\n" << std::endl;

    // Case of a well specified covariance model
    // Test the optimization when the amplitude is deduced analytically from the scale
    {
      AbsoluteExponential covarianceModel2(inputDimension);
      algo = GeneralLinearModelAlgorithm(X, Y, covarianceModel2, basis);
      algo.run();
      result = algo.getResult();
      fullprint << "\ncovariance (reduced, unbiased)=" << result.getCovarianceModel() << std::endl;
      fullprint << "trend (reduced, unbiased)=" << result.getTrendCoefficients() << std::endl;
      fullprint << "===================================================\n" << std::endl;
      ResourceMap::SetAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance", false);
      algo = GeneralLinearModelAlgorithm(X, Y, covarianceModel2, basis);
      algo.run();
      result = algo.getResult();
      fullprint << "\ncovariance (reduced, biased)=" << result.getCovarianceModel() << std::endl;
      fullprint << "trend (reduced, biased)=" << result.getTrendCoefficients() << std::endl;
      fullprint << "===================================================\n" << std::endl;
      ResourceMap::SetAsBool("GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", false);
      algo = GeneralLinearModelAlgorithm(X, Y, covarianceModel2, basis);
      algo.run();
      result = algo.getResult();
      fullprint << "\ncovariance (full optim)=" << result.getCovarianceModel() << std::endl;
      fullprint << "trend (full optim)=" << result.getTrendCoefficients() << std::endl;
      fullprint << "===================================================\n" << std::endl;
    }
    std::cout << "Test Ok" << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
