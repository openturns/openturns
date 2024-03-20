//                                               -*- C++ -*-
/**
 *  @brief The test file of class IntervalMesher for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // The 1D mesher
    IntervalMesher mesher1D(Indices(1, 5));
    fullprint << "mesher1D=" << mesher1D << std::endl;
    Mesh mesh1D(mesher1D.build(Interval(-1.0, 2.0)));
    fullprint << "mesh1D=" << mesh1D << std::endl;
    IntervalMesher mesher2D(Indices(2, 4));
    fullprint << "mesher2D=" << mesher2D << std::endl;
    Mesh mesh2D(mesher2D.build(Interval(Point(2, -1.0), Point(2, 2.0))));
    fullprint << "mesh2D=" << mesh2D << std::endl;
    IntervalMesher mesher3D(Indices(3, 3));
    fullprint << "mesher3D=" << mesher3D << std::endl;
    Mesh mesh3D(mesher3D.build(Interval(Point(3, -1.0), Point(3, 2.0))));
    fullprint << "mesh3D=" << mesh3D << std::endl;
    IntervalMesher mesher4D(Indices(4, 2));
    fullprint << "mesher4D=" << mesher4D << std::endl;
    Mesh mesh4D(mesher4D.build(Interval(Point(4, -1.0), Point(4, 2.0))));
    fullprint << "mesh4D=" << mesh4D << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
