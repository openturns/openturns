//                                               -*- C++ -*-
/**
 *  @brief The test file of class Mesh for standard methods
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
    {
      Sample vertices(0, 1);
      vertices.add(Point(1, 0.5));
      vertices.add(Point(1, 1.5));
      vertices.add(Point(1, 2.1));
      vertices.add(Point(1, 2.7));
      IndicesCollection simplices(3, 2);
      simplices(0, 0) = 0;
      simplices(0, 1) = 1;
      simplices(1, 0) = 1;
      simplices(1, 1) = 2;
      simplices(2, 0) = 2;
      simplices(2, 1) = 3;
      Mesh mesh1D(vertices, simplices);
      mesh1D.draw();
      mesh1D.draw1D();
      fullprint << "mesh 1D ok" << std::endl;
    }
    {
      Sample vertices(0, 2);
      Point p(2);

      p[0] = 0.0;
      p[1] = 0.0;
      vertices.add(p);

      p[0] = 1.0;
      p[1] = 0.0;
      vertices.add(p);

      p[0] = 1.0;
      p[1] = 1.0;
      vertices.add(p);

      p[0] = 1.5;
      p[1] = 1.0;
      vertices.add(p);

      p[0] = 2.0;
      p[1] = 1.5;
      vertices.add(p);

      p[0] = 0.5;
      p[1] = 1.5;
      vertices.add(p);
      IndicesCollection simplices(5, 3);
      simplices(0, 0) = 0;
      simplices(0, 1) = 1;
      simplices(0, 2) = 2;

      simplices(1, 0) = 1;
      simplices(1, 1) = 2;
      simplices(1, 2) = 3;

      simplices(2, 0) = 2;
      simplices(2, 1) = 3;
      simplices(2, 2) = 4;

      simplices(3, 0) = 2;
      simplices(3, 1) = 4;
      simplices(3, 2) = 5;

      simplices(4, 0) = 0;
      simplices(4, 1) = 2;
      simplices(4, 2) = 5;
      Mesh mesh2D(vertices, simplices);
      mesh2D.draw();
      mesh2D.draw2D();
      fullprint << "mesh 2D ok" << std::endl;
    }
    {
      Sample vertices(0, 3);
      Point p(3);

      p[0] = 0.0;
      p[1] = 0.0;
      p[2] = 0.0;
      vertices.add(p);

      p[0] = 0.0;
      p[1] = 0.0;
      p[2] = 1.0;
      vertices.add(p);

      p[0] = 0.0;
      p[1] = 1.0;
      p[2] = 0.0;
      vertices.add(p);

      p[0] = 0.0;
      p[1] = 1.0;
      p[2] = 1.0;
      vertices.add(p);

      p[0] = 1.0;
      p[1] = 0.0;
      p[2] = 0.0;
      vertices.add(p);

      p[0] = 1.0;
      p[1] = 0.0;
      p[2] = 1.0;
      vertices.add(p);

      p[0] = 1.0;
      p[1] = 1.0;
      p[2] = 0.0;
      vertices.add(p);

      p[0] = 1.0;
      p[1] = 1.0;
      p[2] = 1.0;
      vertices.add(p);

      IndicesCollection simplices(6, 4);
      simplices(0, 0) = 0;
      simplices(0, 1) = 1;
      simplices(0, 2) = 2;
      simplices(0, 3) = 4;

      simplices(1, 0) = 3;
      simplices(1, 1) = 5;
      simplices(1, 2) = 6;
      simplices(1, 3) = 7;

      simplices(2, 0) = 1;
      simplices(2, 1) = 2;
      simplices(2, 2) = 3;
      simplices(2, 3) = 6;

      simplices(3, 0) = 1;
      simplices(3, 1) = 2;
      simplices(3, 2) = 4;
      simplices(3, 3) = 6;

      simplices(4, 0) = 1;
      simplices(4, 1) = 3;
      simplices(4, 2) = 5;
      simplices(4, 3) = 6;

      simplices(5, 0) = 1;
      simplices(5, 1) = 4;
      simplices(5, 2) = 5;
      simplices(5, 3) = 6;

      Mesh mesh3D(vertices, simplices);
      mesh3D.draw();
      SquareMatrix rotation(3);
      rotation(0, 0) =  cos(M_PI / 3.0);
      rotation(0, 1) =  sin(M_PI / 3.0);
      rotation(1, 0) = -sin(M_PI / 3.0);
      rotation(1, 1) =  cos(M_PI / 3.0);
      rotation(2, 2) = 1.0;
      mesh3D.draw3D(true, rotation, true, 0.9);
      fullprint << "mesh 3D ok" << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
