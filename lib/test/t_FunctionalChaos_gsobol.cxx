//                                               -*- C++ -*-
/**
 *  @brief The test file of FunctionalChaosAlgorithm class
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
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

Scalar sobol(const Indices & indices,
             const Point & a)
{
  Scalar value = 1.0;
  for (UnsignedInteger i = 0; i < indices.getSize(); ++i)
  {
    value *= 1.0 / (3.0 * pow(1.0 + a[indices[i]], 2.0));
  }
  return value;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    // Problem parameters
    UnsignedInteger dimension = 5;

    // Reference analytical values
    Scalar meanTh = 1.0;
    Scalar covTh = 1.0;
    Point a(dimension);
    // Create the gSobol function
    Description inputVariables(dimension);
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

    SymbolicFunction model(inputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginals(dimension, Uniform(0.0, 1.0));
    JointDistribution distribution(marginals);

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      polynomialCollection[i] = LegendreFactory();
    }
    LinearEnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Create the adaptive strategy
    // We can choose amongst several strategies
    // First, the most efficient (but more complex!) strategy
    Collection<AdaptiveStrategy> listAdaptiveStrategy(0);
    UnsignedInteger degree = 4;
    UnsignedInteger indexMax = enumerateFunction.getStrataCumulatedCardinal(degree);
    UnsignedInteger basisDimension = enumerateFunction.getStrataCumulatedCardinal(degree / 2);
    Scalar threshold = 1.0e-6;
    listAdaptiveStrategy.add(CleaningStrategy(productBasis, indexMax, basisDimension, threshold));
    // Second, the most used (and most basic!) strategy
    listAdaptiveStrategy.add(FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree)));
    //

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
          FunctionalChaosResult result(algo.getResult());
          fullprint << "//////////////////////////////////////////////////////////////////////" << std::endl;
          fullprint << algo.getAdaptiveStrategy() << std::endl;
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
          Indices indices(1);
          for(UnsignedInteger i = 0; i < dimension; ++i)
          {
            indices[0] = i;
            Scalar value = sensitivity.getSobolIndex(i);
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
              Scalar value = sensitivity.getSobolIndex(indices);
              fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sobol(indices, a) / covTh) << std::endl;
              k = k + 1;
            }
          }
          indices = Indices(3);
          indices.fill();
          Scalar value = sensitivity.getSobolIndex(indices);
          fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sobol(indices, a) / covTh) << std::endl;

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
