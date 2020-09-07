//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussianNonLinearCalibration for standard methods
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

  try
  {
    PlatformInfo::SetNumericalPrecision(3);
    UnsignedInteger m = 200;
    Sample x(m, 1);
    for (UnsignedInteger i = 0; i < m; ++i) x(i, 0) = (0.5 + i) / m;

    Description inVars(0);
    inVars.add("a");
    inVars.add("b");
    inVars.add("c");
    inVars.add("x");
    Description formulas(1, "a + b * exp(c * x)");
    formulas.add("(a * x^2 + b) / (c + x^2)");
    SymbolicFunction g(inVars, formulas);
    Point trueParameter(0);
    trueParameter.add(2.8);
    trueParameter.add(1.2);
    trueParameter.add(0.5);
    Indices params(3);
    params.fill();
    ParametricFunction model(g, params, trueParameter);
    Sample y = model(x);
    y += Normal(Point(2), Point(2, 0.05), IdentityMatrix(2)).getSample(y.getSize());
    Point candidate(3, 1.0);
    CovarianceMatrix priorCovariance(3);
    for (UnsignedInteger i = 0; i < 3; ++i)
    {
      priorCovariance(i, i) = 3.0 + (1.0 + i) * (1.0 + i);
      for (UnsignedInteger j = 0; j < i; ++j)
        priorCovariance(i, j) = 1.0 / (1.0 + i + j);
    }
    CovarianceMatrix errorCovariance(2);
    for (UnsignedInteger i = 0; i < 1; ++i)
    {
      errorCovariance(i, i) = 2.0 + (1.0 + i) * (1.0 + i);
      for (UnsignedInteger j = 0; j < i; ++j)
        errorCovariance(i, j) = 1.0 / (1.0 + i + j);
    }
    CovarianceMatrix globalErrorCovariance(2 * m);
    for (UnsignedInteger i = 0; i < 2 * m; ++i)
    {
      globalErrorCovariance(i, i) = 0.01 * (1.0 + (1.0 + i) * (1.0 + i));
      for (UnsignedInteger j = 0; j < i; ++j)
        globalErrorCovariance(i, j) = 0.01 / (1.0 + i + j);
    }
    Indices bootstrapSizes(0);
    bootstrapSizes.add(0);
    bootstrapSizes.add(100);
    for (UnsignedInteger n = 0; n < bootstrapSizes.getSize(); ++n)
    {
      // With default optim
      fullprint << "Bootstrap size =" << bootstrapSizes[n] << std::endl;
      fullprint << "1. Default optim" << std::endl;
      GaussianNonLinearCalibration algo(model, x, y, candidate, priorCovariance, errorCovariance);
      algo.setBootstrapSize(bootstrapSizes[n]);
      algo.run();
      // To avoid discrepance between the plaforms with or without CMinpack
      Point parameterMAP(algo.getResult().getParameterMAP());
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 5e-1);
      // With TNC
      fullprint << "2. TNC optim" << std::endl;
      algo.setOptimizationAlgorithm(MultiStart(TNC(), LowDiscrepancyExperiment(SobolSequence(), Normal(candidate, CovarianceMatrix(candidate.getDimension())), ResourceMap::GetAsUnsignedInteger("GaussianNonLinearCalibration-MultiStartSize")).generate()));
      algo.run();
      parameterMAP = algo.getResult().getParameterMAP();
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 5e-1);
      // With globalErrorCovariance
      fullprint << "3. globalErrorCovariance" << std::endl;
      algo = GaussianNonLinearCalibration(model, x, y, candidate, priorCovariance, globalErrorCovariance);
      algo.setBootstrapSize(bootstrapSizes[n]);
      algo.run();
      parameterMAP = algo.getResult().getParameterMAP();
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 5e-1);
    } // n
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
