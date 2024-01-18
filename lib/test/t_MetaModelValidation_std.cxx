//                                               -*- C++ -*-
/**
 *  @brief The test file of FunctionalChaosAlgoritm class
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

    PlatformInfo::SetNumericalPrecision(3);
    // Problem parameters
    UnsignedInteger dimension = 3;
    Scalar a = 7.0;
    Scalar b = 0.1;
    // Create the Ishigami function
    Description inputVariables(dimension);
    inputVariables[0] = "xi1";
    inputVariables[1] = "xi2";
    inputVariables[2] = "xi3";
    Description formula(1);
    formula[0] = (OSS() << "sin(xi1) + (" << a << ") * (sin(xi2)) ^ 2 + (" << b << ") * xi3^4 * sin(xi1)");
    SymbolicFunction model(inputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginals(dimension);
    marginals[0] = Uniform(-M_PI, M_PI);
    marginals[1] = Uniform(-M_PI, M_PI);
    marginals[2] = Uniform(-M_PI, M_PI);
    JointDistribution distribution(marginals);

    // Fix sampling size
    const UnsignedInteger samplingSize = 100;

    // Get input & output sample
    LHSExperiment lhs(distribution, samplingSize);
    Sample inputSample = lhs.generate();
    Sample outputSample = model(inputSample);

    // Validation of results on independent samples
    const UnsignedInteger validationSize = 10;
    Sample inputValidation = distribution.getSample(validationSize);
    Sample outputValidation = model(inputValidation);

    // 1) SPC algorithm
    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension, LegendreFactory());

    LinearEnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Create the adaptive strategy
    UnsignedInteger degree = 8;
    UnsignedInteger basisSize = enumerateFunction.getStrataCumulatedCardinal(degree);
    AdaptiveStrategy adaptiveStrategy = FixedStrategy(productBasis, basisSize);

    // Select the fitting algorithm
    FittingAlgorithm fittingAlgorithm = KFold();

    // LSMSF
    LeastSquaresMetaModelSelectionFactory leastSquaresFactory(LARS(), fittingAlgorithm);

    // Projection strategy
    LeastSquaresStrategy projectionStrategy(inputSample, outputSample, leastSquaresFactory);

    FunctionalChaosAlgorithm algo(inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy);
    // Reinitialize the RandomGenerator to see the effect of the sampling method only
    RandomGenerator::SetSeed(0);
    algo.run();

    // Get the results
    FunctionalChaosResult result = algo.getResult();

    // MetaModelValidation - SPC
    MetaModelValidation metaModelValidationSPC(inputValidation, outputValidation, result.getMetaModel());

    fullprint << "Sparse chaos scoring" << std::endl;
    fullprint << "Q2 = " << std::setprecision(PlatformInfo::GetNumericalPrecision()) << std::fixed << metaModelValidationSPC.computePredictivityFactor() << std::endl;
    fullprint << "Residual sample = " << metaModelValidationSPC.getResidualSample() << std::endl;

    // 2) Kriging algorithm
    // KrigingAlgorithm
    Basis basis(QuadraticBasisFactory(dimension).build());
    // model computed
    Point scale(3);
    scale[0] = 3.52;
    scale[1] = 2.15;
    scale[2] = 2.99;
    Point amplitude(1, 11.41);
    CovarianceModel covarianceModel = GeneralizedExponential(scale, amplitude, 2.0);


    KrigingAlgorithm algo2(inputSample, outputSample, covarianceModel, basis);
    algo2.setOptimizeParameters(false);
    algo2.run();

    KrigingResult result2 = algo2.getResult();

    // MetaModelValidation - KG
    MetaModelValidation metaModelValidationKG(inputValidation, outputValidation, result2.getMetaModel());

    fullprint << "Kriging scoring" << std::endl;
    fullprint << "Q2 = " << std::setprecision(PlatformInfo::GetNumericalPrecision()) << std::fixed << metaModelValidationKG.computePredictivityFactor() << std::endl;
    PlatformInfo::SetNumericalPrecision(2);
    fullprint << "Residual sample = " << metaModelValidationKG.getResidualSample() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
