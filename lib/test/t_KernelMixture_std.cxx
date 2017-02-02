//                                               -*- C++ -*-
/**
 *  @brief The test file of class KernelMixture for standard methods
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();
  try
  {

    UnsignedInteger dimension = 3;
    NumericalPoint meanPoint(dimension, 1.0);
    meanPoint[0] = 0.5;
    meanPoint[1] = -0.5;
    NumericalPoint sigma(dimension, 1.0);
    sigma[0] = 2.0;
    sigma[1] = 3.0;

    NumericalSample sample(0, dimension);
    // Create a collection of distribution
    Mixture::DistributionCollection aCollection;

    aCollection.add( Normal(meanPoint, sigma, IdentityMatrix(dimension)) );
    sample.add(meanPoint);
    meanPoint += NumericalPoint(dimension, 1.0);
    aCollection.add( Normal(meanPoint, sigma, IdentityMatrix(dimension)) );
    sample.add(meanPoint);
    meanPoint += NumericalPoint(dimension, 1.0);
    aCollection.add( Normal(meanPoint, sigma, IdentityMatrix(dimension)) );
    sample.add(meanPoint);

    // Instanciate one distribution object
    Normal kernel;
    KernelMixture distribution(kernel, sigma, sample);
    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;

    Mixture distributionRef(aCollection);

    // Is this distribution elliptical ?
    fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

    // Is this distribution continuous ?
    fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

    // Test for realization of distribution
    NumericalPoint oneRealization = distribution.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 100;
    NumericalSample oneSample = distribution.getSample( size );
    fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
    fullprint << "mean=" << oneSample.computeMean() << std::endl;
    fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;

    // Define a point
    NumericalPoint point( dimension, 1.0 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    NumericalScalar eps = 1e-5;
    NumericalPoint DDF = distribution.computeDDF( point );
    fullprint << "ddf     =" << DDF << std::endl;
    fullprint << "ddf (ref)=" << distributionRef.computeDDF( point ) << std::endl;
    NumericalPoint ddfFD(distribution.ContinuousDistribution::computeDDF(point));
    fullprint << "ddf (FD)=" << ddfFD << std::endl;
    NumericalScalar LPDF = distribution.computeLogPDF( point );
    fullprint << "log pdf=" << LPDF << std::endl;
    NumericalScalar PDF = distribution.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    fullprint << "pdf (ref)=" << distributionRef.computePDF( point ) << std::endl;
    if (dimension == 1)
    {
      fullprint << "pdf (FD)=" << (distribution.computeCDF( point + NumericalPoint(1, eps) ) - distribution.computeCDF( point  + NumericalPoint(1, -eps) )) / (2.0 * eps) << std::endl;
    }
    NumericalScalar CDF = distribution.computeCDF( point );
    fullprint << "cdf=" << CDF << std::endl;
    NumericalScalar CCDF = distribution.computeComplementaryCDF( point );
    fullprint << "ccdf=" << CCDF << std::endl;
    NumericalScalar Survival = distribution.computeSurvivalFunction( point );
    fullprint << "survival=" << Survival << std::endl;
    NumericalPoint InverseSurvival = distribution.computeInverseSurvivalFunction(0.95);
    fullprint << "Inverse survival=" << InverseSurvival << std::endl;
    fullprint << "Survival(inverse survival)=" << distribution.computeSurvivalFunction(InverseSurvival) << std::endl;
    fullprint << "cdf (ref)=" << distributionRef.computeCDF( point ) << std::endl;
    NumericalComplex CF = distribution.computeCharacteristicFunction( point[0] );
    fullprint << "characteristic function=" << CF << std::endl;
    NumericalComplex LCF = distribution.computeLogCharacteristicFunction( point[0] );
    fullprint << "log characteristic function=" << LCF << std::endl;
    NumericalPoint quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile=" << quantile << std::endl;
    fullprint << "quantile (ref)=" << distributionRef.computeQuantile( 0.95 ) << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    if (distribution.getDimension() <= 2)
    {
      // Confidence regions
      NumericalScalar threshold;
      fullprint << "Minimum volume interval=" << distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95, threshold) << std::endl;
      fullprint << "threshold=" << threshold << std::endl;
      NumericalScalar beta;
      LevelSet levelSet(distribution.computeMinimumVolumeLevelSetWithThreshold(0.95, beta));
      fullprint << "Minimum volume level set=" << levelSet << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Bilateral confidence interval=" << distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Unilateral confidence interval (lower tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, false, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Unilateral confidence interval (upper tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, true, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
    }

    NumericalPoint x(3);
    x[0] = 1.1;
    x[1] = 1.6;
    x[2] = 2.2;
    NumericalPoint q(3);
    q[0] = 0.1;
    q[1] = 0.3;
    q[2] = 0.7;
    NumericalSample y(3, 1);
    y[0][0] = -0.5;
    y[1][0] =  0.5;
    y[2][0] =  1.5;

    NumericalScalar condCDF = distribution.computeConditionalCDF(x[0], y[0]);
    fullprint << "cond. cdf=" << condCDF << std::endl;
    NumericalPoint condCDFs(distribution.computeConditionalCDF(x, y));
    fullprint << "cond. cdf (vect)=" << condCDFs << std::endl;
    NumericalScalar condPDF = distribution.computeConditionalPDF(x[0], y[0]);
    fullprint << "cond. pdf=" << condPDF << std::endl;
    NumericalPoint condPDFs(distribution.computeConditionalPDF(x, y));
    fullprint << "cond. pdf (vect)=" << condPDFs << std::endl;
    NumericalScalar condQuantile = distribution.computeConditionalQuantile(q[0], y[0]);
    fullprint << "cond. quantile=" << condQuantile << std::endl;
    NumericalPoint condQuantiles(distribution.computeConditionalQuantile(q, y));
    fullprint << "cond. quantile (vect)=" << condQuantiles << std::endl;
    fullprint << "cond. cdf(cond. quantile)=" << distribution.computeConditionalCDF(condQuantiles, y) << std::endl;

    NumericalPoint mean = distribution.getMean();
    fullprint << "mean=" << mean << std::endl;
    fullprint << "mean (ref)=" << distributionRef.getMean() << std::endl;
    CovarianceMatrix covariance = distribution.getCovariance();
    fullprint << "covariance=" << covariance << std::endl;
    fullprint << "covariance (ref)=" << distributionRef.getCovariance() << std::endl;
    CovarianceMatrix correlation = distribution.getCorrelation();
    fullprint << "correlation=" << correlation << std::endl;
//     CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
//     fullprint << "spearman=" << spearman << std::endl;
//     CovarianceMatrix kendall = distribution.getKendallTau();
//     fullprint << "kendall=" << kendall << std::endl;
    //    KernelMixture::NumericalPointCollection parameters = distribution.getParametersCollection();
    //    fullprint << "parameters=" << parameters << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

