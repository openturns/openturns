//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussianNonLinearCalibration for standard methods
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
    PlatformInfo::SetNumericalPrecision(3);

    // A use case without observed inputs
    UnsignedInteger m = 100;
    Sample x(m, 0);

    const Description inVars = {"a", "b", "c"};
    // Derived from y = a + b * x + c * x^2 at x=[-1.0, -0.6, -0.2, 0.2, 0.6, 1.0]
    Description formulas(1, "a +  -1.0  * b +  1.0  * c");
    formulas.add("a +  -0.6  * b +  0.36  * c");
    formulas.add("a +  -0.2  * b +  0.04  * c");
    formulas.add("a +  0.2  * b +  0.04  * c");
    formulas.add("a +  0.6  * b +  0.36  * c");
    formulas.add("a +  1.0  * b +  1.0  * c");
    SymbolicFunction g(inVars, formulas);
    UnsignedInteger inputDimension = g.getInputDimension();
    UnsignedInteger outputDimension = g.getOutputDimension();
    const Point trueParameter = {2.8, 1.2, 0.5};
    Indices params(inputDimension);
    params.fill();
    ParametricFunction model(g, params, trueParameter);
    Sample y = model(x);
    y += Normal(Point(outputDimension), Point(outputDimension, 0.05), IdentityMatrix(outputDimension)).getSample(y.getSize());
    Point candidate(inputDimension, 1.0);
    CovarianceMatrix priorCovariance(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      priorCovariance(i, i) = 3.0 + (1.0 + i) * (1.0 + i);
      for (UnsignedInteger j = 0; j < i; ++j)
        priorCovariance(i, j) = 1.0 / (1.0 + i + j);
    }
    CovarianceMatrix errorCovariance(outputDimension);
    for (UnsignedInteger i = 0; i < 1; ++i)
    {
      errorCovariance(i, i) = 0.1 * (2.0 + (1.0 + i) * (1.0 + i));
      for (UnsignedInteger j = 0; j < i; ++j)
        errorCovariance(i, j) = 1.0 / (1.0 + i + j);
    }
    CovarianceMatrix globalErrorCovariance(outputDimension * m);
    for (UnsignedInteger i = 0; i < outputDimension * m; ++i)
    {
      globalErrorCovariance(i, i) = 0.1 * (2.0 + (1.0 + i) * (1.0 + i));
      for (UnsignedInteger j = 0; j < i; ++j)
        globalErrorCovariance(i, j) = 0.1 / (1.0 + i + j);
    }
    const Indices bootstrapSizes = {0, 30};
    for (UnsignedInteger n = 0; n < bootstrapSizes.getSize(); ++n)
    {
      // With default optim
      fullprint << "Bootstrap size =" << bootstrapSizes[n] << std::endl;
      fullprint << "1. Default optim" << std::endl;
      GaussianNonLinearCalibration algo(model, x, y, candidate, priorCovariance, errorCovariance);
      algo.setBootstrapSize(bootstrapSizes[n]);
      algo.run();
      // To avoid discrepance between the platforms with or without CMinpack
      Point parameterMAP(algo.getResult().getParameterMAP());
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 10e-1);
      // With TNC
      fullprint << "2. TNC optim" << std::endl;
      algo.setOptimizationAlgorithm(MultiStart(TNC(), LowDiscrepancyExperiment(SobolSequence(), Normal(candidate, CovarianceMatrix(candidate.getDimension())), ResourceMap::GetAsUnsignedInteger("GaussianNonLinearCalibration-MultiStartSize")).generate()));
      algo.run();
      parameterMAP = algo.getResult().getParameterMAP();
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 10e-1);
      // With globalErrorCovariance
      fullprint << "3. globalErrorCovariance" << std::endl;
      algo = GaussianNonLinearCalibration(model, x, y, candidate, priorCovariance, globalErrorCovariance);
      algo.setBootstrapSize(bootstrapSizes[n]);
      algo.run();
      parameterMAP = algo.getResult().getParameterMAP();
//       fullprint << "MAP =" << parameterMAP << std::endl;
      const Point ref = {2.61, 1.2, 0.731};
      assert_almost_equal(parameterMAP, ref, 50e-1);
    } // n
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
