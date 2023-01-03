//                                               -*- C++ -*-
/**
 *  @brief The test file of KrigingAlgorithm class using
 *  StationaryFunctionalCovarianceModel
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
    // Learning data
    Point levels = {8, 6};
    // Define the Box
    Box box(levels);
    // Get the input sample
    Sample inputSample( box.generate() );
    // Scale each direction
    inputSample *= 10;

    // Define model
    Description inputDescription = {"x", "y"};

    Description formula = {"cos(0.5*x) + sin(y)"};
    const SymbolicFunction model(inputDescription, formula);

    // Build the output sample
    const Sample  outputSample( model(inputSample) );

    // Validation
    Collection<Distribution> coll(2);
    coll[0] = Uniform(1.0, 9.0);
    coll[1] = Uniform(1.0, 9.0);
    ComposedDistribution dist(coll);

    const Sample inputValidation(dist.getSample(10));
    const Sample outputValidation(model(inputValidation));

    // 2) Reimplement the squared exponential covariance model
    formula[0] = "exp(-0.5* (x * x + y * y))" ;
    const SymbolicFunction rho(inputDescription, formula);
    const Point scale = {6.0, 2.0};
    const Point amplitude = {1.5};

    StationaryFunctionalCovarianceModel covarianceModel(scale, amplitude, rho);

    // 3) Basis definition
    Basis basis(LinearBasisFactory(2).build());

    // 4) Kriging algorithm
    KrigingAlgorithm algo(inputSample, outputSample, covarianceModel, basis);
    Point start(inputSample.getDimension(), 50.0);
    Point loglikelihood(algo.getReducedLogLikelihoodFunction()(start));
    algo.setOptimizeParameters(false);
    algo.run();
    KrigingResult result(algo.getResult());
    Function metaModel(result.getMetaModel());
    assert_almost_equal(result.getConditionalMarginalVariance(inputSample),
                        Sample(inputSample.getSize(), 1), 1e-14, 1e-14);


    // Consistency check: does the reimplementation fit the SquaredExponential class?
    SquaredExponential SE(inputSample.getDimension());
    SE.setScale(scale);
    SE.setAmplitude(amplitude);
    KrigingAlgorithm algoSE(inputSample, outputSample, SE, basis);
    Point loglikelihoodSE(algoSE.getReducedLogLikelihoodFunction()(start));
    assert_almost_equal(loglikelihood, loglikelihoodSE, 1e-8, 1e-8);

    // High level consistency check: does the prediction fit too?
    algoSE.setOptimizeParameters(false);
    algoSE.run();
    KrigingResult resultSE(algoSE.getResult());
    Function metaModelSE(resultSE.getMetaModel());
    assert_almost_equal(metaModel(inputValidation), metaModelSE(inputValidation), 1e-8, 1e-8);

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
