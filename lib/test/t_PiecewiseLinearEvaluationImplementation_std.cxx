//                                               -*- C++ -*-
/**
 * @brief The test file of class PiecewiseLinearEvaluationImplementation for standard methods
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

  try
  {
    NumericalMathFunction ref("x", "sin(x)");
    UnsignedInteger size = 12;
    NumericalPoint locations(size);
    NumericalPoint values(size);
    // Build locations/values with non-increasing locations
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      locations[i] = 10.0 * i * i / (size - 1.0) / (size - 1.0);
      values[i] = ref(NumericalPoint(1, locations[i]))[0];
    }
    PiecewiseLinearEvaluationImplementation evaluation(locations, values);
    fullprint << "evaluation=" << evaluation << std::endl;
    // Check the values
    for (UnsignedInteger i = 0; i < 2 * size; ++i)
    {
      NumericalPoint x(1, -1.0 + 12.0 * i / (2.0 * size - 1.0));
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
