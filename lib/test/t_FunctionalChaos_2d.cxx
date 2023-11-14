//                                               -*- C++ -*-
/**
 *  @brief Test of a FunctionalChaosAlgorithm with 2 outputs
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

    // Problem parameters
    const UnsignedInteger inputDimension = 2;
    const UnsignedInteger outputDimension = 2;

    // Create the function
    Description inputVariables(inputDimension);
    inputVariables[0] = "x1";
    inputVariables[1] = "x2";
    Description formula(outputDimension);
    formula[0] = "cos(x1 + x2)";
    formula[1] = "(x2 + 1) * exp(x1 - 2 * x2)";
    const SymbolicFunction model(inputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginals(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i) marginals[i] = Normal(0.0, 1.0);
    const ComposedDistribution distribution(marginals);

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      polynomialCollection[i] = HermiteFactory();
    }
    const LinearEnumerateFunction enumerateFunction(inputDimension);
    const OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    const UnsignedInteger degree = 4;
    const UnsignedInteger basisSize = enumerateFunction.getBasisSizeFromTotalDegree(degree);
    const AdaptiveStrategy adaptiveStrategy(FixedStrategy(productBasis, basisSize));


    // Create the projection strategy
    UnsignedInteger samplingSize = 100;
    const MonteCarloExperiment experiment(distribution, samplingSize);
    RandomGenerator::SetSeed(0);
    const Sample X(experiment.generate());
    const Sample Y(model(X));
    FunctionalChaosAlgorithm algo(X, Y, distribution, adaptiveStrategy, LeastSquaresStrategy());
    algo.run();

    // Examine the results
    const FunctionalChaosResult result(algo.getResult());
    const ProjectionStrategy projectionStrategy(algo.getProjectionStrategy());
    fullprint << "ProjectionStrategy (repr)= " << std::endl;
    fullprint << projectionStrategy << std::endl;
    std::cout << "ProjectionStrategy (str)= " << std::endl;
    std::cout << projectionStrategy << std::endl;
    Function composedMetamodel(result.getComposedMetaModel());
    fullprint << "ComposedMetamodel, ClassName= " << composedMetamodel.getClassName() << std::endl;
    fullprint << "ComposedMetamodel (repr)= " << std::endl;
    fullprint << composedMetamodel << std::endl;
    std::cout << "ComposedMetamodel (str)= " << std::endl;
    std::cout << composedMetamodel << std::endl;
    std::cout << "ComposedMetamodel (html)= " << std::endl;
    std::cout << composedMetamodel._repr_html_() << std::endl;

  } // try
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
