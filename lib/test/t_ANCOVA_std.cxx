//                                               -*- C++ -*-
/**
 *  @brief The test file of ANCOVA method
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Problem parameters
    UnsignedInteger dimension(2);
    NumericalScalar rho(0.3);
    NumericalScalar a(4.);
    NumericalScalar b(5.);

    // Reference analytical values
    NumericalScalar covTh(a * a + b * b + 2 * a * b * rho);
    NumericalSample Si(2, 2);
    Si[0][0] = (a * a + a * b * rho) / covTh;
    Si[1][0] = (b * b + a * b * rho) / covTh;
    Si[0][1] = a * a / covTh;
    Si[1][1] = b * b / covTh;

    // Function
    Description inputVariables(dimension);
    inputVariables[0] = "X1";
    inputVariables[1] = "X2";
    Description outputVariables(1);
    outputVariables[0] = "y";
    Description formula(1);
    formula[0] = (OSS() << a << "* X1 + " << b << "* X2");

    NumericalMathFunction model(inputVariables, outputVariables, formula);

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

    NumericalSample sample(myCorrelatedInputDistribution.getSample(2000));

    // Orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      polynomialCollection[i] = HermiteFactory();

    EnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Adaptive strategy
    UnsignedInteger degree(4);
    FixedStrategy adaptiveStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree));

    // Projection strategy
    UnsignedInteger samplingSize(250);
    MonteCarloExperiment experiment(samplingSize);
    LeastSquaresStrategy projectionStrategy(experiment);

    // Polynomial chaos algorithm
    FunctionalChaosAlgorithm algo(model, distribution, adaptiveStrategy, projectionStrategy);
    RandomGenerator::SetSeed(0);
    algo.run();

    // Post-process the results
    FunctionalChaosResult result(algo.getResult());
    ANCOVA ancova(result, sample);
    NumericalPoint indices(ancova.getIndices());
    NumericalPoint uncorrelatedIndices(ancova.getUncorrelatedIndices());

    for(UnsignedInteger i = 0; i < dimension; ++i)
    {
      NumericalScalar value = indices[i];
      fullprint << "ANCOVA index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << fabs(value - Si[i][0]) << std::endl;
      value = uncorrelatedIndices[i];
      fullprint << "ANCOVA uncorrelated index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << fabs(value - Si[i][1]) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
