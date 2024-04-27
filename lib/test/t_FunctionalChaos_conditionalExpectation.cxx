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
  ComposedDistribution distribution(marginals);

  // Create the orthogonal basis
  Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
  polynomialCollection[0] = LegendreFactory();
  polynomialCollection[1] = LegendreFactory();
  polynomialCollection[2] = LegendreFactory();

  LinearEnumerateFunction enumerateFunction(dimension);
  OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);
  fullprint << productBasis.__str__() << std::endl;

  // Create the adaptive strategy
  UnsignedInteger degree = 10;
  UnsignedInteger basisDimension = enumerateFunction.getBasisSizeFromTotalDegree(degree);
  FixedStrategy adaptiveStrategy(productBasis, basisDimension);
  // Create the result object here in order to test the save/load mechanism outside of the double loop
  FunctionalChaosResult result;
  // Create the projection strategy
  UnsignedInteger samplingSize = 500;
  // Full: ProjectionStrategy projectionStrategy{LeastSquaresStrategy()};
  // Sparse:
  FittingAlgorithm fittingAlgorithm = CorrectedLeaveOneOut();
  const LeastSquaresStrategy projectionStrategy(LeastSquaresMetaModelSelectionFactory(LARS(), fittingAlgorithm));

  MonteCarloExperiment experiment(distribution, samplingSize);
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
  FunctionalChaosResult ceGivenX1(result.getConditionalExpectation({0}));
  fullprint << ceGivenX1.__str__() << std::endl;
  Function functionCEGivenX1(ceGivenX1.getMetaModel());
  // Exact result Y | Xi1
  fullprint << "    Exact result Y | Xi1" << std::endl;
  Description inputVariablesGivenX1(3);
  inputVariablesGivenX1[0] = "xi1";
  inputVariablesGivenX1[1] = "a";
  inputVariablesGivenX1[2] = "b";
  Description formulaGivenX1(1);
  formulaGivenX1[0] = (OSS() << "a / 2 + (1 + b * pi_^4 / 5) * sin(xi1)");
  SymbolicFunction parametricEgivenX1(inputVariablesGivenX1, formulaGivenX1);
  Indices indicesABGivenX1({1, 2});
  Point parametersABGivenX1({a, b});
  ParametricFunction functionEgivenX1Exact(parametricEgivenX1, indicesABGivenX1, parametersABGivenX1);
  // Compute L2 weighted norm
  fullprint << "    Compute L2 weighted norm" << std::endl;
  Distribution distributionMarginalGivenX1(distribution.getMarginal(0));
  MonteCarloExperiment experimentTestGivenX1(distributionMarginalGivenX1, errorSampleSize);
  ExperimentIntegration integrationX1(experimentTestGivenX1);
  Point errorGivenX1(integrationX1.computeL2Norm(functionCEGivenX1 - functionEgivenX1Exact));
  fullprint << "    Error = " << errorGivenX1[0] << std::endl;
  assert(errorGivenX1[0] < atol);

  // Condition with respect to X2
  fullprint << "Condition with respect to X2" << std::endl;
  FunctionalChaosResult ceGivenX2(result.getConditionalExpectation({1}));
  fullprint << ceGivenX2.__str__() << std::endl;
  Function functionCEGivenX2(ceGivenX2.getMetaModel());
  // Exact result Y | Xi2
  fullprint << "    Exact result Y | Xi2" << std::endl;
  Description inputVariablesGivenX2(3);
  inputVariablesGivenX2[0] = "xi2";
  inputVariablesGivenX2[1] = "a";
  inputVariablesGivenX2[2] = "b";
  Description formula2GivenX2(1);
  formula2GivenX2[0] = (OSS() << "a * sin(xi2)^2");
  SymbolicFunction parametricEgivenX2(inputVariablesGivenX2, formula2GivenX2);
  Indices indicesABGivenX2({1, 2});
  Point parametersABGivenX2({a, b});
  ParametricFunction functionEgivenX2Exact(parametricEgivenX2, indicesABGivenX2, parametersABGivenX2);
  // Compute L2 weighted norm
  fullprint << "    Compute L2 weighted norm" << std::endl;
  Distribution distributionMarginalGivenX2(distribution.getMarginal(1));
  MonteCarloExperiment experimentTestGivenX2(distributionMarginalGivenX2, errorSampleSize);
  ExperimentIntegration integrationX2(experimentTestGivenX2);
  Point errorGivenX2(integrationX2.computeL2Norm(functionCEGivenX2 - functionEgivenX2Exact));
  fullprint << "    Error = " << errorGivenX2[0] << std::endl;
  assert(errorGivenX2[0] < atol);

  // Condition with respect to X3
  fullprint << "Condition with respect to X3" << std::endl;
  FunctionalChaosResult ceGivenX3(result.getConditionalExpectation({2}));
  fullprint << ceGivenX3.__str__() << std::endl;
  Function functionCEGivenX3(ceGivenX3.getMetaModel());
  // Exact result Y | Xi3
  fullprint << "    Exact result Y | Xi3" << std::endl;
  Description inputVariablesGivenX3(3);
  inputVariablesGivenX3[0] = "xi3";
  inputVariablesGivenX3[1] = "a";
  inputVariablesGivenX3[2] = "b";
  Description formula2GivenX3(1);
  formula2GivenX3[0] = (OSS() << "a / 2");
  SymbolicFunction parametricEgivenX3(inputVariablesGivenX3, formula2GivenX3);
  Indices indicesABGivenX3({1, 2});
  Point parametersABGivenX3({a, b});
  ParametricFunction functionEgivenX3Exact(parametricEgivenX3, indicesABGivenX3, parametersABGivenX3);
  // Compute L2 weighted norm
  fullprint << "    Compute L2 weighted norm" << std::endl;
  Distribution distributionMarginalGivenX3(distribution.getMarginal(2));
  MonteCarloExperiment experimentTestGivenX3(distributionMarginalGivenX3, errorSampleSize);
  ExperimentIntegration integrationX3(experimentTestGivenX3);
  Point errorGivenX3(integrationX3.computeL2Norm(functionCEGivenX3 - functionEgivenX3Exact));
  fullprint << "    Error = " << errorGivenX3[0] << std::endl;
  assert(errorGivenX3[0] < atol);

  return ExitCode::Success;
}
