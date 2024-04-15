//                                               -*- C++ -*-
/**
 *  @brief The test file of KrigingAlgorithm class
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
    // Define the model
    const Scalar dimension = 3;
    const Description inputNames({"x1", "x2", "x3"});
    const Description formulas({"cos(x1 + x2 + x3)"});
    const SymbolicFunction model(inputNames, formulas);

    // Sample noisy data
    const Normal distribution(dimension);
    const UnsignedInteger samplesize = 100;
    const Sample x(distribution.getSample(samplesize));
    const Normal noiseDistribution(0.0, 0.1);
    const Sample noise(noiseDistribution.getSample(samplesize));
    const Sample y(model(x) + noise);

    // Build the surrogate model
    const ConstantBasisFactory basisFactory(dimension);
    const Basis basis(basisFactory.build());
    SquaredExponential covarianceModel(dimension);
    covarianceModel.activateNuggetFactor(true);
    KrigingAlgorithm algo(x, y, covarianceModel, basis);
    algo.run();
    KrigingResult result(algo.getResult());
    CovarianceModel optimizedCovarianceModel(result.getCovarianceModel());
    assert_almost_equal(optimizedCovarianceModel.getScale(), Point({1.34, 1.34, 1.34}), 0.2, 0.0);
    assert_almost_equal(std::log10(optimizedCovarianceModel.getNuggetFactor()), -1.9, 0.25, 0.0);

    // Product covariance kernel 
    // (with SquaredExponential => identical to the first)
    Collection<CovarianceModel> collection({SquaredExponential(), SquaredExponential(), SquaredExponential()});
    ProductCovarianceModel product(collection);
    product.activateNuggetFactor(true);
    KrigingAlgorithm algoProduct(x, y, product, basis);
    algoProduct.run();
    KrigingResult resultProduct(algoProduct.getResult());
    CovarianceModel optimizedProductModel(resultProduct.getCovarianceModel());
    assert_almost_equal(optimizedProductModel.getScale(), Point({1.34, 1.34, 1.34}), 0.2, 0.0);
    assert_almost_equal(std::log10(optimizedProductModel.getNuggetFactor()), -1.9, 0.25, 0.0);

    // Isotropic covariance kernel
    IsotropicCovarianceModel isotropic(SquaredExponential(), dimension);
    isotropic.activateNuggetFactor(true);
    KrigingAlgorithm algoIso(x, y, isotropic, basis);
    algoIso.run();
    KrigingResult resultIso(algoIso.getResult());
    CovarianceModel optimizedIsotropicModel(resultIso.getCovarianceModel());
    assert_almost_equal(optimizedIsotropicModel.getScale(), Point({1.34}), 0.2, 0.0);
    assert_almost_equal(std::log10(optimizedIsotropicModel.getNuggetFactor()), -1.9, 0.25, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
