//                                               -*- C++ -*-
/**
 *  @brief The test file of the FunctionalChaosAlgorithm class
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

  try
  {

    // Problem parameters
    UnsignedInteger dimension = 3;
    Scalar a = 7.0;
    Scalar b = 0.1;
    // Reference analytical values
    Scalar meanTh = a / 2;
    Scalar covTh = (pow(b, 2.0) * pow(M_PI, 8.0)) / 18.0 + (b * pow(M_PI, 4.0)) / 5.0 + (pow(a, 2.0)) / 8.0 + 1.0 / 2.0;
    Point sob_1(3);
    sob_1[0] = (b * pow(M_PI, 4.0) / 5.0 + pow(b, 2.0) * pow(M_PI, 8.0) / 50.0 + 1.0 / 2.0) / covTh;
    sob_1[1] = (pow(a, 2.0) / 8.0) / covTh;
    sob_1[2] = 0.0;
    Point sob_2(3);
    sob_2[0] = 0.0;
    sob_2[1] = (pow(b, 2.0) * pow(M_PI, 8.0) / 18.0 - pow(b, 2.0) * pow(M_PI, 8.0) / 50.0) / covTh;
    sob_2[2] = 0.0;
    Point sob_3(1, 0.0);
    Point sob_T1(3);
    sob_T1[0] = sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0];
    sob_T1[1] = sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0];
    sob_T1[2] = sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0];
    Point sob_T2(3);
    sob_T2[0] = sob_2[0] + sob_2[1] + sob_3[0];
    sob_T2[1] = sob_2[0] + sob_2[2] + sob_3[0];
    sob_T2[2] = sob_2[1] + sob_2[2] + sob_3[0];
    Point sob_T3(sob_3);
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

    // Create the projection strategy
    UnsignedInteger samplingSize = 250;
    Collection<ProjectionStrategy> listProjectionStrategy(0);
    // Monte Carlo sampling
    Sample inputSample(LowDiscrepancyExperiment(LowDiscrepancySequence(SobolSequence()), distribution, samplingSize).generate());
    Sample outputSample(model(inputSample));
    // From here, the model is no more needed
    listProjectionStrategy.add(LeastSquaresStrategy());
    listProjectionStrategy.add(LeastSquaresStrategy(LeastSquaresMetaModelSelectionFactory (LARS(), CorrectedLeaveOneOut())));
    listProjectionStrategy.add(IntegrationStrategy());
    // Create the adaptive strategy
    // We can choose amongst several strategies
    // First, the most efficient (but more complex!) strategy
    UnsignedInteger degree = 6;
    Collection<AdaptiveStrategy> listAdaptiveStrategy(0);
    UnsignedInteger indexMax = enumerateFunction.getStrataCumulatedCardinal(degree);
    UnsignedInteger basisDimension = enumerateFunction.getStrataCumulatedCardinal(degree / 2);
    Scalar threshold = 1.0e-6;
    listAdaptiveStrategy.add(CleaningStrategy(productBasis, indexMax, basisDimension, threshold, true));
    // Second, the most used (and most basic!) strategy
    listAdaptiveStrategy.add(FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree)));
    for(UnsignedInteger adaptiveStrategyIndex = 0; adaptiveStrategyIndex < listAdaptiveStrategy.getSize(); ++adaptiveStrategyIndex)
    {
      AdaptiveStrategy adaptiveStrategy(listAdaptiveStrategy[adaptiveStrategyIndex]);
      for(UnsignedInteger projectionStrategyIndex = 0; projectionStrategyIndex < listProjectionStrategy.getSize(); ++projectionStrategyIndex)
      {
        ProjectionStrategy projectionStrategy(listProjectionStrategy[projectionStrategyIndex]);
        // Create the polynomial chaos algorithm
        Scalar maximumResidual = 1.0e-10;
        FunctionalChaosAlgorithm algo(inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy);
        algo.setMaximumResidual(maximumResidual);
        // Reinitialize the RandomGenerator to see the effect of the sampling method only
        RandomGenerator::SetSeed(0);
        algo.run();

        // Examine the results
        FunctionalChaosResult result(algo.getResult());
        fullprint << "//////////////////////////////////////////////////////////////////////" << std::endl;
        fullprint << adaptiveStrategy << std::endl;
        fullprint << algo.getProjectionStrategy() << std::endl;
        Point residuals(result.getResiduals());
        fullprint << "residuals=" << std::fixed << std::setprecision(5) << residuals << std::endl;
        Point relativeErrors(result.getRelativeErrors());
        fullprint << "relative errors=" << std::fixed << std::setprecision(5) << relativeErrors << std::endl;

        // Post-process the results
        FunctionalChaosRandomVector vector(result);
        Scalar mean = vector.getMean()[0];
        fullprint << "mean=" << std::fixed << std::setprecision(5) << mean << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(mean - meanTh) << std::endl;
        Scalar variance = vector.getCovariance()(0, 0);
        fullprint << "variance=" << std::fixed << std::setprecision(5) << variance << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(variance - covTh) << std::endl;
        FunctionalChaosSobolIndices sensitivity(result);
        for(UnsignedInteger i = 0; i < dimension; ++i)
        {
          Scalar value = sensitivity.getSobolIndex(i);
          fullprint << "Sobol index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_1[i]) << std::endl;
        }
        Indices indices(2);
        UnsignedInteger k = 0;
        for (UnsignedInteger i = 0; i < dimension; ++i)
        {
          indices[0] = i;
          for (UnsignedInteger j = i + 1; j < dimension; ++j)
          {
            indices[1] = j;
            Scalar value = sensitivity.getSobolIndex(indices);
            fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_2[k]) << std::endl;
            k = k + 1;
          }
        }
        indices = Indices(3);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        Scalar value = sensitivity.getSobolIndex(indices);
        fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_3[0]) << std::endl;
        for (UnsignedInteger i = 0; i < dimension; ++i)
        {
          value = sensitivity.getSobolTotalIndex(i);
          fullprint << "Sobol total index " << i << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_T1[i]) << std::endl;
        }
        indices = Indices(2);
        k = 0;
        for (UnsignedInteger i = 0; i < dimension; ++i)
        {
          indices[0] = i;
          for (UnsignedInteger j = i + 1; j < dimension; ++j)
          {
            indices[1] = j;
            value = sensitivity.getSobolIndex(indices);
            fullprint << "Sobol total index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_2[k]) << std::endl;
            k = k + 1;
          }
        }
        indices = Indices(3);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        value = sensitivity.getSobolTotalIndex(indices);
        fullprint << "Sobol total index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_3[0]) << std::endl;
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
