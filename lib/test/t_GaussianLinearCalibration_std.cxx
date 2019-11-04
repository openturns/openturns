//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    UnsignedInteger m = 10;
    Sample x(m, 1);
    for (UnsignedInteger i = 0; i < m; ++i) x(i, 0) = 0.5 + i;

    Description inVars(0);
    inVars.add("a");
    inVars.add("b");
    inVars.add("c");
    inVars.add("x");
    Description formulas(1, "a + b * exp(c * x)");
    formulas.add("(a * x^2 + b) / (c + x^2)");
    SymbolicFunction model(inVars, formulas);
    Point p_ref(0);
    p_ref.add(2.8);
    p_ref.add(1.2);
    p_ref.add(0.5);
    Indices params(3);
    params.fill();
    ParametricFunction modelX(model, params, p_ref);
    Sample y = modelX(x);
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
      GaussianLinearCalibration algo(modelX, x, y, candidate, priorCovariance, errorCovariance, methods[n]);
      algo.run();
      fullprint << "result (const. 1)=" << algo.getResult() << std::endl;
      modelX.setParameter(candidate);
      Sample modelObservations(modelX(x));
      Matrix transposedGradientObservations(modelX.getParameterDimension(), y.getSize() * modelX.getOutputDimension());
      UnsignedInteger shift = 0;
      UnsignedInteger skip = modelX.getOutputDimension() * modelX.getParameterDimension();
      for (UnsignedInteger i = 0; i < y.getSize(); ++i)
      {
        Matrix localGradient(modelX.parameterGradient(x[i]));
        std::copy(localGradient.getImplementation()->begin(), localGradient.getImplementation()->end(), transposedGradientObservations.getImplementation()->begin() + shift);
        shift += skip;
      }
      algo = GaussianLinearCalibration(modelObservations, transposedGradientObservations.transpose(), y, candidate, priorCovariance, errorCovariance, methods[n]);
      algo.run();
      fullprint << "result (const. 2)=" << algo.getResult() << std::endl;
      algo = GaussianLinearCalibration(modelX, x, y, candidate, priorCovariance, globalErrorCovariance, methods[n]);
      algo.run();
      fullprint << "result   (global)=" << algo.getResult() << std::endl;
    } // n
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
