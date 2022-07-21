//                                               -*- C++ -*-
/**
 *  @brief The test file of ANCOVA method
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
  setRandomGenerator();

  try
  {
    // Problem parameters
    UnsignedInteger dimension = 2;
    Scalar rho = 0.3;
    Scalar a = 4.;
    Scalar b = 5.;

    // Reference analytical values
    Scalar covTh = a * a + b * b + 2 * a * b * rho;
    Sample Si(2, 2);
    Si[0][0] = (a * a + a * b * rho) / covTh;
    Si[1][0] = (b * b + a * b * rho) / covTh;
    Si[0][1] = a * a / covTh;
    Si[1][1] = b * b / covTh;

    // Function
    Description inputVariables(dimension);
    inputVariables[0] = "X1";
    inputVariables[1] = "X2";
    Description formula(1);
    formula[0] = (OSS() << a << "* X1 + " << b << "* X2");

    SymbolicFunction model(inputVariables, formula);

    // Input distribution
    Collection<Distribution> marginals(dimension);
    marginals[0] = Normal();
    marginals[1] = Normal();
    ComposedDistribution distribution(marginals);

    // Correlated input distribution
    CorrelationMatrix S(2);
    S(1, 0) = 0.3;
    CorrelationMatrix R(NormalCopula::GetCorrelationFromSpearmanCorrelation(S));
    NormalCopula myCopula(R);
    ComposedDistribution myCorrelatedInputDistribution(marginals, myCopula);

    Sample sample(myCorrelatedInputDistribution.getSample(2000));

    // Orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      polynomialCollection[i] = HermiteFactory();

    LinearEnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Adaptive strategy
    UnsignedInteger degree = 4;
    FixedStrategy adaptiveStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree));

    // Projection strategy
    UnsignedInteger samplingSize = 250;
    MonteCarloExperiment experiment(samplingSize);
    LeastSquaresStrategy projectionStrategy(experiment);

    // Polynomial chaos algorithm
    FunctionalChaosAlgorithm algo(model, distribution, adaptiveStrategy, projectionStrategy);
    RandomGenerator::SetSeed(0);
    algo.run();

    // Post-process the results
    FunctionalChaosResult result(algo.getResult());
    ANCOVA ancova(result, sample);
    Point indices(ancova.getIndices());
    Point uncorrelatedIndices(ancova.getUncorrelatedIndices());

    for(UnsignedInteger i = 0; i < dimension; ++i)
    {
      Scalar value = indices[i];
      fullprint << "ANCOVA index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - Si[i][0]) << std::endl;
      value = uncorrelatedIndices[i];
      fullprint << "ANCOVA uncorrelated index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - Si[i][1]) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
