//                                               -*- C++ -*-
/**
 *  @brief The test file of class CombinationsDistribution for standard methods
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

class TestObject : public CombinationsDistribution
{
public:
  TestObject() : CombinationsDistribution(5, 12) {}
  virtual ~TestObject() {}
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  try
  {
    // Test basic functionnalities
    checkClassWithClassName<TestObject>();

    // Instantiate one distribution object
    CombinationsDistribution distribution(5, 12);
    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;
    UserDefined ref(distribution.getSupport());
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
    Point point({1, 3, 6, 8, 10});
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    Scalar LPDF = distribution.computeLogPDF( point );
    fullprint << "log-pdf     =" << LPDF << std::endl;
    fullprint << "log-pdf ref =" << ref.computeLogPDF(point) << std::endl;
    Scalar PDF = distribution.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    fullprint << "pdf ref =" << ref.computePDF(point) << std::endl;
    Scalar CDF = distribution.computeCDF( point );
    fullprint << "cdf     =" << CDF << std::endl;
    fullprint << "cdf ref =" << ref.computeCDF(point) << std::endl;
    Scalar CCDF = distribution.computeComplementaryCDF( point );
    fullprint << "ccdf     =" << CCDF << std::endl;
    fullprint << "ccdf ref =" << ref.computeComplementaryCDF(point) << std::endl;
    //    Scalar Survival = distribution.computeSurvivalFunction( point );
    //    fullprint << "survival=" << Survival << std::endl;
    Point quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile     =" << quantile << std::endl;
    fullprint << "quantile ref =" << ref.computeQuantile(0.95) << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    fullprint << "entropy     =" << distribution.computeEntropy() << std::endl;
    fullprint << "entropy ref =" << ref.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
    Point mean = distribution.getMean();
    fullprint << "mean     =" << mean << std::endl;
    fullprint << "mean ref =" << ref.getMean() << std::endl;
    CovarianceMatrix covariance = distribution.getCovariance();
    fullprint << "covariance     =" << covariance << std::endl;
    fullprint << "covariance ref =" << ref.getCovariance() << std::endl;
    CovarianceMatrix correlation = distribution.getCorrelation();
    fullprint << "correlation     =" << correlation << std::endl;
    fullprint << "correlation ref =" << ref.getCorrelation() << std::endl;
    //    CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
    //    fullprint << "spearman=" << spearman << std::endl;
    //    CovarianceMatrix kendall = distribution.getKendallTau();
    //    fullprint << "kendall=" << kendall << std::endl;
    CombinationsDistribution::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
