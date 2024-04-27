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
  setRandomGenerator();

  // Problem parameters
  const UnsignedInteger dimension = 3;
  const Scalar a = 7.0;
  const Scalar b = 0.1;
  // Create the Ishigami function
  const Description inputVariables = {"xi1", "xi2", "xi3"};
  Description formula(1);
  formula[0] = (OSS() << "sin(xi1) + (" << a << ") * (sin(xi2)) ^ 2 + (" << b << ") * xi3^4 * sin(xi1)");
  const SymbolicFunction model(inputVariables, formula);

  // Create the input distribution
  const Collection<Distribution> marginals(dimension, Uniform(-M_PI, M_PI));
  const ComposedDistribution distribution(marginals);

  // Create the orthogonal basis
  const Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension, LegendreFactory());

  const LinearEnumerateFunction enumerateFunction(dimension);
  const OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);
  fullprint << productBasis.__str__() << std::endl;

  // Create the adaptive strategy
  const UnsignedInteger degree = 10;
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
  const Scalar atol = 5.0e-3;
  fullprint << "atol = " << atol << std::endl;

  // Condition with respect to X1
  fullprint << "Condition with respect to X1" << std::endl;
  const FunctionalChaosResult ceGivenX1(result.getConditionalExpectation({0}));
  fullprint << ceGivenX1.__str__() << std::endl;
  const Function functionCEGivenX1(ceGivenX1.getMetaModel());
  // Exact result Y | Xi1
  fullprint << "    Exact result Y | Xi1" << std::endl;
  const Description inputVariablesGivenX1 = {"xi1", "a", "b"};
  Description formulaGivenX1(1);
  formulaGivenX1[0] = (OSS() << "a / 2 + (1 + b * pi_^4 / 5) * sin(xi1)");
  const SymbolicFunction parametricEgivenX1(inputVariablesGivenX1, formulaGivenX1);
  const Indices indicesABGivenX1({1, 2});
  const Point parametersABGivenX1({a, b});
  const ParametricFunction functionEgivenX1Exact(parametricEgivenX1, indicesABGivenX1, parametersABGivenX1);
  // Compute L2 weighted norm
  fullprint << "    Compute L2 weighted norm" << std::endl;
  const Distribution distributionMarginalGivenX1(distribution.getMarginal(0));
  const MonteCarloExperiment experimentTestGivenX1(distributionMarginalGivenX1, errorSampleSize);
  const ExperimentIntegration integrationX1(experimentTestGivenX1);
  const Point errorGivenX1(integrationX1.computeL2Norm(functionCEGivenX1 - functionEgivenX1Exact));
  fullprint << "    Error = " << errorGivenX1[0] << std::endl;
  assert(errorGivenX1[0] < atol);

  // Condition with respect to X2
  fullprint << "Condition with respect to X2" << std::endl;
  const FunctionalChaosResult ceGivenX2(result.getConditionalExpectation({1}));
  fullprint << ceGivenX2.__str__() << std::endl;
  const Function functionCEGivenX2(ceGivenX2.getMetaModel());
  // Exact result Y | Xi2
  fullprint << "    Exact result Y | Xi2" << std::endl;
  const Description inputVariablesGivenX2 = {"xi2", "a", "b"};
  Description formula2GivenX2(1);
  formula2GivenX2[0] = (OSS() << "a * sin(xi2)^2");
  const SymbolicFunction parametricEgivenX2(inputVariablesGivenX2, formula2GivenX2);
  const Indices indicesABGivenX2({1, 2});
  const Point parametersABGivenX2({a, b});
  const ParametricFunction functionEgivenX2Exact(parametricEgivenX2, indicesABGivenX2, parametersABGivenX2);
  // Compute L2 weighted norm
  fullprint << "    Compute L2 weighted norm" << std::endl;
  const Distribution distributionMarginalGivenX2(distribution.getMarginal(1));
  const MonteCarloExperiment experimentTestGivenX2(distributionMarginalGivenX2, errorSampleSize);
  const ExperimentIntegration integrationX2(experimentTestGivenX2);
  const Point errorGivenX2(integrationX2.computeL2Norm(functionCEGivenX2 - functionEgivenX2Exact));
  fullprint << "    Error = " << errorGivenX2[0] << std::endl;
  assert(errorGivenX2[0] < atol);

  // Condition with respect to X3
  fullprint << "Condition with respect to X3" << std::endl;
  const FunctionalChaosResult ceGivenX3(result.getConditionalExpectation({2}));
  fullprint << ceGivenX3.__str__() << std::endl;
  const Function functionCEGivenX3(ceGivenX3.getMetaModel());
  // Exact result Y | Xi3
  fullprint << "    Exact result Y | Xi3" << std::endl;
  const Description inputVariablesGivenX3 = {"xi3", "a", "b"};
  Description formula2GivenX3(1);
  formula2GivenX3[0] = (OSS() << "a / 2");
  const SymbolicFunction parametricEgivenX3(inputVariablesGivenX3, formula2GivenX3);
  const Indices indicesABGivenX3({1, 2});
  const Point parametersABGivenX3({a, b});
  ParametricFunction functionEgivenX3Exact(parametricEgivenX3, indicesABGivenX3, parametersABGivenX3);
  // Compute L2 weighted norm
  fullprint << "    Compute L2 weighted norm" << std::endl;
  const Distribution distributionMarginalGivenX3(distribution.getMarginal(2));
  const MonteCarloExperiment experimentTestGivenX3(distributionMarginalGivenX3, errorSampleSize);
  const ExperimentIntegration integrationX3(experimentTestGivenX3);
  const Point errorGivenX3(integrationX3.computeL2Norm(functionCEGivenX3 - functionEgivenX3Exact));
  fullprint << "    Error = " << errorGivenX3[0] << std::endl;
  assert(errorGivenX3[0] < atol);

  return ExitCode::Success;
}
