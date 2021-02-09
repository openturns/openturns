//                                               -*- C++ -*-
/**
 *  @brief The test file of KrigingAlgorithm class using 
 *  StationaryFunctionalCovarianceModel
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
    // Set Numerical precision to 3
    PlatformInfo::SetNumericalPrecision(3);

    // Learning data
    Point levels(2);
    levels[0] = 8;
    levels[1] = 6;
    // Define the Box
    Box box(levels);
    // Get the input sample
    Sample inputSample( box.generate() );
    // Scale each direction
    inputSample *= 10;

    // Define model
    Description inputDescription(2);
    inputDescription[0] = "x";
    inputDescription[1] = "y";

    Description formula(1);
    formula[0] = "cos(0.5*x) + sin(y)" ;
    const SymbolicFunction model(inputDescription, formula);

    // Build the output sample
    const Sample  outputSample( model(inputSample) );

    // 2) Definition of covariance model
    formula[0] = "exp(-0.5* (x * x + y * y))" ;
    const SymbolicFunction rho(inputDescription, formula);
    const Point scale = {6.72648, 3.49326};
    const Point amplitude = {4.90341};

    StationaryFunctionalCovarianceModel covarianceModel(scale, amplitude, rho);

    // 3) Basis definition
    Basis basis(LinearBasisFactory(2).build());

    // 4) Kriring algorithm
    KrigingAlgorithm algo(inputSample, outputSample, covarianceModel, basis);
    algo.setOptimizeParameters(false);
    algo.run();

    // Get result
    KrigingResult result(algo.getResult());

    // Get meta model
    Function metaModel(result.getMetaModel());
 
    Collection<Distribution> coll(2);
    coll[0] = Uniform(0, 1);
    coll[1] = Uniform(0, 1);
    ComposedDistribution dist(coll);

    const Sample inputValidation(dist.getSample(10));
    const Sample outputValidation(metaModel(inputValidation));

    // Approximation error
    assert_almost_equal(outputValidation,  metaModel(inputValidation), 5.0e-3, 5.0e-3);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
