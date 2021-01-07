//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussianLinearCalibration for standard methods
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

  try
  {
    PlatformInfo::SetNumericalPrecision(5);
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
    for (UnsignedInteger i = 0; i < 2; ++i)
    {
      errorCovariance(i, i) = 2.0 + (1.0 + i) * (1.0 + i);
      for (UnsignedInteger j = 0; j < i; ++j)
        errorCovariance(i, j) = 1.0 / (1.0 + i + j);
    }
    CovarianceMatrix globalErrorCovariance(2 * m);
    for (UnsignedInteger i = 0; i < 2 * m; ++i)
    {
      globalErrorCovariance(i, i) = 2.0 + (1.0 + i) * (1.0 + i);
      for (UnsignedInteger j = 0; j < i; ++j)
        globalErrorCovariance(i, j) = 1.0 / (1.0 + i + j);
    }
    Description methods(0);
    methods.add("SVD");
    methods.add("QR");
    methods.add("Cholesky");
    for (UnsignedInteger n = 0; n < methods.getSize(); ++n)
    {
      fullprint << "method=" << methods[n] << std::endl;
      // 1st constructor
      fullprint << "(const. 1)" << std::endl;
      GaussianLinearCalibration algo(model, x, y, candidate, priorCovariance, errorCovariance, methods[n]);
      algo.run();
      Point parameterMAP(algo.getResult().getParameterMAP());
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 10e-1);
      // 2nd constructor
      fullprint << "(const. 2)" << std::endl;
      fullprint << "error=" << algo.getResult().getObservationsError() << std::endl;
      model.setParameter(candidate);
      Sample modelObservations(model(x));
      Matrix transposedGradientObservations(model.getParameterDimension(), y.getSize() * model.getOutputDimension());
      UnsignedInteger shift = 0;
      UnsignedInteger skip = model.getOutputDimension() * model.getParameterDimension();
      for (UnsignedInteger i = 0; i < y.getSize(); ++i)
      {
        Matrix localGradient(model.parameterGradient(x[i]));
        std::copy(localGradient.getImplementation()->begin(), localGradient.getImplementation()->end(), transposedGradientObservations.getImplementation()->begin() + shift);
        shift += skip;
      }
      algo = GaussianLinearCalibration(modelObservations, transposedGradientObservations.transpose(), y, candidate, priorCovariance, errorCovariance, methods[n]);
      algo.run();
      parameterMAP = algo.getResult().getParameterMAP();
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 10e-1);
      // 3d constructor with globalErrorCovariance
      fullprint << "(const. 3)" << std::endl;
      fullprint << "error=" << algo.getResult().getObservationsError() << std::endl;
      algo = GaussianLinearCalibration(model, x, y, candidate, priorCovariance, globalErrorCovariance, methods[n]);
      algo.run();
      parameterMAP = algo.getResult().getParameterMAP();
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 10e-1);
    } // n
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
