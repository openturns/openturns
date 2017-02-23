//                                               -*- C++ -*-
/**
 *  @brief The test file of GeneralLinearModelAlgorithm class
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;


int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Set Numerical precision to 3
    PlatformInfo::SetNumericalPrecision(3);

    std::cout << "================" << std::endl;
    std::cout << "Test using NLOpt" << std::endl;
    std::cout << "================" << std::endl;
    // Calibration of default optimizer
    ResourceMap::SetAsNumericalScalar("GeneralLinearModelAlgorithm-DefaultOptimizationLowerBound", 1.0e-5);
    ResourceMap::SetAsNumericalScalar("GeneralLinearModelAlgorithm-DefaultOptimizationUpperBound", 100);
    // Data & estimation
    const UnsignedInteger spatialDimension = 1;
    NumericalSample X = Normal(0, 1).getSample(100);
    X = X.sortAccordingToAComponent(0);
    SquaredExponential covarianceModel(1);
    Description inDescription(1);
    inDescription[0] = "x";
    Description formula(1);
    formula[0] = "x - 0.6 * cos(x/3)";
    NumericalMathFunction model(inDescription, formula);
    const NumericalSample Y = model(X);
    const Basis basis = QuadraticBasisFactory(spatialDimension).build();
    GeneralLinearModelAlgorithm algo(X, Y, covarianceModel, basis, true);
    NLopt solver("LN_NELDERMEAD");
    algo.setOptimizationSolver(solver);
    algo.run();

    // perform an evaluation
    GeneralizedLinearModelResult result = algo.getResult();
    const NumericalMathFunction metaModel = result.getMetaModel();
    CovarianceModel conditionalCovariance = result.getCovarianceModel();
    const NumericalSample residual = metaModel(X) - Y;
    assert_almost_equal(residual.computeCenteredMoment(2), NumericalPoint(1, 1.06e-05), 1e-5, 1e-5);
    NumericalPoint parameter(2);
    parameter[0] = 0.702138;
    parameter[1] = 0.00137;
    assert_almost_equal(conditionalCovariance.getParameter(), parameter, 2e-3, 1e-3);
    std::cout << "Test Ok" << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
