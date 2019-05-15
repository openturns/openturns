//                                               -*- C++ -*-
/**
 * @brief The test file of class PiecewiseHermiteEvaluation for standard methods
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
    Sample locations(size,1);
    Sample values(size,1);
    // Build locations/values/derivatives with non-increasing locations
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      locations(i,0) = 10.0 * i * i / (size - 1.0) / (size - 1.0);
      values[i] = ref(locations[i]);
    }
    InverseDistanceWeightingInterpolation evaluation(locations, values, 10.);
    fullprint << "evaluation=" << evaluation << std::endl;
    // Check the values
    Sample testedLocations(2*size,1);
    for (UnsignedInteger i = 0; i < 2 * size; ++i)
    {
      Point x(1, 10.0 * i * i / (2 * size - 1.0) / ( 2 * size - 1.0));
      testedLocations[i] = x;
      fullprint << "f(" << x[0] << ")=" << evaluation(x) << ", ref=" << ref(x) << std::endl;
    }
    Sample interpolation = evaluation(testedLocations);
    fullprint << interpolation << std::endl;
    fullprint << ref(testedLocations) << std::endl;
    fullprint << (interpolation - ref(testedLocations)) << std::endl;
    //Graph graph = evaluation.draw(testedLocations[0],testedLocations[2*size-1]);
    //graph.add(ref.draw(testedLocations[0],testedLocations[2*size-1]));
    //graph.draw("MonFichier.png");
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
