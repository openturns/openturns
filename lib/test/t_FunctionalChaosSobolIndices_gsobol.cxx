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

Scalar sobol(const Indices & indices,
             const Point & a,
             Scalar variance
            )
{
  Scalar value = 1.0;
  for (UnsignedInteger i = 0; i < indices.getSize(); ++i)
  {
    value *= 1.0 / (3.0 * pow(1.0 + a[indices[i]], 2.0));
  }
  return value / variance;
}

// Simultaneously sort the nodes and weights
void printSobolResult(Scalar S_computed, Scalar S_exact)
{
  OStream fullprint(std::cout);
  fullprint << "   S (PCE) " << std::fixed << std::setprecision(4) << S_computed << std::endl;
  fullprint << "   S (exact) " << std::fixed << std::setprecision(4) << S_exact << std::endl;
  Scalar absoluteError = abs(S_computed - S_exact);
  fullprint << "   Abs. Error " << std::scientific << std::setprecision(4) << absoluteError << std::endl;
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
    // Reference analytical values
    Scalar mean = 1.0;
    Scalar variance = 1.0;
    Point a(dimension);
    // Create the gSobol function
    Description inputVariables(dimension);
    Description formula(1);
    formula[0] = "1.0";
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      a[i] = 0.5 * i;
      variance *= 1.0 + 1.0 / (3.0 * pow(1.0 + a[i], 2.0));
      inputVariables[i] = (OSS() << "xi" << i);
      formula[0] = (OSS() << formula[0] << " * ((abs(4.0 * xi" << i << " - 2.0) + " << a[i] << ") / (1.0 + " << a[i] << "))");
    }
    --variance;
    SymbolicFunction model(inputVariables, formula);

    // 
    fullprint << "Reference analytical values" << std::endl;
    Scalar S0 = sobol({0}, a, variance);
    Scalar S1 = sobol({1}, a, variance);
    Scalar S2 = sobol({2}, a, variance);
    Scalar S01 = sobol({0, 1}, a, variance);
    Scalar S02 = sobol({0, 2}, a, variance);
    Scalar S12 = sobol({1, 2}, a, variance);
    Scalar S012 = sobol({0, 1, 2}, a, variance);
    Scalar ST0 = S0 + S01 + S02 + S012;
    Scalar ST1 = S1 + S01 + S12 + S012;
    Scalar ST2 = S2 + S02 + S12 + S012;
    fullprint << "  mean = " << mean << std::endl;
    fullprint << "  variance = " << std::fixed << std::setprecision(4) << variance << std::endl;
    fullprint << "  S0 = " << std::fixed << std::setprecision(4) << S0 << std::endl;
    fullprint << "  S1 = " << std::fixed << std::setprecision(4) << S1 << std::endl;
    fullprint << "  S2 = " << std::fixed << std::setprecision(4) << S2 << std::endl;
    fullprint << "  S01 = " << std::fixed << std::setprecision(4) << S01 << std::endl;
    fullprint << "  S02 = " << std::fixed << std::setprecision(4) << S02 << std::endl;
    fullprint << "  S12 = " << std::fixed << std::setprecision(4) << S12 << std::endl;
    fullprint << "  S012 = " << std::fixed << std::setprecision(4) << S012 << std::endl;
    fullprint << "  ST0 = " << std::fixed << std::setprecision(4) << ST0 << std::endl;
    fullprint << "  ST1 = " << std::fixed << std::setprecision(4) << ST1 << std::endl;
    fullprint << "  ST2 = " << std::fixed << std::setprecision(4) << ST2 << std::endl;

    // Create the input distribution
    Collection<Distribution> marginals(dimension, Uniform(0.0, 1.0));
    ComposedDistribution distribution(marginals);

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      polynomialCollection[i] = LegendreFactory();
    }

    HyperbolicAnisotropicEnumerateFunction enumerateFunction(dimension,  0.5);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    UnsignedInteger size = pow(2, 12);
    fullprint << "size = " << size << std::endl;
    WeightedExperiment experiment(LowDiscrepancyExperiment(SobolSequence(), distribution, size));
    // Select the basis
    UnsignedInteger degree = 26;
    UnsignedInteger basisSize = enumerateFunction.getBasisSizeFromTotalDegree(degree);
    fullprint << "basisSize = " << basisSize << std::endl;
    AdaptiveStrategy adaptiveStrategy(FixedStrategy(productBasis, basisSize));
    // Use full PCE (sparse PCE is too slow)
    ProjectionStrategy projectionStrategy{LeastSquaresStrategy()};
    const Sample X(experiment.generate());
    const Sample Y(model(X));
    fullprint << "Create object" << std::endl;
    FunctionalChaosAlgorithm algo(X, Y, distribution, adaptiveStrategy, projectionStrategy);
    fullprint << "Run()" << std::endl;
    algo.run();
    fullprint << "GetResult()" << std::endl;
    FunctionalChaosResult result = algo.getResult();
    FunctionalChaosSobolIndices sensitivity(result);
    ResourceMap::SetAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold", 0.00001);
    String report(sensitivity.__str__());
    fullprint << report << std::endl;
    //
    const Scalar rtol = 0.0;
    const Scalar atol = 0.1;
    Scalar S_computed;
    Scalar S_exact;
    //
    fullprint << "Test first order Sobol' indices" << std::endl;
    fullprint << "First order, X0" << std::endl;
    S_computed = sensitivity.getSobolIndex(0);
    printSobolResult(S_computed, S0);
    assert_almost_equal(S_computed, S0, rtol, atol);
    //
    fullprint << "First order, X1" << std::endl;
    S_computed = sensitivity.getSobolIndex(1);
    printSobolResult(S_computed, S1);
    assert_almost_equal(S_computed, S1, rtol, atol);
    //
    fullprint << "First order, X2" << std::endl;
    S_computed = sensitivity.getSobolIndex(2);
    printSobolResult(S_computed, S2);
    assert_almost_equal(S_computed, S2, rtol, atol);
    //
    fullprint << "Test total order Sobol' indices" << std::endl;
    fullprint << "Total, X0" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(0);
    printSobolResult(S_computed, ST0);
    assert_almost_equal(S_computed, ST0, rtol, atol);
    //
    fullprint << "Total, X1" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(1);
    printSobolResult(S_computed, ST1);
    assert_almost_equal(S_computed, ST1, rtol, atol);
    //
    fullprint << "Total, X2" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(2);
    printSobolResult(S_computed, ST2);
    assert_almost_equal(S_computed, ST2, rtol, atol);
    //
    fullprint << "Test first order (closed) group Sobol' indices" << std::endl;
    fullprint << "X0" << std::endl;
    Indices indices0 = {0};
    S_computed = sensitivity.getSobolGroupedIndex(indices0);
    printSobolResult(S_computed, S0);
    assert_almost_equal(S_computed, S0, rtol, atol);
    //
    fullprint << "X1" << std::endl;
    Indices indices1 = {1};
    S_computed = sensitivity.getSobolGroupedIndex(indices1);
    printSobolResult(S_computed, S1);
    assert_almost_equal(S_computed, S1, rtol, atol);
    //
    fullprint << "X2" << std::endl;
    Indices indices2 = {2};
    S_computed = sensitivity.getSobolGroupedIndex(indices2);
    printSobolResult(S_computed, S2);
    assert_almost_equal(S_computed, S2, rtol, atol);
    //
    fullprint << "(X0, X1)" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({0, 1});
    S_exact = S0 + S1 + S01;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X0, X2)" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({0, 2});
    S_exact = S0 + S2 + S02;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X1, X2)" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({1, 2});
    S_exact = S1 + S2 + S12;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X0, X1, X2)" << std::endl;
    S_computed = sensitivity.getSobolGroupedIndex({0, 1, 2});
    S_exact = 1.0;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "Test total group Sobol' indices" << std::endl;
    fullprint << "X0" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex(indices0);
    printSobolResult(S_computed, ST0);
    assert_almost_equal(S_computed, ST0, rtol, atol);
    //
    fullprint << "X1" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex(indices1);
    printSobolResult(S_computed, ST1);
    assert_almost_equal(S_computed, ST1, rtol, atol);
    //
    fullprint << "X2" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex(indices2);
    printSobolResult(S_computed, ST2);
    assert_almost_equal(S_computed, ST2, rtol, atol);
    //
    fullprint << "(X0, X1)" << std::endl;  // Difficult !
    S_computed = sensitivity.getSobolGroupedTotalIndex({0, 1});
    S_exact = S0 + S1 + S01 + S02 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X0, X2)" << std::endl;  // Difficult !
    S_computed = sensitivity.getSobolGroupedTotalIndex({0, 2});
    S_exact = S0 + S2 + S02 + S12 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X1, X2)" << std::endl;  // Difficult !
    S_computed = sensitivity.getSobolGroupedTotalIndex({1, 2});
    S_exact = S1 + S2 + S02 + S12 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "(X1, X2, X3)" << std::endl;
    S_computed = sensitivity.getSobolGroupedTotalIndex({0, 1, 2});
    S_exact = 1.0;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    //
    fullprint << "Test interaction group Sobol' indices" << std::endl;
    fullprint << "X0" << std::endl;
    S_computed = sensitivity.getSobolIndex(indices0);
    printSobolResult(S_computed, S0);
    assert_almost_equal(S_computed, S0, rtol, atol);
    // 
    fullprint << "X1" << std::endl;
    S_computed = sensitivity.getSobolIndex(indices1);
    printSobolResult(S_computed, S1);
    assert_almost_equal(S_computed, S1, rtol, atol);
    // 
    fullprint << "X2" << std::endl;
    S_computed = sensitivity.getSobolIndex(indices2);
    printSobolResult(S_computed, S2);
    assert_almost_equal(S_computed, S2, rtol, atol);
    // 
    fullprint << "X0, X1" << std::endl;
    S_computed = sensitivity.getSobolIndex({0, 1});
    printSobolResult(S_computed, S01);
    assert_almost_equal(S_computed, S01, rtol, atol);
    // 
    fullprint << "X0, X2" << std::endl;
    S_computed = sensitivity.getSobolIndex({0, 2});
    printSobolResult(S_computed, S02);
    assert_almost_equal(S_computed, S02, rtol, atol);
    // 
    fullprint << "X1, X2" << std::endl;
    S_computed = sensitivity.getSobolIndex({1, 2});
    printSobolResult(S_computed, S12);
    assert_almost_equal(S_computed, S12, rtol, atol);
    // 
    fullprint << "X0, X1, X2" << std::endl;
    S_computed = sensitivity.getSobolIndex({0, 1, 2});
    printSobolResult(S_computed, S012);
    assert_almost_equal(S_computed, S012, rtol, atol);
    //
    fullprint << "Test interaction total group Sobol' indices" << std::endl;
    fullprint << "X0" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(indices0);
    S_exact = S0 + S01 + S02 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    // 
    fullprint << "X1" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(indices1);
    S_exact = S1 + S01 + S12 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    // 
    fullprint << "X2" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex(indices2);
    S_exact = S2 + S02 + S12 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    // 
    fullprint << "X0, X1" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex({0, 1});
    S_exact = S01 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    // 
    fullprint << "X0, X2" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex({0, 2});
    S_exact = S02 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    // 
    fullprint << "X1, X2" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex({1, 2});
    S_exact = S12 + S012;
    printSobolResult(S_computed, S_exact);
    assert_almost_equal(S_computed, S_exact, rtol, atol);
    // 
    fullprint << "X0, X1, X2" << std::endl;
    S_computed = sensitivity.getSobolTotalIndex({0, 1, 2});
    printSobolResult(S_computed, S012);
    assert_almost_equal(S_computed, S012, rtol, atol);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
