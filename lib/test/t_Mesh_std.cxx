//                                               -*- C++ -*-
/**
 *  @brief The test file of class Mesh for standard methods
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
      Sample vertices(0, 1);
      vertices.add(Point(1, 0.5));
      vertices.add(Point(1, 1.5));
      vertices.add(Point(1, 2.1));
      vertices.add(Point(1, 2.7));
      Mesh::IndicesCollection simplicies(3, Indices(2));
      simplicies[0][0] = 0;
      simplicies[0][1] = 1;
      simplicies[1][0] = 1;
      simplicies[1][1] = 2;
      simplicies[2][0] = 2;
      simplicies[2][1] = 3;
      Mesh mesh1D(vertices, simplicies);
      mesh1D.computeKDTree();
      fullprint << "1D mesh=" << mesh1D << std::endl;
      fullprint << "Is empty? " << mesh1D.isEmpty() << std::endl;
      fullprint << "vertices=" << mesh1D.getVertices() << std::endl;
      fullprint << "simplices=" << mesh1D.getSimplices() << std::endl;
      fullprint << "volume=" << mesh1D.getVolume() << std::endl;
      fullprint << "First simplex volume=" << mesh1D.computeSimplexVolume(0) << std::endl;
      Point p(1);
      p[0] = 1.3;
      fullprint << "is p=" << p << " in mesh? " << mesh1D.contains(p) << std::endl;
      {
        Point point(1, 1.8);
        fullprint << "Nearest index(" << point << ")=" << mesh1D.getNearestVertexIndex(point) << std::endl;
        Point coordinates;
        Indices vertexSimplexIndices = mesh1D.getNearestVertexAndSimplexIndicesWithCoordinates(point, coordinates);
        fullprint << "Nearest index(" << point << "), simplex and coordinates=" << vertexSimplexIndices << ", " << coordinates << std::endl;
      }
      {
        Point point(1, -1.8);
        fullprint << "Nearest index(" << point << ")=" << mesh1D.getNearestVertexIndex(point) << std::endl;
        Point coordinates;
        Indices vertexSimplexIndices = mesh1D.getNearestVertexAndSimplexIndicesWithCoordinates(point, coordinates);
        fullprint << "Nearest index(" << point << "), simplex and coordinates=" << vertexSimplexIndices << ", " << coordinates << std::endl;
      }
      Sample points(2, 1);
      points[0] = Point(1, -0.25);
      points[1] = Point(1, 2.25);
      fullprint << "Nearest index(" << points << ")=" << mesh1D.getNearestVertexIndex(points) << std::endl;
      fullprint << "P1 Gram=" << mesh1D.computeP1Gram() << std::endl;
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
      mesh2D.computeKDTree();
      fullprint << "2D mesh=" << mesh2D << std::endl;
      Point point(2, 1.8);
      fullprint << "Nearest index(" << point << ")=" << mesh2D.getNearestVertexIndex(point) << std::endl;
      Sample points(2, 2);
      points[0] = Point(2, -0.25);
      points[1] = Point(2, 2.25);
      fullprint << "Nearest index(" << points << ")=" << mesh2D.getNearestVertexIndex(points) << std::endl;
      fullprint << "P1 Gram=" << mesh2D.computeP1Gram() << std::endl;
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
      mesh3D.computeKDTree();
      fullprint << "3D mesh=" << mesh3D << std::endl;
      Point point(3, 1.8);
      fullprint << "Nearest index(" << point << ")=" << mesh3D.getNearestVertexIndex(point) << std::endl;
      Sample points(2, 3);
      points[0] = Point(3, -0.25);
      points[1] = Point(3, 2.25);
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
