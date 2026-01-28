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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  // Problem parameters
  const UnsignedInteger dimension = 3;
  const Scalar a = 7.0;
  const Scalar b = 0.1;
  // Create the Ishigami function
  const Description inputVariables = {"x1", "x2", "x3"};
  Description formula(1);
  formula[0] = (OSS() << "sin(x1) + (" << a << ") * (sin(x2)) ^ 2 + (" << b << ") * x3^4 * sin(x1)");
  const SymbolicFunction model(inputVariables, formula);

  // Create the input distribution
  const Collection<Distribution> marginals(dimension, Uniform(-M_PI, M_PI));
  const JointDistribution distribution(marginals);

  // Create the orthogonal basis
  const Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension, LegendreFactory());

  const LinearEnumerateFunction enumerateFunction(dimension);
  const OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);
  fullprint << productBasis.__str__() << std::endl;

  // Create the adaptive strategy
  const UnsignedInteger degree = 12;
  const UnsignedInteger basisDimension = enumerateFunction.getBasisSizeFromTotalDegree(degree);
  const FixedStrategy adaptiveStrategy(productBasis, basisDimension);
  // Create the result object here in order to test the save/load mechanism outside of the double loop
  FunctionalChaosResult result;
  // Create the projection strategy
  const UnsignedInteger samplingSize = 500;
  // Full: ProjectionStrategy projectionStrategy{LeastSquaresStrategy()};
  // Sparse:
  const FittingAlgorithm fittingAlgorithm = CorrectedLeaveOneOut();
  const LeastSquaresStrategy projectionStrategy(LeastSquaresMetaModelSelectionFactory(LARS(), fittingAlgorithm));

  const MonteCarloExperiment experiment(distribution, samplingSize);
  RandomGenerator::SetSeed(0);
  const Sample X(experiment.generate());
  const Sample Y(model(X));

  // Create the polynomial chaos algorithm
  FunctionalChaosAlgorithm algo(X, Y, distribution, adaptiveStrategy, projectionStrategy);
  algo.run();

  // Print the results
  result = algo.getResult();
  fullprint << result.__str__() << std::endl;
  const UnsignedInteger errorSampleSize = 1000;
  const Scalar atol = 1.0e-3;
  fullprint << "atol = " << atol << std::endl;

  // Create list of functions and the corresponding marginal indices
  Collection<Function> functionCollection;
  Collection<Indices> listOfConditioningIndices;
  // Condition with respect to X1
  const SymbolicFunction conditionalExpectationGivenX1({"a", "b", "x1"}, {"a / 2 + (1 + b * pi_^4 / 5) * sin(x1)"});
  functionCollection.add(conditionalExpectationGivenX1);
  Indices indices1 = {0};
  listOfConditioningIndices.add(indices1);
  // Condition with respect to X2
  const SymbolicFunction conditionalExpectationGivenX2({"a", "b", "x2"}, {"a * sin(x2)^2"});
  functionCollection.add(conditionalExpectationGivenX2);
  Indices indices2 = {1};
  listOfConditioningIndices.add(indices2);
  // Condition with respect to X3
  const SymbolicFunction conditionalExpectationGivenX3({"a", "b", "x3"}, {"a / 2"});
  functionCollection.add(conditionalExpectationGivenX3);
  Indices indices3 = {2};
  listOfConditioningIndices.add(indices3);
  // Given X1, X2
  const SymbolicFunction conditionalExpectationGivenX1X2({"a", "b", "x1", "x2"}, {"a * sin(x2)^2 + (1 + b * pi_^4 / 5) * sin(x1)"});
  functionCollection.add(conditionalExpectationGivenX1X2);
  Indices indices01 = {0, 1};
  listOfConditioningIndices.add(indices01);
  // Given X1, X3
  const SymbolicFunction conditionalExpectationGivenX1X3({"a", "b", "x1", "x3"}, {"a / 2 + (1 + b * x3^4) * sin(x1)"});
  functionCollection.add(conditionalExpectationGivenX1X3);
  Indices indices02 = {0, 2};
  listOfConditioningIndices.add(indices02);
  // Given X2, X3
  const SymbolicFunction conditionalExpectationGivenX2X3({"a", "b", "x2", "x3"}, {"a * sin(x2)^2"});
  functionCollection.add(conditionalExpectationGivenX2X3);
  Indices indices12 = {1, 2};
  listOfConditioningIndices.add(indices12);
  // Given X1, X2, X3
  const SymbolicFunction conditionalExpectationGivenX1X2X3({"a", "b", "x1", "x2", "x3"}, {"sin(x1) + a * (sin(x2)) ^ 2 + b * x3^4 * sin(x1)"});
  functionCollection.add(conditionalExpectationGivenX1X2X3);
  Indices indices012 = {0, 1, 2};
  listOfConditioningIndices.add(indices012);
  //
  fullprint << "functionCollection = " << functionCollection << std::endl;
  fullprint << "listOfConditioningIndices = " << listOfConditioningIndices << std::endl;
  for (UnsignedInteger k = 0; k < functionCollection.getSize(); ++k)
  {
    const Indices indices = listOfConditioningIndices[k];
    fullprint << "Test #" << k << " / " << functionCollection.getSize()
              << ", condition with respect to X" << indices << std::endl;
    // Conditional expectation of PCE given X
    const FunctionalChaosResult ceOfPCEGivenX(result.getConditionalExpectation(indices));
    const Function functionCEGivenX(ceOfPCEGivenX.getMetaModel());
    // Exact result Y | X
    const ParametricFunction functionEgivenXExact(functionCollection[k], {0, 1}, {a, b});
    // Compute L2 error
    const Distribution distributionMarginalGivenX(distribution.getMarginal(indices));
    const LowDiscrepancyExperiment qmcExperiment(SobolSequence(), distributionMarginalGivenX, errorSampleSize, true);
    const ExperimentIntegration integration(qmcExperiment);
    const Point errorGivenX1(integration.computeL2Norm(functionCEGivenX - functionEgivenXExact));
    fullprint << "    L2 Error = " << errorGivenX1[0] << std::endl;
    String errMsg = "Conditional expectation of PCE with respect to " + indices.__str__();
    assert_equal(errorGivenX1[0] < atol, true, errMsg);
  }

  return ExitCode::Success;
}
