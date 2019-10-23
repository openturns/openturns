//                                               -*- C++ -*-
/**
 *  @brief The test file of class Multinomial for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

class TestObject : public Multinomial
{
public:
  TestObject() : Multinomial(5, Point(3, 0.25)) {}
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

    {
      // Instanciate one distribution object
      Multinomial distribution;
    }

    // Instanciate one distribution object
    Multinomial distribution(5, Point(3, 0.25));
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
    // Support
    Sample support(distribution.getSupport());
    fullprint << "support=" << support << std::endl;
    Interval interval(Point(distribution.getDimension(), 1.0), Point(distribution.getDimension(), 3.0));
    Sample restrictedSupport(distribution.getSupport(interval));
    fullprint << "support restricted to the interval=" << interval << " gives=" << restrictedSupport << std::endl;
    // Define a point
    Point point( distribution.getDimension(), 1.0 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    Scalar LPDF = distribution.computeLogPDF( point );
    fullprint << "log pdf(" << point.__str__() << ")=" << LPDF << std::endl;
    Scalar PDF = distribution.computePDF( point );
    fullprint << "pdf    (" << point.__str__() << ")=" << PDF << std::endl;
    Scalar CDF = distribution.computeCDF( point );
    fullprint << "cdf    (" << point.__str__() << ")=" << std::setprecision(5) << CDF << std::setprecision(6) << std::endl;
    Scalar CCDF = distribution.computeComplementaryCDF( point );
    fullprint << "ccdf   (" << point.__str__() << ")=" << std::setprecision(5) << CCDF << std::setprecision(6) << std::endl;
    Scalar Survival = distribution.computeSurvivalFunction( point );
    fullprint << "survival(" << point.__str__() << ")=" << Survival << std::endl;
    Point quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile(0.95)=" << quantile << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    Point lower(distribution.getDimension());
    Point upper(distribution.getDimension());
    for (UnsignedInteger i = 0; i < distribution.getDimension(); ++i)
    {
      lower[i] = i;
      upper[i] = i + 1.0;
    }
    interval = Interval(lower, upper);
    fullprint << "probability(" << interval.__str__() << ")=" << distribution.computeProbability(interval) << std::endl;
    fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
    Point mean = distribution.getMean();
    fullprint << "mean=" << mean << std::endl;
    CovarianceMatrix covariance = distribution.getCovariance();
    fullprint << "covariance=" << covariance << std::endl;
    CovarianceMatrix correlation = distribution.getCorrelation();
    fullprint << "correlation=" << correlation << std::endl;
    //    CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
    //    fullprint << "spearman=" << spearman << std::endl;
    //    CovarianceMatrix kendall = distribution.getKendallTau();
    //    fullprint << "kendall=" << kendall << std::endl;
    Multinomial::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
