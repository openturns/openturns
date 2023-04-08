//                                               -*- C++ -*-
/**
 *  @brief The test file of FunctionalChaosAlgoritm class
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

// Simultaneously sort the nodes and weights
void printSobolResult(Scalar S_computed, Scalar S_exact)
{
  OStream fullprint(std::cout);
  fullprint << "   S (PCE) " << S_computed << std::endl;
  fullprint << "   S (exact) " << S_exact << std::endl;
  Scalar absoluteError = abs(S_computed - S_exact);
  fullprint << "   Abs. Error " << absoluteError << std::endl;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    // Problem parameters
    UnsignedInteger dimension = 3;
    Scalar a = 7.0;
    Scalar b = 0.1;
    // Reference analytical values
    Scalar mean = a / 2;
    Scalar variance = 1.0 / 2.0 + pow(a, 2.0) / 8.0 + b * pow(M_PI, 4.0) / 5.0 + pow(b, 2.0) * pow(M_PI, 8.0) / 18.0;
    Scalar S1 = (1.0 / 2.0 + b * pow(M_PI, 4.0) / 5.0 + pow(b, 2.0) * pow(M_PI, 8.0) / 50.0) / variance;
    Scalar S2 = (pow(a, 2.0) / 8.0) / variance;
    Scalar S3 = 0.0;
    Scalar S12 = 0.0;
    Scalar S13 = (pow(b, 2.0) * pow(M_PI, 8.0) / 2.0 * (1.0 / 9.0 - 1.0 / 25.0)) / variance;
    Scalar S23 = 0.0;
    Scalar ST1 = S1 + S13;
    Scalar ST2 = S2;
    Scalar ST3 = S3 + S13;
    Scalar S123 = 0.0;
    fullprint << "mean = " << mean << std::endl;
    fullprint << "variance = " << variance << std::endl;
    fullprint << "S1 = " << S1 << std::endl;
    fullprint << "S2 = " << S2 << std::endl;
    fullprint << "S3 = " << S3 << std::endl;
    fullprint << "S13 = " << S13 << std::endl;
    fullprint << "ST1 = " << ST1 << std::endl;
    fullprint << "ST2 = " << ST2 << std::endl;
    fullprint << "ST3 = " << ST3 << std::endl;
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
    ComposedDistribution distribution(marginals);

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    polynomialCollection[0] = LegendreFactory();
    polynomialCollection[1] = LegendreFactory();
    polynomialCollection[2] = LegendreFactory();

    LinearEnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Create the adaptive strategy
    // We can choose amongst several strategies
    // First, the most efficient (but more complex!) strategy
    UnsignedInteger degree = 8;
    UnsignedInteger basisSize = enumerateFunction.getBasisSizeFromTotalDegree(degree);
    fullprint << "basisSize = " << basisSize << std::endl;
    FixedStrategy adaptiveStrategy( productBasis, basisSize );
    FittingAlgorithm fittingAlgorithm = CorrectedLeaveOneOut();
    const LeastSquaresStrategy projectionStrategy(LeastSquaresMetaModelSelectionFactory(LARS(), fittingAlgorithm));
    UnsignedInteger size = 1000;
    const Sample X(distribution.getSample(size));
    const Sample Y(model(X));
    fullprint << "Create object" << std::endl;
    FunctionalChaosAlgorithm algo(X, Y, distribution, adaptiveStrategy, projectionStrategy);
    fullprint << "Run()" << std::endl;
    algo.run();
    fullprint << "GetResult()" << std::endl;
    FunctionalChaosResult result = algo.getResult();
    FunctionalChaosSobolIndices sensitivity(result);
    String report(sensitivity.__str__());
    fullprint << report << std::endl;
    //
    const Scalar rtol = 0.0;
    const Scalar atol = 0.001;
    Scalar S_computed;
    Scalar S_exact;
    // 
    fullprint << "Test first order Sobol' indices" << std::endl;
    fullprint << "First order, X1" << std::endl;
    S_computed = sensitivity.getSobolIndex(0);
    printSobolResult(S_computed, S1);
    assert_almost_equal(S_computed, S1, rtol, atol);
    //
    fullprint << "First order, X2" << std::endl;
    S_computed = sensitivity.getSobolIndex(1);
    printSobolResult(S_computed, S2);
    assert_almost_equal(S_computed, S2, rtol, atol);
    //
    fullprint << "First order, X3" << std::endl;
    S_computed = sensitivity.getSobolIndex(2);
    printSobolResult(S_computed, S3);
    assert_almost_equal(S_computed, S3, rtol, atol);
    // 
    fullprint << "Test total order Sobol' indices" << std::endl;
    fullprint << "Total, X1" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(0);
    printSobolResult(S_computed, ST1);
    assert_almost_equal(S_computed, ST1, rtol, atol);
    //
    fullprint << "Total, X2" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(1);
    printSobolResult(S_computed, ST2);
    assert_almost_equal(S_computed, ST2, rtol, atol);
    //
    fullprint << "Total, X3" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(2);
    printSobolResult(S_computed, ST3);
    assert_almost_equal(S_computed, ST3, rtol, atol);
    // 
    fullprint << "Test first order (closed) group Sobol' indices" << std::endl;
    fullprint << "X1" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({0});
    printSobolResult(S_computed, S1);
    assert_almost_equal(S_computed, S1, rtol, atol);
    //
    fullprint << "X2" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({1});
    printSobolResult(S_computed, S2);
    assert_almost_equal(S_computed, S2, rtol, atol);
    //
    fullprint << "X3" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({2});
    printSobolResult(S_computed, S3);
    assert_almost_equal(S_computed, S3, rtol, atol);
    //
    fullprint << "(X1, X2)" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({0, 1});
    S_exact = S1 + S2 + S12;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X1, X3)" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({0, 2});
    S_exact = S1 + S3 + S13;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X2, X3)" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({1, 2});
    S_exact = S2 + S3 + S23;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X1, X2, X3)" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({0, 1, 2});
    S_exact = 1.0;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    // 
    fullprint << "Test total group Sobol' indices" << std::endl;
    fullprint << "X1" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex({0});
    printSobolResult(S_computed, ST1);
    assert_almost_equal(S_computed, ST1, rtol, atol);
    //
    fullprint << "X2" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex({1});
    printSobolResult(S_computed, ST2);
    assert_almost_equal(S_computed, ST2, rtol, atol);
    //
    fullprint << "X3" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex({2});
    printSobolResult(S_computed, ST3);
    assert_almost_equal(S_computed, ST3, rtol, atol);
    //
    fullprint << "(X1, X2)" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex({0, 1});
    S_exact = S1 + S2 + S12 + S13 + S123;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X1, X3)" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex({0, 2});
    S_exact = S1 + S3 + S13 + S23 + S123;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X2, X3)" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex({1, 2});
    S_exact = S2 + S3 + S13 + S23 + S123;;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X1, X2, X3)" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex({0, 1, 2});
    S_exact = 1.0;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
