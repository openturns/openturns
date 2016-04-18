//                                               -*- C++ -*-
/**
 *  @brief The test file of class Arcsine for standard methods
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

class TestObject : public Arcsine
{
public:
  TestObject() : Arcsine(-1.0, 1.0) {}
  virtual ~TestObject() {}
};


int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Test basic functionnalities
    checkClassWithClassName<TestObject>();

    // Instanciate one distribution object
    Arcsine distribution(5.2, 11.6);
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
    fullprint << "skewness=" << oneSample.computeSkewness() << std::endl;
    fullprint << "kurtosis=" << oneSample.computeKurtosis() << std::endl;
    size = 100;
    for (UnsignedInteger i = 0; i < 2; ++i)
    {
      fullprint << "Kolmogorov test for the generator, sample size=" << size << " is " << (FittingTest::Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure() ? "accepted" : "rejected") << std::endl;
      size *= 10;
    }
    // Define a point
    NumericalPoint point(distribution.getDimension(), 9.1);
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    NumericalScalar eps = 1e-5;
    NumericalPoint DDF = distribution.computeDDF( point );
    fullprint << "ddf     =" << DDF << std::endl;
    fullprint << "ddf (FD)=" << distribution.ContinuousDistribution::computeDDF(point) << std::endl;
    NumericalScalar LPDF = distribution.computeLogPDF( point );
    fullprint << "log pdf=" << LPDF << std::endl;
    NumericalScalar PDF = distribution.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    fullprint << "pdf (FD)=" << (distribution.computeCDF( point + NumericalPoint(1, eps) ) - distribution.computeCDF( point  + NumericalPoint(1, -eps) )) / (2.0 * eps) << std::endl;
    NumericalScalar CDF = distribution.computeCDF( point );
    fullprint << "cdf=" << CDF << std::endl;
    NumericalScalar CCDF = distribution.computeComplementaryCDF( point );
    fullprint << "ccdf=" << CCDF << std::endl;
    NumericalScalar Survival = distribution.computeSurvivalFunction( point );
    fullprint << "survival=" << Survival << std::endl;
    NumericalPoint quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile=" << quantile << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    NumericalPoint quantileTail = distribution.computeQuantile( 0.95, true );
    fullprint << "quantile (tail)=" << quantileTail << std::endl;
    NumericalScalar CDFTail = distribution.computeComplementaryCDF( quantileTail );
    fullprint << "cdf (tail)=" << CDFTail << std::endl;
    NumericalPoint PDFgr = distribution.computePDFGradient( point );
    fullprint << "pdf gradient     =" << PDFgr << std::endl;
    NumericalPoint PDFgrFD(2);
    PDFgrFD[0] = (Arcsine(distribution.getA() + eps, distribution.getB()).computePDF(point) -
                  Arcsine(distribution.getA() - eps, distribution.getB()).computePDF(point)) / (2.0 * eps);
    PDFgrFD[1] = (Arcsine(distribution.getA(), distribution.getB() + eps).computePDF(point) -
                  Arcsine(distribution.getA(), distribution.getB() - eps).computePDF(point)) / (2.0 * eps);
    fullprint << "pdf gradient (FD)=" << PDFgrFD << std::endl;
    NumericalPoint CDFgr = distribution.computeCDFGradient( point );
    fullprint << "cdf gradient     =" << CDFgr << std::endl;
    NumericalPoint CDFgrFD(2);
    CDFgrFD[0] = (Arcsine(distribution.getA() + eps, distribution.getB()).computeCDF(point) -
                  Arcsine(distribution.getA() - eps, distribution.getB()).computeCDF(point)) / (2.0 * eps);
    CDFgrFD[1] = (Arcsine(distribution.getA(), distribution.getB() + eps).computeCDF(point) -
                  Arcsine(distribution.getA(), distribution.getB() - eps).computeCDF(point)) / (2.0 * eps);
    fullprint << "cdf gradient (FD)=" << CDFgrFD << std::endl;
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
    Arcsine::NumericalPointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;
    for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
    fullprint << "Standard representative=" << distribution.getStandardRepresentative()->__str__() << std::endl;
    // Specific to this distribution
    NumericalScalar mu = distribution.getMu();
    fullprint << "mu=" << mu << std::endl;
    NumericalScalar sigma = distribution.getSigma();
    fullprint << "sigma=" << sigma << std::endl;
    Arcsine newDistribution(mu, sigma, Arcsine::MUSIGMA);
    fullprint << "a from (mu, sigma)=" << newDistribution.getA() << std::endl;
    fullprint << "b from (mu, sigma)=" << newDistribution.getB() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
