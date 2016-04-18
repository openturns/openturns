//                                               -*- C++ -*-
/**
 *  @brief The test file of class Poisson for standard methods
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

class TestObject : public Poisson
{
public:
  TestObject() : Poisson(10.0) {}
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
    Poisson distribution(10.0);
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
#if 0
    size = 100;
    for (UnsignedInteger i = 0; i < 2; ++i)
    {
      fullprint << "ChiSquare test for the generator, sample size=" << size << " is " << (FittingTest::ChiSquared(distribution.getSample(size), distribution).getBinaryQualityMeasure() ? "accepted" : "rejected") << std::endl;
      size *= 10;
    }
#endif
    // Define a point
    NumericalPoint point( distribution.getDimension(), 12.0 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    NumericalScalar eps = 1e-5;
    NumericalScalar LPDF = distribution.computeLogPDF( point );
    fullprint << "log pdf=" << LPDF << std::endl;
    NumericalScalar PDF = distribution.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    fullprint << "pdf (FD)=" << (distribution.computeCDF( point + NumericalPoint(1, 0) ) - distribution.computeCDF( point  + NumericalPoint(1, -1) )) << std::endl;
    NumericalScalar CDF = distribution.computeCDF( point );
    fullprint << "cdf=" << CDF << std::endl;
    NumericalScalar CCDF = distribution.computeComplementaryCDF( point );
    fullprint << "ccdf=" << CCDF << std::endl;
    NumericalScalar Survival = distribution.computeSurvivalFunction( point );
    fullprint << "survival=" << Survival << std::endl;
    NumericalComplex CF = distribution.computeCharacteristicFunction( point[0] );
    fullprint << "characteristic function=" << CF << std::endl;
    NumericalComplex LCF = distribution.computeLogCharacteristicFunction( point[0] );
    fullprint << "log characteristic function=" << LCF << std::endl;
    NumericalComplex GF = distribution.computeGeneratingFunction( NumericalComplex(0.3, 0.7) );
    fullprint << "generating function=" << GF << std::endl;
    NumericalComplex LGF = distribution.computeLogGeneratingFunction( NumericalComplex(0.3, 0.7) );
    fullprint << "log generating function=" << LGF << std::endl;
    NumericalPoint PDFgr = distribution.computePDFGradient( point );
    fullprint << "pdf gradient     =" << PDFgr << std::endl;
    NumericalPoint PDFgrFD(1);
    PDFgrFD[0] = (Poisson(distribution.getLambda() + eps).computePDF(point) -
                  Poisson(distribution.getLambda() - eps).computePDF(point)) / (2.0 * eps);
    fullprint << "pdf gradient (FD)=" << PDFgrFD << std::endl;
    NumericalPoint CDFgr = distribution.computeCDFGradient( point );
    fullprint << "cdf gradient     =" << CDFgr << std::endl;
    NumericalPoint CDFgrFD(1);
    CDFgrFD[0] = (Poisson(distribution.getLambda() + eps).computeCDF(point) -
                  Poisson(distribution.getLambda() - eps).computeCDF(point)) / (2.0 * eps);
    fullprint << "cdf gradient (FD)=" << CDFgrFD << std::endl;
    NumericalPoint quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile=" << quantile << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    NumericalPoint quantileTail = distribution.computeQuantile( 0.95, true );
    fullprint << "quantile (tail)=" << quantileTail << std::endl;
    NumericalScalar CDFTail = distribution.computeComplementaryCDF( quantileTail );
    fullprint << "cdf (tail)=" << CDFTail << std::endl;
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
    Poisson::NumericalPointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;
    for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
    fullprint << "Standard representative=" << distribution.getStandardRepresentative()->__str__() << std::endl;
    fullprint << "probabilities=" << distribution.getProbabilities() << std::endl;

    // Specific to this distribution
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
