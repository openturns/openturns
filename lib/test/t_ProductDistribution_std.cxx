//                                               -*- C++ -*-
/**
 *  @brief The test file of class ProductDistribution for standard methods
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    Uniform left(-1.0, 2.0);
    Normal right(1.0, 2.0);
    ProductDistribution distribution(left, right);
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
    Scalar PDF = distribution.computePDF( point );
    fullprint << "pdf      =" << PDF << std::endl;
    Scalar CDF = distribution.computeCDF( point );
    fullprint << "cdf      =" << CDF << std::endl;
    Point PDFgr = distribution.computePDFGradient( point );
    fullprint << "pdf gradient      =" << clean(PDFgr) << std::endl;
    Point CDFgr = distribution.computeCDFGradient( point );
    fullprint << "cdf gradient      =" << clean(CDFgr) << std::endl;
    Point quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile     =" << quantile << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(10000)).computeMean()[0] << std::endl;
    Point mean = distribution.getMean();
    fullprint << "mean      =" << mean << std::endl;
    Point standardDeviation = distribution.getStandardDeviation();
    fullprint << "standard deviation      =" << standardDeviation << std::endl;
    Point skewness = distribution.getSkewness();
    fullprint << "skewness      =" << skewness << std::endl;
    Point kurtosis = distribution.getKurtosis();
    fullprint << "kurtosis      =" << kurtosis << std::endl;
    CovarianceMatrix covariance = distribution.getCovariance();
    fullprint << "covariance      =" << covariance << std::endl;
    ProductDistribution::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters      =" << parameters << std::endl;
    for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
    fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;

    // Specific to this distribution
    fullprint << "left=" << distribution.getLeft() << std::endl;
    fullprint << "right=" << distribution.getRight() << std::endl;

    // For ticket 957
    {
      Distribution distribution2 = Uniform() * Uniform() * Uniform();
      fullprint << "distribution=" << distribution2 << std::endl;
      fullprint << "mean=" << distribution2.getMean() << std::endl;
      fullprint << "standard deviation=" << distribution2.getStandardDeviation() << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
