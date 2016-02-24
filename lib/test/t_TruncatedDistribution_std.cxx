//                                               -*- C++ -*-
/**
 *  @brief The test file of class TruncatedDistribution for standard methods
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

static NumericalPoint clean(NumericalPoint in)
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
    Collection<TruncatedNormal> referenceDistribution(3);
    referenceDistribution[0] = TruncatedNormal(2.0, 1.5, 1.0, 4.0);
    referenceDistribution[1] = TruncatedNormal(2.0, 1.5, 1.0, 200.0);
    referenceDistribution[2] = TruncatedNormal(2.0, 1.5, -200.0, 4.0);
    Collection<TruncatedDistribution> distribution(3);
    distribution[0] = TruncatedDistribution(Normal(2.0, 1.5), 1.0, 4.0);
    distribution[1] = TruncatedDistribution(Normal(2.0, 1.5), 1.0, TruncatedDistribution::LOWER);
    distribution[2] = TruncatedDistribution(Normal(2.0, 1.5), 4.0, TruncatedDistribution::UPPER);
    for (UnsignedInteger testCase = 0; testCase < 3; ++testCase)
    {
      fullprint << "Distribution " << distribution[testCase] << std::endl;
      std::cout << "Distribution " << distribution[testCase] << std::endl;

      // Is this distribution elliptical ?
      fullprint << "Elliptical = " << (distribution[testCase].isElliptical() ? "true" : "false") << std::endl;

      // Is this distribution continuous ?
      fullprint << "Continuous = " << (distribution[testCase].isContinuous() ? "true" : "false") << std::endl;

      // Test for realization of distribution
      NumericalPoint oneRealization = distribution[testCase].getRealization();
      fullprint << "oneRealization=" << oneRealization << std::endl;

      // Test for sampling
      UnsignedInteger size = 10000;
      NumericalSample oneSample = distribution[testCase].getSample( size );
      fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
      fullprint << "mean=" << oneSample.computeMean() << std::endl;
      fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;

      // Define a point
      NumericalPoint point( distribution[testCase].getDimension(), 2.5 );
      fullprint << "Point= " << point << std::endl;

      // Show PDF and CDF of point
      NumericalPoint DDF = distribution[testCase].computeDDF( point );
      fullprint << "ddf      =" << DDF << std::endl;
      fullprint << "ddf (ref)=" << referenceDistribution[testCase].computeDDF(point) << std::endl;
      NumericalScalar PDF = distribution[testCase].computePDF( point );
      fullprint << "pdf      =" << PDF << std::endl;
      fullprint << "pdf (ref)=" << referenceDistribution[testCase].computePDF(point) << std::endl;
      NumericalScalar CDF = distribution[testCase].computeCDF( point );
      fullprint << "cdf      =" << CDF << std::endl;
      fullprint << "cdf (ref)=" << referenceDistribution[testCase].computeCDF(point) << std::endl;
      NumericalPoint PDFgr = distribution[testCase].computePDFGradient( point );
      fullprint << "pdf gradient      =" << clean(PDFgr) << std::endl;
      fullprint << "pdf gradient (ref)=" << clean(referenceDistribution[testCase].computePDFGradient(point)) << std::endl;
      NumericalPoint CDFgr = distribution[testCase].computeCDFGradient( point );
      fullprint << "cdf gradient      =" << clean(CDFgr) << std::endl;
      fullprint << "cdf gradient (ref)=" << clean(referenceDistribution[testCase].computeCDFGradient(point)) << std::endl;
      NumericalPoint quantile = distribution[testCase].computeQuantile( 0.95 );
      fullprint << "quantile      =" << quantile << std::endl;
      fullprint << "quantile (ref)=" << referenceDistribution[testCase].computeQuantile( 0.95 ) << std::endl;
      fullprint << "cdf(quantile)=" << distribution[testCase].computeCDF(quantile) << std::endl;
      NumericalPoint mean = distribution[testCase].getMean();
      fullprint << "mean      =" << mean << std::endl;
      fullprint << "mean (ref)=" << referenceDistribution[testCase].getMean() << std::endl;
      NumericalPoint standardDeviation = distribution[testCase].getStandardDeviation();
      fullprint << "standard deviation      =" << standardDeviation << std::endl;
      fullprint << "standard deviation (ref)=" << referenceDistribution[testCase].getStandardDeviation() << std::endl;
      NumericalPoint skewness = distribution[testCase].getSkewness();
      fullprint << "skewness      =" << skewness << std::endl;
      fullprint << "skewness (ref)=" << referenceDistribution[testCase].getSkewness() << std::endl;
      NumericalPoint kurtosis = distribution[testCase].getKurtosis();
      fullprint << "kurtosis      =" << kurtosis << std::endl;
      fullprint << "kurtosis (ref)=" << referenceDistribution[testCase].getKurtosis() << std::endl;
      CovarianceMatrix covariance = distribution[testCase].getCovariance();
      fullprint << "covariance      =" << covariance << std::endl;
      fullprint << "covariance (ref)=" << referenceDistribution[testCase].getCovariance() << std::endl;
      TruncatedDistribution::NumericalPointWithDescriptionCollection parameters = distribution[testCase].getParametersCollection();
      fullprint << "parameters      =" << parameters << std::endl;
      fullprint << "parameters (ref)=" << referenceDistribution[testCase].getParametersCollection() << std::endl;
      for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution[testCase].getStandardMoment(i) << std::endl;
      fullprint << "Standard representative=" << distribution[testCase].getStandardRepresentative()->__str__() << std::endl;

      // Specific to this distribution
      NumericalScalar lowerBound(distribution[testCase].getLowerBound());
      fullprint << "lowerBound=" << lowerBound << std::endl;
      NumericalScalar upperBound(distribution[testCase].getUpperBound());
      fullprint << "upperBound=" << upperBound << std::endl;

    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
