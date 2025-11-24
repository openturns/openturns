//                                               -*- C++ -*-
/**
 *  @brief The test file of class Geometric for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

class TestObject : public Geometric
{
public:
  TestObject() : Geometric(0.7) {}
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
    Geometric distribution(0.7);
    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;

    // Is this distribution elliptical ?
    fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

    // Is this distribution continuous ?
    fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

    // Test for realization of distribution
    Point oneRealization = distribution.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Define a point
    Point point( distribution.getDimension(), 3.0 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    Scalar LPDF = distribution.computeLogPDF( point );
    fullprint << "log pdf=" << LPDF << std::endl;
    Scalar PDF = distribution.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    Scalar CDF = distribution.computeCDF( point );
    fullprint << "cdf=" << CDF << std::endl;
    Scalar CCDF = distribution.computeComplementaryCDF( point );
    fullprint << "ccdf=" << CCDF << std::endl;
    Scalar Survival = distribution.computeSurvivalFunction( point );
    fullprint << "survival=" << Survival << std::endl;
    Complex CF = distribution.computeCharacteristicFunction( point[0] );
    fullprint << "characteristic function=" << CF << std::endl;
    Complex LCF = distribution.computeLogCharacteristicFunction( point[0] );
    fullprint << "log characteristic function=" << LCF << std::endl;
    Complex GF = distribution.computeGeneratingFunction( Complex(0.3, 0.7) );
    fullprint << "generating function=" << GF << std::endl;
    Complex LGF = distribution.computeLogGeneratingFunction( Complex(0.3, 0.7) );
    fullprint << "log generating function=" << LGF << std::endl;
    Point PDFgr = distribution.computePDFGradient( point );
    fullprint << "pdf gradient     =" << PDFgr << std::endl;
    Point CDFgr = distribution.computeCDFGradient( point );
    fullprint << "cdf gradient     =" << CDFgr << std::endl;
    Point quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile=" << quantile << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
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
    Geometric::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;
    fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;

    Log::Show(Log::TRACE);
    DistributionValidation validation(distribution);
    validation.run();
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
