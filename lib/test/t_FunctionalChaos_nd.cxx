//                                               -*- C++ -*-
/**
 *  @brief The test file of MonteCarlo class
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

inline Scalar sobol(const Indices & indices,
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
    UnsignedInteger inputDimension = 3;
    UnsignedInteger outputDimension = 2;

    // Reference analytical values
    Scalar meanTh_Sobol = 1.0;
    Scalar covTh_Sobol = 1.0;
    Point kappa(inputDimension);
    Scalar a = 7.0;
    Scalar b = 0.1;
    // Create the gSobol function
    Description inputVariables(inputDimension);
    Description formula(outputDimension);
    formula[0] = "1.0";
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      kappa[i] = 0.5 * i;
      covTh_Sobol *= 1.0 + 1.0 / (3.0 * pow(1.0 + kappa[i], 2.0));
      inputVariables[i] = (OSS() << "xi" << i);
      formula[0] = (OSS() << formula[0] << " * ((abs(4.0 * xi" << i << " - 2.0) + " << kappa[i] << ") / (1.0 + " << kappa[i] << "))");
    }
    formula[1] = (OSS() << "sin(" << -M_PI << " + 2 * " << M_PI << " * xi0) + (" << a << ") * (sin(" << -M_PI << " + 2 * " << M_PI << " * xi1)) ^ 2 + (" << b << ") * (" << -M_PI << " + 2 * " << M_PI << " * xi2)^4 * sin(" << -M_PI << " + 2 * " << M_PI << " * xi0)");
    --covTh_Sobol;
    // Reference analytical values
    Scalar meanTh_Ishigami = a / 2;
    Scalar covTh_Ishigami = (pow(b, 2.0) * pow(M_PI, 8.0)) / 18.0 + (b * pow(M_PI, 4.0)) / 5.0 + (pow(a, 2.0)) / 8.0 + 1.0 / 2.0;
    Point sob_1_Ishigami(3);
    sob_1_Ishigami[0] = (b * pow(M_PI, 4.0) / 5.0 + pow(b, 2.0) * pow(M_PI, 8.0) / 50.0 + 1.0 / 2.0) / covTh_Ishigami;
    sob_1_Ishigami[1] = (pow(a, 2.0) / 8.0) / covTh_Ishigami;
    sob_1_Ishigami[2] = 0.0;
    Point sob_2_Ishigami(3);
    sob_2_Ishigami[0] = 0.0;
    sob_2_Ishigami[1] = (pow(b, 2.0) * pow(M_PI, 8.0) / 18.0 - pow(b, 2.0) * pow(M_PI, 8.0) / 50.0) / covTh_Ishigami;
    sob_2_Ishigami[2] = 0.0;
    Point sob_3_Ishigami(1, 0.0);
    // Multidimensional reference values
    // Mean
    Point meanTh(outputDimension);
    meanTh[0] = meanTh_Sobol;
    meanTh[1] = meanTh_Ishigami;
    // Covariance
    CovarianceMatrix covTh(outputDimension);
    covTh(0, 0) = covTh_Sobol;
    covTh(1, 1) = covTh_Ishigami;
    // 1rst order Sobol
    Point sob_1(inputDimension * outputDimension);
    {
      Indices indices(1);
      indices[0] = 0;
      sob_1[0] = sobol(indices, kappa) / covTh_Sobol;
      indices[0] = 1;
      sob_1[1] = sobol(indices, kappa) / covTh_Sobol;
      indices[0] = 2;
      sob_1[2] = sobol(indices, kappa) / covTh_Sobol;
      sob_1[3] = sob_1_Ishigami[0];
      sob_1[4] = sob_1_Ishigami[1];
      sob_1[5] = sob_1_Ishigami[2];
    }
    // 2nd order Sobol
    Point sob_2(inputDimension * outputDimension);
    {
      Indices indices(2);
      indices[0] = 0;
      indices[1] = 1;
      sob_2[0] = sobol(indices, kappa) / covTh_Sobol;
      indices[1] = 2;
      sob_2[1] = sobol(indices, kappa) / covTh_Sobol;
      indices[0] = 1;
      indices[1] = 2;
      sob_2[2] = sobol(indices, kappa) / covTh_Sobol;
      sob_2[3] = sob_2_Ishigami[0];
      sob_2[4] = sob_2_Ishigami[1];
      sob_2[5] = sob_2_Ishigami[2];
    }
    // 3rd order Sobol
    Point sob_3(outputDimension);
    {
      Indices indices(3);
      indices[0] = 0;
      indices[1] = 1;
      indices[2] = 2;
      sob_3[0] = sobol(indices, kappa) / covTh_Sobol;
      sob_3[1] = sob_3_Ishigami[0];
    }
    // 1rst order Total Sobol
    Point sob_T1(inputDimension * outputDimension);
    sob_T1[0] = sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0];
    sob_T1[1] = sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0];
    sob_T1[2] = sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0];
    sob_T1[3] = sob_1[3] + sob_2[3] + sob_2[4] + sob_3[1];
    sob_T1[4] = sob_1[4] + sob_2[3] + sob_2[5] + sob_3[1];
    sob_T1[5] = sob_1[5] + sob_2[4] + sob_2[5] + sob_3[1];
    Point sob_T2(inputDimension * outputDimension);
    sob_T2[0] = sob_2[0] + sob_3[0];
    sob_T2[1] = sob_2[1] + sob_3[0];
    sob_T2[2] = sob_2[2] + sob_3[0];
    sob_T2[3] = sob_2[3] + sob_3[1];
    sob_T2[4] = sob_2[4] + sob_3[1];
    sob_T2[5] = sob_2[5] + sob_3[1];
    // 3rd order Total Sobol
    Point sob_T3(sob_3);
    SymbolicFunction model(inputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginals(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i) marginals[i] = Uniform(0.0, 1.0);
    ComposedDistribution distribution(marginals);

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
    {
      polynomialCollection[i] = LegendreFactory();
    }
    LinearEnumerateFunction enumerateFunction(inputDimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

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

    for(UnsignedInteger adaptiveStrategyIndex = 0; adaptiveStrategyIndex < listAdaptiveStrategy.getSize(); ++adaptiveStrategyIndex)
    {
      AdaptiveStrategy adaptiveStrategy(listAdaptiveStrategy[adaptiveStrategyIndex]);
      // Create the projection strategy
      UnsignedInteger samplingSize = 250;
      Collection<WeightedExperiment> listExperiment(0);
      // LHS experiment
      listExperiment.add(LHSExperiment(distribution, samplingSize));
      for(UnsignedInteger experimentIndex = 0; experimentIndex < listExperiment.getSize(); ++ experimentIndex)
      {
        const WeightedExperiment experiment(listExperiment[experimentIndex]);
        RandomGenerator::SetSeed(0);
        const Sample X(experiment.generate());
        const Sample Y(model(X));
        // Create the polynomial chaos algorithm
        Scalar maximumResidual = 1.0e-10;
        ProjectionStrategy projectionStrategy{LeastSquaresStrategy()};
        FunctionalChaosAlgorithm algo(X, Y, distribution, adaptiveStrategy, projectionStrategy);
        algo.setMaximumResidual(maximumResidual);
        algo.run();

        // Examine the results
        FunctionalChaosResult result(algo.getResult());
        fullprint << result.__str__() << std::endl;
        fullprint << "//////////////////////////////////////////////////////////////////////" << std::endl;
        fullprint << algo.getAdaptiveStrategy() << std::endl;
        fullprint << algo.getProjectionStrategy() << std::endl;
        Point residuals(result.getResiduals());
        fullprint << "residuals=" << std::fixed << std::setprecision(5) << residuals << std::endl;
        Point relativeErrors(result.getRelativeErrors());
        fullprint << "relative errors=" << std::fixed << std::setprecision(5) << relativeErrors << std::endl;

        // Post-process the results
        FunctionalChaosRandomVector vector(result);
        FunctionalChaosSobolIndices sensitivity(result);
        for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
        {
          fullprint << "output=" << outputIndex << std::endl;
          Scalar mean = vector.getMean()[outputIndex];
          fullprint << "mean=" << std::fixed << std::setprecision(5) << mean << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(mean - meanTh[outputIndex]) << std::endl;
          Scalar variance = vector.getCovariance()(outputIndex, outputIndex);
          fullprint << "variance=" << std::fixed << std::setprecision(5) << variance << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(variance - covTh(outputIndex, outputIndex)) << std::endl;
          Indices indices(1);
          for(UnsignedInteger i = 0; i < inputDimension; ++i)
          {
            indices[0] = i;
            Scalar value = sensitivity.getSobolIndex(i, outputIndex);
            fullprint << "Sobol index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_1[i + inputDimension * outputIndex]) << std::endl;
          }
          indices = Indices(2);
          UnsignedInteger k = 0;
          for (UnsignedInteger i = 0; i < inputDimension; ++i)
          {
            indices[0] = i;
            for (UnsignedInteger j = i + 1; j < inputDimension; ++j)
            {
              indices[1] = j;
              Scalar value = sensitivity.getSobolIndex(indices, outputIndex);
              fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_2[k + inputDimension * outputIndex]) << std::endl;
              k = k + 1;
            } // j
          } // i
          indices = Indices(3);
          indices[0] = 0;
          indices[1] = 1;
          indices[2] = 2;
          Scalar value = sensitivity.getSobolIndex(indices, outputIndex);
          fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_3[outputIndex]) << std::endl;
          for (UnsignedInteger i = 0; i < inputDimension; ++i)
          {
            value = sensitivity.getSobolTotalIndex(i, outputIndex);
            fullprint << "Sobol total index " << i << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_T1[i + inputDimension * outputIndex]) << std::endl;
          }
          indices = Indices(2);
          k = 0;
          for (UnsignedInteger i = 0; i < inputDimension; ++i)
          {
            indices[0] = i;
            for (UnsignedInteger j = i + 1; j < inputDimension; ++j)
            {
              indices[1] = j;
              value = sensitivity.getSobolTotalIndex(indices, outputIndex);
              fullprint << "Sobol total index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_T2[k + inputDimension * outputIndex]) << std::endl;
              k = k + 1;
            }
          }
          indices = Indices(3);
          indices[0] = 0;
          indices[1] = 1;
          indices[2] = 2;
          value = sensitivity.getSobolTotalIndex(indices, outputIndex);
          fullprint << "Sobol total index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_T3[1]) << std::endl;

        } // outputIndex
      } // ProjectionStrategy
    } // AdaptiveStrategy
  } // try
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
