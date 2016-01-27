//                                               -*- C++ -*-
/**
 *  @brief The test file of class ProductDistribution for standard methods
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

static NumericalPoint clean(NumericalPoint in)
{
  UnsignedInteger dim(in.getDimension());
  for(UnsignedInteger i = 0; i < dim; i++)
    if (fabs(in[i]) < 1.e-10) in[i] = 0.0;
  return in;
}

int main(int argc, char *argv[])
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
    NumericalPoint oneRealization = distribution.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 10000;
    NumericalSample oneSample = distribution.getSample( size );
    fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
    fullprint << "mean=" << oneSample.computeMean() << std::endl;
    fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;

    // Define a point
    NumericalPoint point( distribution.getDimension(), 2.5 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    NumericalPoint DDF = distribution.computeDDF( point );
    fullprint << "ddf      =" << DDF << std::endl;
    NumericalScalar PDF = distribution.computePDF( point );
    fullprint << "pdf      =" << PDF << std::endl;
    NumericalScalar CDF = distribution.computeCDF( point );
    fullprint << "cdf      =" << CDF << std::endl;
    NumericalPoint PDFgr = distribution.computePDFGradient( point );
    fullprint << "pdf gradient      =" << clean(PDFgr) << std::endl;
    NumericalPoint CDFgr = distribution.computeCDFGradient( point );
    fullprint << "cdf gradient      =" << clean(CDFgr) << std::endl;
    NumericalPoint quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile     =" << quantile << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    NumericalPoint mean = distribution.getMean();
    fullprint << "mean      =" << mean << std::endl;
    NumericalPoint standardDeviation = distribution.getStandardDeviation();
    fullprint << "standard deviation      =" << standardDeviation << std::endl;
    NumericalPoint skewness = distribution.getSkewness();
    fullprint << "skewness      =" << skewness << std::endl;
    NumericalPoint kurtosis = distribution.getKurtosis();
    fullprint << "kurtosis      =" << kurtosis << std::endl;
    CovarianceMatrix covariance = distribution.getCovariance();
    fullprint << "covariance      =" << covariance << std::endl;
    ProductDistribution::NumericalPointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters      =" << parameters << std::endl;
    for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
    fullprint << "Standard representative=" << distribution.getStandardRepresentative()->__str__() << std::endl;

    // Specific to this distribution
    fullprint << "left=" << distribution.getLeft() << std::endl;
    fullprint << "right=" << distribution.getRight() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
