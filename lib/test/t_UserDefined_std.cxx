//                                               -*- C++ -*-
/**
 *  @brief The test file of class UserDefined for standard methods
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Instanciate one distribution object
    Sample x(4, 1);
    x[0][0] = 1.0;
    x[1][0] = 2.0;
    x[2][0] = 3.0;
    x[3][0] = 3.0;
    Point p(4);
    p[0] = 0.3;
    p[1] = 0.1;
    p[2] = 0.6;
    p[3] = 0.6;
    UserDefined distribution(x, p);
    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;
    // Is this distribution elliptical ?
    fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

    // Is this distribution continuous ?
    fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

    // Has the distribution an independent copula ?
    fullprint << "Independent copula = " << (distribution.hasIndependentCopula() ? "true" : "false") << std::endl;

    // Test for realization of distribution
    Point oneRealization = distribution.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 10;
    Sample oneSample = distribution.getSample( size );
    fullprint << "oneSample=" << oneSample << std::endl;

    // Define a point
    Point point(distribution.getDimension(), 2.0);

    // Show PDF and CDF of a point
    Scalar pointPDF = distribution.computePDF( point );
    Scalar pointCDF = distribution.computeCDF( point );
    fullprint << "point= " << point
              << " pdf=" << pointPDF
              << " cdf=" << pointCDF
              << std::endl;

    // Get 95% quantile
    Point quantile = distribution.computeQuantile( 0.95 );
    fullprint << "Quantile=" << quantile << std::endl;
    fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;

    for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
    fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;
    // To prevent automatic compaction
    ResourceMap::SetAsUnsignedInteger("UserDefined-SmallSize", 5);
    Sample sample(40, 3);
    for (UnsignedInteger i = 0; i < 4; ++i)
    {
      for (UnsignedInteger j = 0; j < 3; ++j)
      {
        sample[i][j] = 10 * (i / 3 + 1) + 0.1 * (j + 1);
      }
    }
    UserDefined multivariateUserDefined(sample);
    multivariateUserDefined.setName("Unnamed");
    fullprint << "Multivariate UserDefined=" << multivariateUserDefined << std::endl;
    multivariateUserDefined.compactSupport();
    fullprint << "Multivariate UserDefined=" << multivariateUserDefined << std::endl;
    // Has the distribution an independent copula ?
    fullprint << "Independent copula = " << (multivariateUserDefined.hasIndependentCopula() ? "true" : "false") << std::endl;

    fullprint << "entropy=" << multivariateUserDefined.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -multivariateUserDefined.computeLogPDF(multivariateUserDefined.getSample(1000000)).computeMean()[0] << std::endl;
    fullprint << "Marginal 0=" << Distribution(multivariateUserDefined.getMarginal(0)) << std::endl;
    Indices indices(2);
    indices[0] = 2;
    indices[1] = 0;
    fullprint << "Marginal (2, 0)=" << Distribution(multivariateUserDefined.getMarginal(indices)) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
