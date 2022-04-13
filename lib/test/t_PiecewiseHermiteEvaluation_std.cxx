//                                               -*- C++ -*-
/**
 * @brief The test file of class PiecewiseHermiteEvaluation for standard methods
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
    SymbolicFunction ref("x", "sin(x)");
    UnsignedInteger size = 12;
    Point locations(size);
    Point values(size);
    Point derivatives(size);
    // Build locations/values/derivatives with non-increasing locations
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      locations[i] = 10.0 * i * i / (size - 1.0) / (size - 1.0);
      values[i] = ref(Point(1, locations[i]))[0];
      derivatives[i] = ref.gradient(Point(1, locations[i]))(0, 0);
    }
    PiecewiseHermiteEvaluation evaluation(locations, values, derivatives);
    fullprint << "evaluation=" << evaluation << std::endl;
    // Check the values
    for (UnsignedInteger i = 0; i < 2 * size; ++i)
    {
      Point x(1, -1.0 + 12.0 * i / (2.0 * size - 1.0));
      fullprint << "f(" << x[0] << ")=" << evaluation(x) << ", ref=" << ref(x) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
