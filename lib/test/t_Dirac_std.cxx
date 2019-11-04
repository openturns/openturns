//                                               -*- C++ -*-
/**
 *  @brief The test file of class Dirac for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

class TestObject : public Dirac
{
public:
  TestObject() : Dirac(0.7) {}
  virtual ~TestObject() {}
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Test basic functionnalities
    checkClassWithClassName<TestObject>();

    // Instanciate one distribution object
    // 1D tests
    {
      Dirac distribution(0.7);
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
      Scalar sampleCovariance = oneSample.computeCovariance()(0, 0);
      fullprint << "covariance=" << (std::abs(sampleCovariance) < 1.0e-16 ? 0.0 : sampleCovariance) << std::endl;

      {
        // Define a point
        Point point( distribution.getDimension(), 0.0 );
        fullprint << "Point= " << point << std::endl;

        // Show PDF and CDF of point
        Scalar PDF = distribution.computePDF( point );
        fullprint << "pdf     =" << PDF << std::endl;
        fullprint << "pdf (FD)=" << (distribution.computeCDF( point + Point(1, 0) ) - distribution.computeCDF( point  + Point(1, -1) )) << std::endl;
        Scalar CDF = distribution.computeCDF( point );
        fullprint << "cdf=" << CDF << std::endl;
        Scalar CCDF = distribution.computeComplementaryCDF( point );
        fullprint << "ccdf=" << CCDF << std::endl;
        Scalar Survival = distribution.computeSurvivalFunction( point );
        fullprint << "survival=" << Survival << std::endl;
      }
      {
        // Define a point
        Point point( distribution.getSupport(distribution.getRange())[0]);
        fullprint << "Point= " << point << std::endl;

        // Show PDF and CDF of point
        Scalar LPDF = distribution.computeLogPDF( point );
        fullprint << "log pdf=" << LPDF << std::endl;
        Scalar PDF = distribution.computePDF( point );
        fullprint << "pdf     =" << PDF << std::endl;
        fullprint << "pdf (FD)=" << (distribution.computeCDF( point + Point(1, 0) ) - distribution.computeCDF( point  + Point(1, -1) )) << std::endl;
        Scalar CDF = distribution.computeCDF( point );
        fullprint << "cdf=" << CDF << std::endl;
        Scalar CCDF = distribution.computeComplementaryCDF( point );
        fullprint << "ccdf=" << CCDF << std::endl;
        Scalar Survival = distribution.computeSurvivalFunction( point );
        fullprint << "survival=" << Survival << std::endl;
      }
      Complex CF = distribution.computeCharacteristicFunction( 0.5 );
      fullprint << "characteristic function=" << CF << std::endl;
      Complex LCF = distribution.computeLogCharacteristicFunction(0.5 );
      fullprint << "log characteristic function=" << LCF << std::endl;
      Complex GF = distribution.computeGeneratingFunction( Complex(0.5, 0.3) );
      fullprint << "generating function=" << GF << std::endl;
      Complex LGF = distribution.computeLogGeneratingFunction( Complex(0.5, 0.3) );
      fullprint << "log generating function=" << LGF << std::endl;
      Point quantile = distribution.computeQuantile( 0.95 );
      fullprint << "quantile=" << quantile << std::endl;
      fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
      fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
      Point mean = distribution.getMean();
      fullprint << "mean=" << mean << std::endl;
      Point standardDeviation = distribution.getStandardDeviation();
      fullprint << "standard deviation=" << standardDeviation << std::endl;
      Point skewness = distribution.getSkewness();
      fullprint << "skewness=" << skewness << std::endl;
      Point kurtosis = distribution.getKurtosis();
      fullprint << "kurtosis=" << kurtosis << std::endl;
      CovarianceMatrix covariance = distribution.getCovariance();
      fullprint << "covariance=" << covariance << std::endl;
      CovarianceMatrix correlation = distribution.getCorrelation();
      fullprint << "correlation=" << correlation << std::endl;
      CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
      fullprint << "spearman=" << spearman << std::endl;
      CovarianceMatrix kendall = distribution.getKendallTau();
      fullprint << "kendall=" << kendall << std::endl;
      Dirac::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
      fullprint << "parameters=" << parameters << std::endl;
      for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
      fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;
    }
    // N-D tests
    {
      UnsignedInteger dim = 4;
      Dirac distribution(Point(dim, 2.3));
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
      CovarianceMatrix sampleCovariance(oneSample.computeCovariance());
      for (UnsignedInteger i = 0; i < dim; ++i)
        for (UnsignedInteger j = 0; j <= i; ++j)
          if (std::abs(sampleCovariance(i, j)) < 1.0e-16) sampleCovariance(i, j) = 0.0;
      fullprint << "covariance=" << sampleCovariance << std::endl;

      {
        // Define a point
        Point point( dim, 0.0 );
        fullprint << "Point= " << point << std::endl;

        // Show PDF and CDF of point
        Scalar PDF = distribution.computePDF( point );
        fullprint << "pdf     =" << PDF << std::endl;
        fullprint << "pdf (FD)=" << (distribution.computeCDF( point + Point(dim, 0) ) - distribution.computeCDF( point  + Point(dim, -1) )) << std::endl;
        Scalar CDF = distribution.computeCDF( point );
        fullprint << "cdf=" << CDF << std::endl;
        Scalar CCDF = distribution.computeComplementaryCDF( point );
        fullprint << "ccdf=" << CCDF << std::endl;
      }
      {
        // Define a point
        Point point( distribution.getSupport(distribution.getRange())[0]);
        fullprint << "Point= " << point << std::endl;

        // Show PDF and CDF of point
        Scalar LPDF = distribution.computeLogPDF( point );
        fullprint << "log pdf=" << LPDF << std::endl;
        Scalar PDF = distribution.computePDF( point );
        fullprint << "pdf     =" << PDF << std::endl;
        fullprint << "pdf (FD)=" << (distribution.computeCDF( point + Point(dim, 0) ) - distribution.computeCDF( point  + Point(dim, -1) )) << std::endl;
        Scalar CDF = distribution.computeCDF( point );
        fullprint << "cdf=" << CDF << std::endl;
        Scalar CCDF = distribution.computeComplementaryCDF( point );
        fullprint << "ccdf=" << CCDF << std::endl;
      }
      Point quantile = distribution.computeQuantile( 0.95 );
      fullprint << "quantile=" << quantile << std::endl;
      fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      Point mean = distribution.getMean();
      fullprint << "mean=" << mean << std::endl;
      Point standardDeviation = distribution.getStandardDeviation();
      fullprint << "standard deviation=" << standardDeviation << std::endl;
      Point skewness = distribution.getSkewness();
      fullprint << "skewness=" << skewness << std::endl;
      Point kurtosis = distribution.getKurtosis();
      fullprint << "kurtosis=" << kurtosis << std::endl;
      CovarianceMatrix covariance = distribution.getCovariance();
      fullprint << "covariance=" << covariance << std::endl;
      CovarianceMatrix correlation = distribution.getCorrelation();
      fullprint << "correlation=" << correlation << std::endl;
      CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
      fullprint << "spearman=" << spearman << std::endl;
      CovarianceMatrix kendall = distribution.getKendallTau();
      fullprint << "kendall=" << kendall << std::endl;
      Dirac::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
      fullprint << "parameters=" << parameters << std::endl;
      for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
      fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
