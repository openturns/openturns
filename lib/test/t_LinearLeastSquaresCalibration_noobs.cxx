//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearLeastSquaresCalibration for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // A case without observed inputs
    UnsignedInteger m = 1000;
    Sample x(m, 0);

    Description inVars(0);
    inVars.add("a");
    inVars.add("b");
    inVars.add("c");
    // This g is linear in (a, b, c) and identifiable.
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
    Point trueParameter(0);
    trueParameter.add(2.8);
    trueParameter.add(1.2);
    trueParameter.add(0.5);
    Indices params(inputDimension);
    params.fill();
    ParametricFunction model(g, params, trueParameter);
    Sample y = model(x);
    y += Normal(Point(outputDimension), Point(outputDimension, 0.05), IdentityMatrix(outputDimension)).getSample(y.getSize());
    Point candidate(inputDimension, 1.0);
    Description methods(0);
    methods.add("SVD");
    methods.add("QR");
    methods.add("Cholesky");
    for (UnsignedInteger n = 0; n < methods.getSize(); ++n)
    {
      fullprint << "method=" << methods[n] << std::endl;
      // 1st constructor
      fullprint << "(const. 1)" << std::endl;
      LinearLeastSquaresCalibration algo(model, x, y, candidate, methods[n]);
      algo.run();
      Point parameterMAP(algo.getResult().getParameterMAP());
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 1e-2);
      // 2nd constructor
      fullprint << "(const. 2)" << std::endl;
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
      algo = LinearLeastSquaresCalibration(modelObservations, transposedGradientObservations.transpose(), y, candidate, methods[n]);
      algo.run();
      parameterMAP = algo.getResult().getParameterMAP();
      fullprint << "MAP =" << parameterMAP << std::endl;
      assert_almost_equal(parameterMAP, trueParameter, 1e-2);
    } // n
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
