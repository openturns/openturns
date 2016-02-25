//                                               -*- C++ -*-
/**
 *  @brief The test file of class Mesh for standard methods
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
    {
      NumericalSample vertices(0, 1);
      vertices.add(NumericalPoint(1, 0.5));
      vertices.add(NumericalPoint(1, 1.5));
      vertices.add(NumericalPoint(1, 2.1));
      vertices.add(NumericalPoint(1, 2.7));
      Mesh::IndicesCollection simplicies(3, Indices(2));
      simplicies[0][0] = 0;
      simplicies[0][1] = 1;
      simplicies[1][0] = 1;
      simplicies[1][1] = 2;
      simplicies[2][0] = 2;
      simplicies[2][1] = 3;
      Mesh mesh1D(vertices, simplicies);
      mesh1D.draw().draw("mesh1D");
      mesh1D.draw1D().draw("mesh1D");
      fullprint << "mesh 1D ok" << std::endl;
    }
    {
      NumericalSample vertices(0, 2);
      NumericalPoint p(2);

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
      Mesh::IndicesCollection simplicies(5, Indices(3));
      simplicies[0][0] = 0;
      simplicies[0][1] = 1;
      simplicies[0][2] = 2;

      simplicies[1][0] = 1;
      simplicies[1][1] = 2;
      simplicies[1][2] = 3;

      simplicies[2][0] = 2;
      simplicies[2][1] = 3;
      simplicies[2][2] = 4;

      simplicies[3][0] = 2;
      simplicies[3][1] = 4;
      simplicies[3][2] = 5;

      simplicies[4][0] = 0;
      simplicies[4][1] = 2;
      simplicies[4][2] = 5;
      Mesh mesh2D(vertices, simplicies);
      mesh2D.draw().draw("mesh2D");
      mesh2D.draw2D().draw("mesh2D");
      fullprint << "mesh 2D ok" << std::endl;
    }
    {
      NumericalSample vertices(0, 3);
      NumericalPoint p(3);

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

      Mesh::IndicesCollection simplicies(6, Indices(4));
      simplicies[0][0] = 0;
      simplicies[0][1] = 1;
      simplicies[0][2] = 2;
      simplicies[0][3] = 4;

      simplicies[1][0] = 3;
      simplicies[1][1] = 5;
      simplicies[1][2] = 6;
      simplicies[1][3] = 7;

      simplicies[2][0] = 1;
      simplicies[2][1] = 2;
      simplicies[2][2] = 3;
      simplicies[2][3] = 6;

      simplicies[3][0] = 1;
      simplicies[3][1] = 2;
      simplicies[3][2] = 4;
      simplicies[3][3] = 6;

      simplicies[4][0] = 1;
      simplicies[4][1] = 3;
      simplicies[4][2] = 5;
      simplicies[4][3] = 6;

      simplicies[5][0] = 1;
      simplicies[5][1] = 4;
      simplicies[5][2] = 5;
      simplicies[5][3] = 6;

      Mesh mesh3D(vertices, simplicies);
      mesh3D.draw().draw("mesh3D");
      SquareMatrix rotation(3);
      rotation(0, 0) =  cos(M_PI / 3.0);
      rotation(0, 1) =  sin(M_PI / 3.0);
      rotation(1, 0) = -sin(M_PI / 3.0);
      rotation(1, 1) =  cos(M_PI / 3.0);
      rotation(2, 2) = 1.0;
      mesh3D.draw3D(true, rotation, true, 0.9).draw("mesh3D_2");
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
