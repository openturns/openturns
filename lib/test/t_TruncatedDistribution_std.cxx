//                                               -*- C++ -*-
/**
 *  @brief The test file of class TruncatedDistribution for standard methods
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

static Point clean(Point in)
{
  UnsignedInteger dim = in.getDimension();
  for(UnsignedInteger i = 0; i < dim; i++)
    if (std::abs(in[i]) < 1.e-10) in[i] = 0.0;
  return in;
}

int main(int , char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    Collection<TruncatedNormal> referenceDistributionCollection(3);
    referenceDistributionCollection[0] = TruncatedNormal(2.0, 1.5, 1.0, 4.0);
    referenceDistributionCollection[1] = TruncatedNormal(2.0, 1.5, 1.0, 200.0);
    referenceDistributionCollection[2] = TruncatedNormal(2.0, 1.5, -200.0, 4.0);
    Collection<TruncatedDistribution> distributionCollection(3);
    distributionCollection[0] = TruncatedDistribution(Normal(2.0, 1.5), 1.0, 4.0);
    distributionCollection[1] = TruncatedDistribution(Normal(2.0, 1.5), 1.0, TruncatedDistribution::LOWER);
    distributionCollection[2] = TruncatedDistribution(Normal(2.0, 1.5), 4.0, TruncatedDistribution::UPPER);
    for (UnsignedInteger testCase = 0; testCase < 3; ++testCase)
    {
      TruncatedDistribution distribution(distributionCollection[testCase]);
      Distribution referenceDistribution(distributionCollection[testCase]);
      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << distribution << std::endl;

      // Is this distribution elliptical ?
      fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

      // Is this distribution continuous ?
      fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

      // Test for realization of distribution
      Point oneRealization = distribution.getRealization();
      fullprint << "oneRealization=" << oneRealization << std::endl;

      // Test for sampling
      UnsignedInteger size = 10000;
      Sample oneSample = distribution.getSample( size );
      fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
      fullprint << "mean=" << oneSample.computeMean() << std::endl;
      fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;

      // Define a point
      Point point( distribution.getDimension(), 2.5 );
      fullprint << "Point= " << point << std::endl;

      // Show PDF and CDF of point
      Point DDF = distribution.computeDDF( point );
      fullprint << "ddf      =" << DDF << std::endl;
      fullprint << "ddf (ref)=" << referenceDistribution.computeDDF(point) << std::endl;
      Scalar PDF = distribution.computePDF( point );
      fullprint << "pdf      =" << PDF << std::endl;
      fullprint << "pdf (ref)=" << referenceDistribution.computePDF(point) << std::endl;
      Scalar CDF = distribution.computeCDF( point );
      fullprint << "cdf      =" << CDF << std::endl;
      fullprint << "cdf (ref)=" << referenceDistribution.computeCDF(point) << std::endl;
      Point PDFgr = distribution.computePDFGradient( point );
      fullprint << "pdf gradient      =" << clean(PDFgr) << std::endl;
      fullprint << "pdf gradient (ref)=" << clean(referenceDistribution.computePDFGradient(point)) << std::endl;
      Point CDFgr = distribution.computeCDFGradient( point );
      fullprint << "cdf gradient      =" << clean(CDFgr) << std::endl;
      fullprint << "cdf gradient (ref)=" << clean(referenceDistribution.computeCDFGradient(point)) << std::endl;
      Point quantile = distribution.computeQuantile( 0.95 );
      fullprint << "quantile      =" << quantile << std::endl;
      fullprint << "quantile (ref)=" << referenceDistribution.computeQuantile( 0.95 ) << std::endl;
      fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      fullprint << "entropy      =" << distribution.computeEntropy() << std::endl;
      fullprint << "entropy (ref)=" << referenceDistribution.computeEntropy() << std::endl;
      fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
      Point mean = distribution.getMean();
      fullprint << "mean      =" << mean << std::endl;
      fullprint << "mean (ref)=" << referenceDistribution.getMean() << std::endl;
      Point standardDeviation = distribution.getStandardDeviation();
      fullprint << "standard deviation      =" << standardDeviation << std::endl;
      fullprint << "standard deviation (ref)=" << referenceDistribution.getStandardDeviation() << std::endl;
      Point skewness = distribution.getSkewness();
      fullprint << "skewness      =" << skewness << std::endl;
      fullprint << "skewness (ref)=" << referenceDistribution.getSkewness() << std::endl;
      Point kurtosis = distribution.getKurtosis();
      fullprint << "kurtosis      =" << kurtosis << std::endl;
      fullprint << "kurtosis (ref)=" << referenceDistribution.getKurtosis() << std::endl;
      CovarianceMatrix covariance = distribution.getCovariance();
      fullprint << "covariance      =" << covariance << std::endl;
      fullprint << "covariance (ref)=" << referenceDistribution.getCovariance() << std::endl;
      TruncatedDistribution::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
      fullprint << "parameters      =" << parameters << std::endl;
      fullprint << "parameters (ref)=" << referenceDistribution.getParametersCollection() << std::endl;
      for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
      fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;

      // Get/Set parameter
      Point parameter(distribution.getParameter());
      fullprint << "Distribution parameters      =" << parameter.__str__() << std::endl;
      parameter[0] = 1.0;
      distribution.setParameter(parameter);
      fullprint << "Distribution after setParameter =" << distribution.getParameter().__str__() << std::endl;

    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
