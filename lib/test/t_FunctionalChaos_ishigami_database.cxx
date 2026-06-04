//                                               -*- C++ -*-
/**
 *  @brief The test file of the FunctionalChaosAlgorithm class
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/IshigamiUseCase.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    // Problem parameters
    IshigamiUseCase ishigami;
    UnsignedInteger dimension = 3;
    Function model(ishigami.getModel());  // Create the Ishigami function
    JointDistribution distribution(ishigami.getInputDistribution());  // Create the input distribution
    // Reference analytical values
    Scalar meanTh = ishigami.getMean();
    Scalar covTh = ishigami.getVariance();

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
    listAdaptiveStrategy.add(CleaningStrategy(productBasis, indexMax, basisDimension, threshold));
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

        // Post-process the results
        FunctionalChaosRandomVector vector(result);
        Scalar mean = vector.getMean()[0];
        fullprint << "mean=" << std::fixed << std::setprecision(5) << mean << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(mean - meanTh) << std::endl;
        Scalar variance = vector.getCovariance()(0, 0);
        fullprint << "variance=" << std::fixed << std::setprecision(5) << variance << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(variance - covTh) << std::endl;
        FunctionalChaosSobolIndices sensitivity(result);
        // Check first order indices
        const Point referenceFirstOrderSobolIndices(ishigami.getFirstOrderSobolIndices());
        for(UnsignedInteger i = 0; i < dimension; ++i)
        {
          Scalar value = sensitivity.getSobolIndex(i);
          fullprint << "Sobol index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - referenceFirstOrderSobolIndices[i]) << std::endl;
        }
        // Check first order interaction indices (Xi, Xj)
        Indices indices(2);
        UnsignedInteger k = 0;
        for (UnsignedInteger i = 0; i < dimension; ++i)
        {
          indices[0] = i;
          for (UnsignedInteger j = i + 1; j < dimension; ++j)
          {
            indices[1] = j;
            Scalar value = sensitivity.getSobolIndex(indices);
            const Scalar referenceValue = ishigami.getFirstOrderInteractionSobolIndex(indices);
            fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - referenceValue) << std::endl;
            k = k + 1;
          }
        }
        // Check first order interaction indices (Xi, Xj, Xk)
        indices = Indices(3);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        Scalar value = sensitivity.getSobolIndex(indices);
        const Scalar referenceValue1 = ishigami.getFirstOrderInteractionSobolIndex(indices);
        fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - referenceValue1) << std::endl;
        // Check total indices
        const Point referenceTotalSobolIndices(ishigami.getTotalSobolIndices());
        for (UnsignedInteger i = 0; i < dimension; ++i)
        {
          value = sensitivity.getSobolTotalIndex(i);
          fullprint << "Sobol total index " << i << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - referenceTotalSobolIndices[i]) << std::endl;
        }
        // Check total interaction indices (Xi, Xj)
        indices = Indices(2);
        k = 0;
        for (UnsignedInteger i = 0; i < dimension; ++i)
        {
          indices[0] = i;
          for (UnsignedInteger j = i + 1; j < dimension; ++j)
          {
            indices[1] = j;
            value = sensitivity.getSobolTotalIndex(indices);
            const Scalar referenceValue = ishigami.getTotalInteractionSobolIndex(indices);
            fullprint << "Sobol total index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - referenceValue) << std::endl;
            k = k + 1;
          }
        }
        // Check total interaction indices (Xi, Xj, Xk)
        indices = Indices(3);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        value = sensitivity.getSobolTotalIndex(indices);
        const Scalar referenceValue2 = ishigami.getTotalInteractionSobolIndex(indices);
        fullprint << "Sobol total index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - referenceValue2) << std::endl;
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
