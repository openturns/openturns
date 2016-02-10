//                                               -*- C++ -*-
/**
 *  @brief The test file of MonteCarlo class
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

NumericalScalar sobol(const Indices & indices,
                      const NumericalPoint & a)
{
  NumericalScalar value(1.0);
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
    UnsignedInteger inputDimension(3);
    UnsignedInteger outputDimension(2);

    // Reference analytical values
    NumericalScalar meanTh_Sobol(1.0);
    NumericalScalar covTh_Sobol(1.0);
    NumericalPoint kappa(inputDimension);
    NumericalScalar a(7.0);
    NumericalScalar b(0.1);
    // Create the gSobol function
    Description inputVariables(inputDimension);
    Description outputVariables(outputDimension);
    outputVariables[0] = "y0";
    outputVariables[1] = "y1";
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
    NumericalScalar meanTh_Ishigami(a / 2);
    NumericalScalar covTh_Ishigami((pow(b, 2.0) * pow(M_PI, 8.0)) / 18.0 + (b * pow(M_PI, 4.0)) / 5.0 + (pow(a, 2.0)) / 8.0 + 1.0 / 2.0);
    NumericalPoint sob_1_Ishigami(3);
    sob_1_Ishigami[0] = (b * pow(M_PI, 4.0) / 5.0 + pow(b, 2.0) * pow(M_PI, 8.0) / 50.0 + 1.0 / 2.0) / covTh_Ishigami;
    sob_1_Ishigami[1] = (pow(a, 2.0) / 8.0) / covTh_Ishigami;
    sob_1_Ishigami[2] = 0.0;
    NumericalPoint sob_2_Ishigami(3);
    sob_2_Ishigami[0] = 0.0;
    sob_2_Ishigami[1] = (pow(b, 2.0) * pow(M_PI, 8.0) / 18.0 - pow(b, 2.0) * pow(M_PI, 8.0) / 50.0) / covTh_Ishigami;
    sob_2_Ishigami[2] = 0.0;
    NumericalPoint sob_3_Ishigami(1, 0.0);
    // Multidimensional reference values
    // Mean
    NumericalPoint meanTh(outputDimension);
    meanTh[0] = meanTh_Sobol;
    meanTh[1] = meanTh_Ishigami;
    // Covariance
    CovarianceMatrix covTh(outputDimension);
    covTh(0, 0) = covTh_Sobol;
    covTh(1, 1) = covTh_Ishigami;
    // 1rst order Sobol
    NumericalPoint sob_1(inputDimension * outputDimension);
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
    NumericalPoint sob_2(inputDimension * outputDimension);
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
    NumericalPoint sob_3(outputDimension);
    {
      Indices indices(3);
      indices[0] = 0;
      indices[1] = 1;
      indices[2] = 2;
      sob_3[0] = sobol(indices, kappa) / covTh_Sobol;
      sob_3[1] = sob_3_Ishigami[0];
    }
    // 1rst order Total Sobol
    NumericalPoint sob_T1(inputDimension * outputDimension);
    sob_T1[0] = sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0];
    sob_T1[1] = sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0];
    sob_T1[2] = sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0];
    sob_T1[3] = sob_1[3] + sob_2[3] + sob_2[4] + sob_3[1];
    sob_T1[4] = sob_1[4] + sob_2[3] + sob_2[5] + sob_3[1];
    sob_T1[5] = sob_1[5] + sob_2[4] + sob_2[5] + sob_3[1];
    NumericalPoint sob_T2(inputDimension * outputDimension);
    sob_T2[0] = sob_2[0] + sob_3[0];
    sob_T2[1] = sob_2[1] + sob_3[0];
    sob_T2[2] = sob_2[2] + sob_3[0];
    sob_T2[3] = sob_2[3] + sob_3[1];
    sob_T2[4] = sob_2[4] + sob_3[1];
    sob_T2[5] = sob_2[5] + sob_3[1];
    // 3rd order Total Sobol
    NumericalPoint sob_T3(sob_3);
    NumericalMathFunction model(inputVariables, outputVariables, formula);

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
    EnumerateFunction enumerateFunction(inputDimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Create the adaptive strategy
    // We can choose amongst several strategies
    // First, the most efficient (but more complex!) strategy
    Collection<AdaptiveStrategy> listAdaptiveStrategy(0);
    UnsignedInteger degree(6);
    UnsignedInteger indexMax(enumerateFunction.getStrataCumulatedCardinal(degree));
    UnsignedInteger basisDimension(enumerateFunction.getStrataCumulatedCardinal(degree / 2));
    NumericalScalar threshold(1.0e-6);
    listAdaptiveStrategy.add(CleaningStrategy(productBasis, indexMax, basisDimension, threshold, false));
    // Second, the most used (and most basic!) strategy
    listAdaptiveStrategy.add(FixedStrategy(productBasis, enumerateFunction.getStrataCumulatedCardinal(degree)));

    for(UnsignedInteger adaptiveStrategyIndex = 0; adaptiveStrategyIndex < listAdaptiveStrategy.getSize(); ++adaptiveStrategyIndex)
    {
      AdaptiveStrategy adaptiveStrategy(listAdaptiveStrategy[adaptiveStrategyIndex]);
      // Create the projection strategy
      UnsignedInteger samplingSize(250);
      Collection<ProjectionStrategy> listProjectionStrategy(0);
      // LHS experiment
      listProjectionStrategy.add(LeastSquaresStrategy(LHSExperiment(samplingSize)));
      for(UnsignedInteger projectionStrategyIndex = 0; projectionStrategyIndex < listProjectionStrategy.getSize(); ++projectionStrategyIndex)
      {
        ProjectionStrategy projectionStrategy(listProjectionStrategy[projectionStrategyIndex]);
        // Create the polynomial chaos algorithm
        NumericalScalar maximumResidual(1.0e-10);
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
        for (UnsignedInteger outputIndex = 0; outputIndex < outputDimension; ++outputIndex)
        {
          fullprint << "output=" << outputIndex << std::endl;
          NumericalScalar mean(vector.getMean()[outputIndex]);
          fullprint << "mean=" << std::fixed << std::setprecision(5) << mean << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(mean - meanTh[outputIndex]) << std::endl;
          NumericalScalar variance(vector.getCovariance()(outputIndex, outputIndex));
          fullprint << "variance=" << std::fixed << std::setprecision(5) << variance << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(variance - covTh(outputIndex, outputIndex)) << std::endl;
          Indices indices(1);
          for(UnsignedInteger i = 0; i < inputDimension; ++i)
          {
            indices[0] = i;
            NumericalScalar value(vector.getSobolIndex(i, outputIndex));
            fullprint << "Sobol index " << i << " = " << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_1[i + inputDimension * outputIndex]) << std::endl;
          }
          indices = Indices(2);
          UnsignedInteger k(0);
          for (UnsignedInteger i = 0; i < inputDimension; ++i)
          {
            indices[0] = i;
            for (UnsignedInteger j = i + 1; j < inputDimension; ++j)
            {
              indices[1] = j;
              NumericalScalar value(vector.getSobolIndex(indices, outputIndex));
              fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_2[k + inputDimension * outputIndex]) << std::endl;
              k = k + 1;
            } // j
          } // i
          indices = Indices(3);
          indices[0] = 0;
          indices[1] = 1;
          indices[2] = 2;
          NumericalScalar value(vector.getSobolIndex(indices, outputIndex));
          fullprint << "Sobol index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_3[outputIndex]) << std::endl;
          for (UnsignedInteger i = 0; i < inputDimension; ++i)
          {
            NumericalScalar value(vector.getSobolTotalIndex(i, outputIndex));
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
              NumericalScalar value(vector.getSobolTotalIndex(indices, outputIndex));
              fullprint << "Sobol total index " << indices << " =" << std::fixed << std::setprecision(5) << value << " absolute error=" << std::scientific << std::setprecision(1) << std::abs(value - sob_T2[k + inputDimension * outputIndex]) << std::endl;
              k = k + 1;
            }
          }
          indices = Indices(3);
          indices[0] = 0;
          indices[1] = 1;
          indices[2] = 2;
          value = vector.getSobolTotalIndex(indices, outputIndex);
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
