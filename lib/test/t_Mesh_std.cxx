//                                               -*- C++ -*-
/**
 *  @brief The test file of class Mesh for standard methods
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
      IndicesCollection simplices(3, 2);
      simplices(0, 0) = 0;
      simplices(0, 1) = 1;
      simplices(1, 0) = 1;
      simplices(1, 1) = 2;
      simplices(2, 0) = 2;
      simplices(2, 1) = 3;
      Mesh mesh1D(vertices, simplices);
      MeshDomain mesh1Ddomain(mesh1D);
      KDTree tree(vertices);
      EnclosingSimplexAlgorithm enclosingSimplex(vertices, simplices);
      fullprint << "1D mesh=" << mesh1D << std::endl;
      fullprint << "Is empty? " << mesh1D.isEmpty() << std::endl;
      fullprint << "vertices=" << mesh1D.getVertices() << std::endl;
      fullprint << "simplices=" << mesh1D.getSimplices() << std::endl;
      fullprint << "volume=" << mesh1D.getVolume() << std::endl;
      fullprint << "simplices volume=" << mesh1D.computeSimplicesVolume() << std::endl;
      Point p(1);
      p[0] = 1.3;
      fullprint << "is p=" << p << " in mesh? " << mesh1Ddomain.contains(p) << std::endl;
      {
        Point point(1, 1.8);
        UnsignedInteger nearestIndex = tree.query(point);
        fullprint << "Nearest index(" << point << ")=" << nearestIndex << std::endl;
        Point coordinates;
        const UnsignedInteger simplexIndex = enclosingSimplex.query(point);
        Bool found = mesh1D.checkPointInSimplexWithCoordinates(point, simplexIndex, coordinates);
        Indices vertexSimplexIndices(1, nearestIndex);
        if (found) vertexSimplexIndices.add(simplexIndex);
        fullprint << "Nearest index(" << point << "), simplex and coordinates=" << vertexSimplexIndices << ", " << coordinates << std::endl;
      }
      {
        Point point(1, -1.8);
        UnsignedInteger nearestIndex = tree.query(point);
        fullprint << "Nearest index(" << point << ")=" << nearestIndex << std::endl;
        Point coordinates;
        const UnsignedInteger simplexIndex = enclosingSimplex.query(point);
        Bool found = mesh1D.checkPointInSimplexWithCoordinates(point, simplexIndex, coordinates);
        Indices vertexSimplexIndices(1, nearestIndex);
        if (found) vertexSimplexIndices.add(simplexIndex);
        fullprint << "Nearest index(" << point << "), simplex and coordinates=" << vertexSimplexIndices << ", " << coordinates << std::endl;
      }
      Sample points(2, 1);
      points[0] = Point(1, -0.25);
      points[1] = Point(1, 2.25);
      fullprint << "Nearest index(" << points << ")=" << tree.query(points) << std::endl;
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
      KDTree tree(vertices);
      fullprint << "2D mesh=" << mesh2D << std::endl;
      fullprint << "volume=" << mesh2D.getVolume() << std::endl;
      fullprint << "simplices volume=" << mesh2D.computeSimplicesVolume() << std::endl;
      Point point(2, 1.8);
      fullprint << "Nearest index(" << point << ")=" << tree.query(point) << std::endl;
      Sample points(2, 2);
      points[0] = Point(2, -0.25);
      points[1] = Point(2, 2.25);
      fullprint << "Nearest index(" << points << ")=" << tree.query(points) << std::endl;
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
      KDTree tree(vertices);
      fullprint << "3D mesh=" << mesh3D << std::endl;
      fullprint << "volume=" << mesh3D.getVolume() << std::endl;
      fullprint << "simplices volume=" << mesh3D.computeSimplicesVolume() << std::endl;
      Point point(3, 1.8);
      fullprint << "Nearest index(" << point << ")=" << tree.query(point) << std::endl;
      Sample points(2, 3);
      points[0] = Point(3, -0.25);
      points[1] = Point(3, 2.25);
      fullprint << "Nearest index(" << points << ")=" << tree.query(points) << std::endl;
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
