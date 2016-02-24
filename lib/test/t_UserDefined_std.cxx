//                                               -*- C++ -*-
/**
 *  @brief The test file of class UserDefined for standard methods
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Instanciate one distribution object
    typedef UserDefined::UserDefinedPairCollection PairCollection;
    PairCollection collection(4);
    NumericalPoint point(1);
    point[0] = 1.0;
    collection[0] = UserDefinedPair(point, 0.3);
    point[0] = 2.0;
    collection[1] = UserDefinedPair(point, 0.1);
    point[0] = 3.0;
    collection[2] = UserDefinedPair(point, 0.6);
    collection[3] = UserDefinedPair(point, 0.6);
    UserDefined distribution(collection);
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
    UnsignedInteger size = 10;
    NumericalSample oneSample = distribution.getSample( size );
    fullprint << "oneSample=" << oneSample << std::endl;

    // Define a point
    point = NumericalPoint( distribution.getDimension(), 2.0 );

    // Show PDF and CDF of a point
    NumericalScalar pointPDF = distribution.computePDF( point );
    NumericalScalar pointCDF = distribution.computeCDF( point );
    fullprint << "point= " << point
              << " pdf=" << pointPDF
              << " cdf=" << pointCDF
              << std::endl;

    // Get 95% quantile
    NumericalPoint quantile = distribution.computeQuantile( 0.95 );
    fullprint << "Quantile=" << quantile << std::endl;

    for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
    fullprint << "Standard representative=" << distribution.getStandardRepresentative()->__str__() << std::endl;
    // To prevent automatic compaction
    ResourceMap::SetAsUnsignedInteger("UserDefined-SmallSize", 5);
    NumericalSample sample(40, 3);
    for (UnsignedInteger i = 0; i < 4; ++i)
    {
      for (UnsignedInteger j = 0; j < 3; ++j)
      {
        sample[i][j] = 10 * (i / 3 + 1) + 0.1 * (j + 1);
      }
    }
    UserDefined multivariateUserDefined(sample);
    fullprint << "Multivariate UserDefined=" << multivariateUserDefined << std::endl;
    multivariateUserDefined.compactSupport();
    fullprint << "Multivariate UserDefined=" << multivariateUserDefined << std::endl;
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
