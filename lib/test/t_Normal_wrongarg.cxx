//                                               -*- C++ -*-
/**
 *  @brief The test file of class Normal with wrong arguments
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

class TestObject : public Normal
{
public:
  TestObject() : Normal(Point(1), Point(1), CorrelationMatrix(1)) {}
  virtual ~TestObject() {}
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    try
    {

      // Instanciate one distribution object
      Point meanPoint(1);
      meanPoint[0] = 1.0;
      Point sigma(1);
      sigma[0] = 1.0;
      CorrelationMatrix R(1);
      R(0, 0) = 1.0;
      Normal distribution(meanPoint, sigma, R);
      fullprint << "Distribution " << distribution << std::endl;

      // We try to set an erroneous covariance matrix (wrong dimension) into distribution
      CorrelationMatrix newR(2);
      distribution.setCorrelation(newR);

      // Normally, we should never go here
      throw TestFailed("Exception has NOT been thrown or catched !");


    }
    catch (InvalidArgumentException & ex)
    {
      // Nothing to do
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
