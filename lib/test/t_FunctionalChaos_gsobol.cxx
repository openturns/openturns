//                                               -*- C++ -*-
/**
 *  @brief The test file of FunctionalChaosAlgorithm class
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

NumericalScalar sobol(const Indices & indices,
                      const NumericalPoint & a)
{
  NumericalScalar value = 1.0;
  for (UnsignedInteger i = 0; i < indices.getSize(); ++i)
  {
    value *= 1.0 / (3.0 * pow(1.0 + a[indices[i]], 2.0));
  }
  return value;
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    // Problem parameters
    UnsignedInteger dimension = 5;

    // Reference analytical values
    NumericalScalar meanTh = 1.0;
    NumericalScalar covTh = 1.0;
    NumericalPoint a(dimension);
    // Create the gSobol function
    Description inputVariables(dimension);
    Description outputVariables(1);
    outputVariables[0] = "y";
    Description formula(1);
    formula[0] = "1.0";
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      a[i] = 0.5 * i;
      covTh *= 1.0 + 1.0 / (3.0 * pow(1.0 + a[i], 2.0));
      inputVariables[i] = (OSS() << "xi" << i);
      formula[0] = (OSS() << formula[0] << " * ((abs(4.0 * xi" << i << " - 2.0) + " << a[i] << ") / (1.0 + " << a[i] << "))");
    }
    --covTh;

    NumericalMathFunction model(inputVariables, outputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginals(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      marginals[i] = Uniform(0.0, 1.0);
    }
    ComposedDistribution distribution(marginals);

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      polynomialCollection[i] = LegendreFactory();
    }
    EnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Create the adaptive strategy
    // We can choose amongst several strategies
    // First, the most efficient (but more complex!) strategy
    Collection<AdaptiveStrategy> listAdaptiveStrategy(0);
    UnsignedInteger degree = 4;
    UnsignedInteger indexMax = enumerateFunction.getStrataCumulatedCardinal(degree);
    UnsignedInteger basisDimension = enumerateFunction.getStrataCumulatedCardinal(degree / 2);
    NumericalScalar threshold = 1.0e-6;
    listAdaptiveStrategy.add(CleaningStrategy(productBasis, indexMax, basisDimension, threshold, false));
    // Second, the most used (and most basic!) strategy
    listAdaptiveStrategy.add(FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree)));
    // Third, a slight enhancement with respect to the basic strategy
    listAdaptiveStrategy.add(SequentialStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree / 2), false));

    for(UnsignedInteger adaptiveStrategyIndex = 0; adaptiveStrategyIndex < listAdaptiveStrategy.getSize(); ++adaptiveStrategyIndex)
    {
      AdaptiveStrategy adaptiveStrategy(listAdaptiveStrategy[adaptiveStrategyIndex]);
      // Create the projection strategy
      UnsignedInteger samplingSize = 250;
      Collection<ProjectionStrategy> listProjectionStrategy(0);
      // The least squares strategy
      // Monte Carlo sampling
      listProjectionStrategy.add(LeastSquaresStrategy(MonteCarloExperiment(samplingSize)));
      // LHS sampling
      listProjectionStrategy.add(LeastSquaresStrategy(LHSExperiment(samplingSize)));
      // Low Discrepancy sequence
      listProjectionStrategy.add(LeastSquaresStrategy(LowDiscrepancyExperiment(LowDiscrepancySequence(SobolSequence()), samplingSize)));
      // The integration strategy
      // Monte Carlo sampling
      listProjectionStrategy.add(IntegrationStrategy(MonteCarloExperiment(samplingSize)));
      // LHS sampling
      listProjectionStrategy.add(IntegrationStrategy(LHSExperiment(samplingSize)));
      // Low Discrepancy sequence
      listProjectionStrategy.add(IntegrationStrategy(LowDiscrepancyExperiment(LowDiscrepancySequence(SobolSequence()), samplingSize)));
      for(UnsignedInteger projectionStrategyIndex = 0; projectionStrategyIndex < listProjectionStrategy.getSize(); ++projectionStrategyIndex)
      {
        ProjectionStrategy projectionStrategy(listProjectionStrategy[projectionStrategyIndex]);
        // Create the polynomial chaos algorithm
        NumericalScalar maximumResidual = 1.0e-10;
        FunctionalChaosAlgorithm algo(model, distribution, adaptiveStrategy, projectionStrategy);
        algo.setMaximumResidual(maximumResidual);
        // Reinitialize the RandomGenerator to see the effect of the sampling method only
        RandomGenerator::SetSeed(0);

        algo.run();

        // Examine the results
        FunctionalChaosResult result(algo.getResult());
        fullprint << "//////////////////////////////////////////////////////////////////////" << std::endl;
        fullprint << adaptiveStrategy << std::endl;
        fullprint << projectionStrategy << std::endl;
        NumericalPoint residuals(result.getResiduals());
        fullprint << "residuals=" << std::fixed << std::setprecision(5) << residuals << std::endl;
        NumericalPoint relativeErrors(result.getRelativeErrors());
        fullprint << "relative errors=" << std::fixed << std::setprecision(5) << relativeErrors << std::endl;

        // Post-process the results
        FunctionalChaosRandomVector vector(result);
        NumericalScalar mean = vector.getMean()[0];
        fullprint << "mean=" << std::fixed << std::setprecision(5) << mean << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(mean - meanTh) << std::endl;
        NumericalScalar variance = vector.getCovariance()(0, 0);
        fullprint << "variance=" << std::fixed << std::setprecision(5) << variance << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(variance - covTh) << std::endl;
        Indices indices(1);
        for(UnsignedInteger i = 0; i < dimension; ++i)
        {
          indices[0] = i;
          NumericalScalar value = vector.getSobolIndex(i);
          fullprint << "Sobol index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sobol(indices, a) / covTh) << std::endl;
        }
        indices = Indices(2);
        UnsignedInteger k = 0;
        for (UnsignedInteger i = 0; i < dimension; ++i)
        {
          indices[0] = i;
          for (UnsignedInteger j = i + 1; j < dimension; ++j)
          {
            indices[1] = j;
            NumericalScalar value = vector.getSobolIndex(indices);
            fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sobol(indices, a) / covTh) << std::endl;
            k = k + 1;
          }
        }
        indices = Indices(3);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        NumericalScalar value = vector.getSobolIndex(indices);
        fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sobol(indices, a) / covTh) << std::endl;
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
