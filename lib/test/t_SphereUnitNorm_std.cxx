//                                               -*- C++ -*-
/**
 *  @brief The test file of class SphereUnitNorm
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {
    const UnsignedInteger maxDistance = 4;
    for (UnsignedInteger symmetric = 0 ; symmetric < 2; ++symmetric)
    {
      for (UnsignedInteger dimension = 1 ; dimension <= 3; ++dimension)
      {
        // d- dimensional SphereUniformNorm
        const Point step(dimension, 1.0);
        // Create d-dimensional sphere using static constructor
        SphereUniformNorm sphereUnitNorm(SphereUniformNorm::GetFromGridSteps(step, symmetric));
        fullprint << "sphereUnitNorm = " << sphereUnitNorm << std::endl;
        for (UnsignedInteger distance = 0 ; distance < maxDistance; ++distance)
        {
          const Sample points(sphereUnitNorm.getPoints(distance));
          fullprint << "distance=" << distance << " -  points = " << points << std::endl;
        }
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
