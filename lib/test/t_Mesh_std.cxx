//                                               -*- C++ -*-
/**
 *  @brief The test file of class Mesh for standard methods
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
      Mesh mesh1D;
      fullprint << "Default 1D mesh=" << mesh1D << std::endl;
    }
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
      fullprint << "1D mesh=" << mesh1D << std::endl;
      fullprint << "Is empty? " << mesh1D.isEmpty() << std::endl;
      fullprint << "vertices=" << mesh1D.getVertices() << std::endl;
      fullprint << "simplices=" << mesh1D.getSimplices() << std::endl;
      fullprint << "volume=" << mesh1D.getVolume() << std::endl;
      fullprint << "First simplex volume=" << mesh1D.computeSimplexVolume(0) << std::endl;
      NumericalPoint p(1);
      p[0] = 1.3;
      fullprint << "is p=" << p << " in mesh? " << mesh1D.contains(p) << std::endl;
      {
        NumericalPoint point(1, 1.8);
        fullprint << "Nearest index(" << point << ")=" << mesh1D.getNearestVertexIndex(point) << std::endl;
        NumericalPoint coordinates;
        Indices vertexSimplexIndices = mesh1D.getNearestVertexAndSimplexIndicesWithCoordinates(point, coordinates);
        fullprint << "Nearest index(" << point << "), simplex and coordinates=" << vertexSimplexIndices << ", " << coordinates << std::endl;
      }
      {
        NumericalPoint point(1, -1.8);
        fullprint << "Nearest index(" << point << ")=" << mesh1D.getNearestVertexIndex(point) << std::endl;
        NumericalPoint coordinates;
        Indices vertexSimplexIndices = mesh1D.getNearestVertexAndSimplexIndicesWithCoordinates(point, coordinates);
        fullprint << "Nearest index(" << point << "), simplex and coordinates=" << vertexSimplexIndices << ", " << coordinates << std::endl;
      }
      NumericalSample points(2, 1);
      points[0] = NumericalPoint(1, -0.25);
      points[1] = NumericalPoint(1, 2.25);
      fullprint << "Nearest index(" << points << ")=" << mesh1D.getNearestVertexIndex(points) << std::endl;
      fullprint << "P1 Gram=" << mesh1D.computeP1Gram() << std::endl;
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
      fullprint << "2D mesh=" << mesh2D << std::endl;
      NumericalPoint point(2, 1.8);
      fullprint << "Nearest index(" << point << ")=" << mesh2D.getNearestVertexIndex(point) << std::endl;
      NumericalSample points(2, 2);
      points[0] = NumericalPoint(2, -0.25);
      points[1] = NumericalPoint(2, 2.25);
      fullprint << "Nearest index(" << points << ")=" << mesh2D.getNearestVertexIndex(points) << std::endl;
      fullprint << "P1 Gram=" << mesh2D.computeP1Gram() << std::endl;
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
      fullprint << "3D mesh=" << mesh3D << std::endl;
      NumericalPoint point(3, 1.8);
      fullprint << "Nearest index(" << point << ")=" << mesh3D.getNearestVertexIndex(point) << std::endl;
      NumericalSample points(2, 3);
      points[0] = NumericalPoint(3, -0.25);
      points[1] = NumericalPoint(3, 2.25);
      fullprint << "Nearest index(" << points << ")=" << mesh3D.getNearestVertexIndex(points) << std::endl;
      fullprint << "P1 Gram=" << mesh3D.computeP1Gram() << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
