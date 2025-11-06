//                                               -*- C++ -*-
/**
 *  @brief The test file of GaussianProcessFitter class
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // Data & estimation
    const UnsignedInteger inputDimension = 1;
    Sample X = Normal(0, 1).getSample(100);
    X = X.sortAccordingToAComponent(0);
    SquaredExponential covarianceModel(1);
    Description inDescription(1);
    inDescription[0] = "x";
    Description formula(1);
    formula[0] = "x - 0.6 * cos(x/3)";
    SymbolicFunction model(inDescription, formula);
    const Sample Y = model(X);
    const Basis basis = QuadraticBasisFactory(inputDimension).build();
    GaussianProcessFitter algo(X, Y, covarianceModel, basis);
    NLopt solver("LN_NELDERMEAD");
    algo.setOptimizationAlgorithm(solver);
    algo.run();

    // perform an evaluation
    GaussianProcessFitterResult result = algo.getResult();
    const Function metaModel = result.getMetaModel();
    const CovarianceModel conditionalCovariance = result.getCovarianceModel();
    const Sample residual = metaModel(X) - Y;
    assert_almost_equal(residual.computeCentralMoment(2), Point(1, 1.06e-05), 1e-5, 1e-5);
    std::cout << "param=" << conditionalCovariance.getParameter() << std::endl;
    const Point parameter = {0.620868, 0.000941836};
    assert_almost_equal(conditionalCovariance.getParameter(), parameter, 1e-2, 1e-2);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
