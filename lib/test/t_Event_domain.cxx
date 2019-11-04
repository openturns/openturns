//                                               -*- C++ -*-
/**
 *  @brief The test file of Event class
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    UnsignedInteger dim = 2;
    Normal distribution(dim);

    //
    // Case 1: composite random vector based event
    //
    {
      // The input vector
      RandomVector X(distribution);
      // The model: the identity function
      Description inVars(Description::BuildDefault(dim, "x"));
      SymbolicFunction model(inVars, inVars);
      // The output vector
      CompositeRandomVector Y(model, X);
      // The domain: [0, 1]^dim
      Interval domain(dim);
      // The event
      ThresholdEvent event(Y, domain);

      fullprint << "sample=" << event.getSample(10) << std::endl;

    }
    //
    // Case 2: process based event
    //
    {
      // The input process
      WhiteNoise X(distribution);
      // The domain: [0, 1]^dim
      Interval domain(dim);
      // The event
      ProcessEvent event(X, domain);

      fullprint << "sample=" << event.getSample(10) << std::endl;

    }


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
