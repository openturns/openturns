//                                               -*- C++ -*-
/**
 *  @brief The test file of FunctionalChaosAlgoritm class
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
  setRandomGenerator();

  try
  {

    IshigamiUseCase ishigami;
    Function model(ishigami.getModel());
    JointDistribution distribution(ishigami.getInputDistribution());
    
    // Problem parameters
    UnsignedInteger dimension = 3;
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
    fullprint << productBasis.__str__() << std::endl;

    // Create the adaptive strategy
    // We can choose amongst several strategies
    // First, the most efficient (but more complex!) strategy
    Collection<AdaptiveStrategy> listAdaptiveStrategy(0);
    UnsignedInteger degree = 6;
    UnsignedInteger indexMax = enumerateFunction.getStrataCumulatedCardinal(degree);
    UnsignedInteger basisDimension = enumerateFunction.getStrataCumulatedCardinal(degree / 2);
    Scalar threshold = 1.0e-6;
    listAdaptiveStrategy.add(CleaningStrategy(productBasis, indexMax, basisDimension, threshold));
    // Second, the most used (and most basic!) strategy
    listAdaptiveStrategy.add(FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree)));
    // Create the result object here in order to test the save/load mechanism outside of the double loop
    FunctionalChaosResult result;

    for(UnsignedInteger adaptiveStrategyIndex = 0; adaptiveStrategyIndex < listAdaptiveStrategy.getSize(); ++adaptiveStrategyIndex)
    {
      AdaptiveStrategy adaptiveStrategy(listAdaptiveStrategy[adaptiveStrategyIndex]);
      // Create the projection strategy
      UnsignedInteger samplingSize = 250;
      Collection<ProjectionStrategy> listProjectionStrategy(0);
      listProjectionStrategy.add(LeastSquaresStrategy());
      listProjectionStrategy.add(IntegrationStrategy());

      for(UnsignedInteger projectionStrategyIndex = 0; projectionStrategyIndex < listProjectionStrategy.getSize(); ++projectionStrategyIndex)
      {
        ProjectionStrategy projectionStrategy(listProjectionStrategy[projectionStrategyIndex]);

        Collection<WeightedExperiment> listExperiment(0);
        listExperiment.add(MonteCarloExperiment(distribution, samplingSize));
        listExperiment.add(LHSExperiment(distribution, samplingSize));
        listExperiment.add(LowDiscrepancyExperiment(SobolSequence(), distribution, samplingSize));

        for(UnsignedInteger experimentIndex = 0; experimentIndex < listExperiment.getSize(); ++ experimentIndex)
        {
          WeightedExperiment experiment(listExperiment[experimentIndex]);
          RandomGenerator::SetSeed(0);
          const Sample X(experiment.generate());
          const Sample Y(model(X));

          // Create the polynomial chaos algorithm
          Scalar maximumResidual = 1.0e-10;
          FunctionalChaosAlgorithm algo(X, Y, distribution, adaptiveStrategy, projectionStrategy);
          algo.setMaximumResidual(maximumResidual);
          algo.run();

          // Examine the results
          result = algo.getResult();
          fullprint << result.__str__() << std::endl;
          fullprint << result.__repr_markdown__() << std::endl;
          fullprint << "//////////////////////////////////////////////////////////////////////" << std::endl;
          fullprint << algo.getAdaptiveStrategy() << std::endl;
          fullprint << algo.getProjectionStrategy() << std::endl;
          Point residuals(result.getResiduals());
          fullprint << "residuals=" << std::fixed << std::setprecision(5) << residuals << std::endl;
          Point relativeErrors(result.getRelativeErrors());
          fullprint << "relative errors=" << std::fixed << std::setprecision(5) << relativeErrors << std::endl;
          Bool isLeastSquaresPCE = result.isLeastSquares();
          Bool isLeastSquaresProjection = projectionStrategy.isLeastSquares();
          assert_equal(isLeastSquaresProjection, isLeastSquaresPCE);
          Bool involvesModelSelectionPCE = result.involvesModelSelection();
          Bool involvesModelSelection = projectionStrategy.involvesModelSelection() || adaptiveStrategy.getImplementation()->involvesModelSelection();
          assert_equal(involvesModelSelection, involvesModelSelectionPCE);

          // Post-process the results
          FunctionalChaosRandomVector vector(result);
          Scalar mean = vector.getMean()[0];
          fullprint << "mean=" << std::fixed << std::setprecision(5) << mean << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(mean - meanTh) << std::endl;
          Scalar variance = vector.getCovariance()(0, 0);
          fullprint << "variance=" << std::fixed << std::setprecision(5) << variance << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(variance - covTh) << std::endl;
          
          // Check Sobol' indices
          // Check first order indices
          FunctionalChaosSobolIndices sensitivity(result);
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
          indices.fill();
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
          indices.fill();
          value = sensitivity.getSobolTotalIndex(indices);
          const Scalar referenceValue2 = ishigami.getTotalInteractionSobolIndex(indices);
          fullprint << "Sobol total index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - referenceValue2) << std::endl;
          // Print part of variance
          // If the exact coefficients of the PCE on Legendre polynomial of the Ishigami function
          // were known, we could check.
          const Point partOfVariance(sensitivity.getPartOfVariance());
          fullprint << "Part of variance" << std::endl;
          indices = result.getIndices();
          const UnsignedInteger basisSize = indices.getSize();
          for (UnsignedInteger i = 0; i < basisSize; ++i)
          {
            UnsignedInteger globalIndex = indices[i];
            Indices multiIndex(enumerateFunction(globalIndex));
            if (partOfVariance[i] > 1.e-3)
            {
              fullprint << i << " " << globalIndex << " " << multiIndex << " "
                        << std::fixed << std::setprecision(4) << partOfVariance[i]
                        << std::endl;
            }
          } // loop over the multi-indices
          // Print summary
          fullprint << "Summary" << std::endl;
          fullprint << sensitivity.__str__() << std::endl;

          // Convert to LinearModelResult
          const LinearModelResult lmResult(result.getLinearModelResult());
          // Check the coefficients
          const Point coefficientsPCE(result.getCoefficients().getMarginal(0).asPoint());
          const Point coefficientsLM(lmResult.getCoefficients());
          const Scalar rtol = 1.e-15;
          assert_almost_equal(coefficientsPCE, coefficientsLM, rtol);
          // Check the metamodel
          const Function metamodelPCE(result.getMetaModel());
          const Sample Xtest(experiment.generate());
          const Sample YtestPCE(metamodelPCE(Xtest));
          const Function metamodelLM(lmResult.getMetaModel());
          const Function transformation(result.getTransformation());
          const Sample Ztest(transformation(Xtest));
          const Sample YtestLM(metamodelLM(Ztest));
          assert_almost_equal(YtestPCE, YtestLM, rtol);
          
        }
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
