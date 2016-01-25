//                                               -*- C++ -*-
/**
 *  @brief The test file of class Normal for standard methods
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

inline NumericalScalar clean(NumericalScalar in)
{
  if (std::abs(in) < 1.e-10) in = 0.0;
  return in;
}

inline NumericalPoint clean(NumericalPoint in)
{
  UnsignedInteger dim(in.getDimension());
  for(UnsignedInteger i = 0; i < dim; i++)
    if (std::abs(in[i]) < 1.e-10) in[i] = 0.0;
  return in;
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();
  try
  {
    // Instanciate one distribution object
    for (UnsignedInteger dim = 1; dim <= 4; dim++)
    {
      fullprint << "\n*** Case " << dim << " ***\n" << std::endl;
      NumericalPoint meanPoint(dim, 0.0);
      NumericalPoint sigma(dim);
      for (UnsignedInteger i = 0; i < dim; i++)
      {
        sigma[i] = i + 1.0;
      }
      CorrelationMatrix R(dim);
      for (UnsignedInteger i = 1; i < dim; i++)
      {
        R(i, i - 1) = 0.5;
      }
      Normal distribution(meanPoint, sigma, R);
      Description description(dim);
      for (UnsignedInteger j = 1; j <= dim; j++)
      {
        OSS oss;
        oss << "Marginal " << j;
        description[j - 1] = oss;
      }
      distribution.setDescription(description);
      fullprint << std::setprecision(5);
      fullprint << "Parameters collection=" << distribution.getParametersCollection() << std::endl;
      for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
      fullprint << "Standard representative=" << distribution.getStandardRepresentative()->__str__() << std::endl;
      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << distribution << std::endl;

      // Is this distribution elliptical ?
      fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

      // Is this distribution continuous ?
      fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

      // Test for realization of distribution
      NumericalPoint oneRealization = distribution.getRealization();
      fullprint << "oneRealization=" << oneRealization << std::endl;

      // Test for sampling
      UnsignedInteger size = 10000;
      NumericalSample oneSample(distribution.getSample( size ));
      fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
      fullprint << "mean=" << oneSample.computeMean() << std::endl;
      fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;
      if (distribution.getDimension() == 1)
      {
        size = 100;
        for (UnsignedInteger i = 0; i < 2; ++i)
        {
          RandomGenerator::SetSeed(1);
          fullprint << "Kolmogorov test for the generator, sample size=" << size << " is " << (FittingTest::Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure() ? "accepted" : "rejected") << std::endl;
          size *= 10;
        }
      }

      // Define a point
      NumericalPoint point( distribution.getDimension(), 0.5 );
      fullprint << "Point= " << point << std::endl;

      // Show PDF and CDF of point
      NumericalScalar eps(1e-4);
      NumericalPoint DDF = distribution.computeDDF( point );
      fullprint << "ddf     =" << clean(DDF) << std::endl;
      NumericalPoint ddfFD(distribution.ContinuousDistribution::computeDDF(point));
      fullprint << "ddf (FD)=" << clean(ddfFD) << std::endl;
      NumericalScalar LPDF = distribution.computeLogPDF( point );
      fullprint << "log pdf=" << LPDF << std::endl;
      NumericalScalar PDF = distribution.computePDF( point );
      fullprint << "pdf     =" << PDF << std::endl;
      if (dim == 1)
      {
        fullprint << "pdf (FD)=" << clean((distribution.computeCDF( point + NumericalPoint(1, eps) ) - distribution.computeCDF( point  + NumericalPoint(1, -eps) )) / (2.0 * eps)) << std::endl;
      }
      NumericalScalar CDF = distribution.computeCDF( point );
      fullprint << "cdf=" << CDF << std::endl;
      if (dim == 1)
      {
        NumericalScalar CCDF = distribution.computeComplementaryCDF( point );
        fullprint << "ccdf=" << CCDF << std::endl;
      }
      NumericalScalar Survival = distribution.computeSurvivalFunction( point );
      fullprint << "survival=" << Survival << std::endl;
      NumericalComplex CF = distribution.computeCharacteristicFunction( point );
      fullprint << "characteristic function=" << CF << std::endl;
      NumericalComplex LCF = distribution.computeLogCharacteristicFunction( point );
      fullprint << "log characteristic function=" << LCF << std::endl;
      NumericalPoint PDFgr = distribution.computePDFGradient( point );
      fullprint << "pdf gradient     =" << clean(PDFgr) << std::endl;
      NumericalPoint PDFgrFD(2 * dim);
      for (UnsignedInteger i = 0; i < dim; i++)
      {
        meanPoint[i] += eps;
        Normal distributionLeft(meanPoint, sigma, R);
        meanPoint[i] -= 2.0 * eps;
        Normal distributionRight(meanPoint, sigma, R);
        PDFgrFD[i] = (distributionLeft.computePDF(point) - distributionRight.computePDF(point)) / (2.0 * eps);
        meanPoint[i] += eps;
      }
      for (UnsignedInteger i = 0; i < dim; i++)
      {
        sigma[i] += eps;
        Normal distributionLeft(meanPoint, sigma, R);
        sigma[i] -= 2.0 * eps;
        Normal distributionRight(meanPoint, sigma, R);
        PDFgrFD[dim + i] = (distributionLeft.computePDF(point) - distributionRight.computePDF(point)) / (2.0 * eps);
        sigma[i] += eps;
      }
      fullprint << "pdf gradient (FD)=" << clean(PDFgrFD) << std::endl;
      NumericalPoint CDFgr = distribution.computeCDFGradient( point );
      fullprint << "cdf gradient     =" << CDFgr << std::endl;
      NumericalPoint quantile = distribution.computeQuantile( 0.95 );
      int oldPrecision = PlatformInfo::GetNumericalPrecision();
      PlatformInfo::SetNumericalPrecision( 4 );
      fullprint << "quantile=" << quantile << std::endl;
      PlatformInfo::SetNumericalPrecision( oldPrecision );
      fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      NumericalPoint mean = distribution.getMean();
      fullprint << "mean=" << mean << std::endl;
      NumericalPoint standardDeviation = distribution.getStandardDeviation();
      fullprint << "standard deviation=" << standardDeviation << std::endl;
      NumericalPoint skewness = distribution.getSkewness();
      fullprint << "skewness=" << skewness << std::endl;
      NumericalPoint kurtosis = distribution.getKurtosis();
      fullprint << "kurtosis=" << kurtosis << std::endl;
      CovarianceMatrix covariance = distribution.getCovariance();
      fullprint << "covariance=" << covariance << std::endl;
      CovarianceMatrix correlation = distribution.getCorrelation();
      fullprint << "correlation=" << correlation << std::endl;
      CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
      fullprint << "spearman=" << spearman << std::endl;
      CovarianceMatrix kendall = distribution.getKendallTau();
      fullprint << "kendall=" << kendall << std::endl;
      Normal::NumericalPointWithDescriptionCollection parameters = distribution.getParametersCollection();
      fullprint << "parameters=" << parameters << std::endl;
      for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
      fullprint << "Standard representative=" << distribution.getStandardRepresentative()->__str__() << std::endl;

      // Specific to this distribution
      NumericalScalar beta(point.normSquare());
      NumericalScalar densityGenerator(distribution.computeDensityGenerator(beta));
      fullprint << "density generator=" << densityGenerator << std::endl;
      fullprint << "pdf via density generator=" << distribution.EllipticalDistribution::computePDF(point) << std::endl;
      NumericalScalar densityGeneratorDerivative(distribution.computeDensityGeneratorDerivative(beta));
      fullprint << "density generator derivative     =" << densityGeneratorDerivative << std::endl;
      fullprint << "density generator derivative (FD)=" << clean((distribution.computeDensityGenerator(beta + eps) - distribution.computeDensityGenerator(beta - eps)) / (2.0 * eps)) << std::endl;
      NumericalScalar densityGeneratorSecondDerivative(distribution.computeDensityGeneratorSecondDerivative(beta));
      fullprint << "density generator second derivative     =" << densityGeneratorSecondDerivative << std::endl;
      fullprint << "density generator second derivative (FD)=" << clean((distribution.computeDensityGeneratorDerivative(beta + eps) - distribution.computeDensityGeneratorDerivative(beta - eps)) / (2.0 * eps)) << std::endl;
      // Compute the radial CDF
      NumericalScalar radius(2.0);
      fullprint << "Radial CDF(" << radius << ")=" << distribution.computeRadialDistributionCDF(radius) << std::endl;
      // Extract the marginals
      for (UnsignedInteger i = 0; i < dim; i++)
      {
        Distribution margin(distribution.getMarginal(i));
        fullprint << "margin=" << margin << std::endl;
        fullprint << "margin PDF=" << margin.computePDF(NumericalPoint(1, 0.5)) << std::endl;
        fullprint << "margin CDF=" << margin.computeCDF(NumericalPoint(1, 0.5)) << std::endl;
        fullprint << "margin quantile=" << margin.computeQuantile(0.95) << std::endl;
        fullprint << "margin realization=" << margin.getRealization() << std::endl;
      }
      if (dim >= 2)
      {
        // Extract a 2-D marginal
        Indices indices(2, 0);
        indices[0] = 1;
        indices[1] = 0;
        fullprint << "indices=" << indices << std::endl;
        Distribution margins(distribution.getMarginal(indices));
        fullprint << "margins=" << margins << std::endl;
        fullprint << "margins PDF=" << margins.computePDF(NumericalPoint(2, 0.5)) << std::endl;
        fullprint << "margins CDF=" << margins.computeCDF(NumericalPoint(2, 0.5)) << std::endl;
        quantile = margins.computeQuantile(0.95);
        fullprint << "margins quantile=" << quantile << std::endl;
        fullprint << "margins CDF(quantile)=" << margins.computeCDF(quantile) << std::endl;
        fullprint << "margins realization=" << margins.getRealization() << std::endl;
      }
      Matrix chol(distribution.getCholesky());
      Matrix invChol(distribution.getInverseCholesky());
      fullprint << "chol=" << chol.clean(1.0e-6) << std::endl;
      fullprint << "invchol=" << invChol.clean(1.0e-6) << std::endl;
      fullprint << "chol*t(chol)=" << (chol * chol.transpose()).clean(1.0e-6) << std::endl;
      fullprint << "chol*invchol=" << (chol * invChol).clean(1.0e-6) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
