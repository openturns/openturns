//                                               -*- C++ -*-
/**
 *  @brief The test file of KrigingRandomVector class
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
    PlatformInfo::SetNumericalPrecision(2);

    // Learning data
    Point levels(2);
    levels[0] = 8;
    levels[1] = 5;
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

    // 2) Definition of exponential model
    Point scale(2);
    scale[0] = 5.33532;
    scale[1] = 2.61534;
    Point amplitude(1, 1.61536);

    SquaredExponential covarianceModel(scale, amplitude);

    // 3) Basis definition
    Basis basis(ConstantBasisFactory(2).build());

    // Kriring algorithm
    KrigingAlgorithm algo(inputSample, outputSample, covarianceModel, basis);
    algo.setOptimizeParameters(false);
    algo.run();

    // Get result
    KrigingResult result(algo.getResult());

    // Get meta model
    Function metaModel(result.getMetaModel());

    // Interpolation error
    assert_almost_equal(outputSample,  metaModel(inputSample), 3.0e-5, 3.0e-5);


    // Kriging variance is 0 on learning points
    CovarianceMatrix var(result.getConditionalCovariance(inputSample));

    // assert_almost_equal could not be applied to matrices
    Point covariancePoint(*var.getImplementation());
    assert_almost_equal(covariancePoint, Point(covariancePoint.getSize()), 1e-6, 1e-6);

    // Random vector evaluation
    Sample unifRealization(Uniform(0.0, 10.0).getSample(2));
    Point validationPoint(unifRealization.getImplementation()->getData());
    KrigingRandomVector rvector(result, validationPoint);

    // Realization of the random vector
    Point realization (rvector.getRealization());
    std::cout << "Realization of the KRV=" << realization << std::endl;

    // Get a sample of size 10
    Sample realizations(rvector.getSample(10));
    std::cout << "Sample of realizations of the KRV=" << realizations << std::endl;
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
