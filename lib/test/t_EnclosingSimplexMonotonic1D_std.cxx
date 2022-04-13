//                                               -*- C++ -*-
/**
 *  @brief The test file of class EnclosingSimplexMonotonic1D for standard methods
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

  const UnsignedInteger nrVertices = 100;
  Sample vertices(Normal().getSample(nrVertices).sort());
  IndicesCollection simplices(nrVertices - 1,  2);
  for(UnsignedInteger i = 0;  i < nrVertices - 1; ++i)
  {
    simplices(i, 0) = i;
    simplices(i, 1) = i + 1;
  }
  Collection<Mesh> meshes1D;
  meshes1D.add(Mesh(vertices, simplices));
  vertices *= -1.0;
  meshes1D.add(Mesh(vertices, simplices));
  for(UnsignedInteger m = 0; m < meshes1D.getSize(); ++m)
  {
    const Mesh mesh = meshes1D[m];
    const Scalar lowerBound = mesh.getLowerBound()[0];
    const Scalar upperBound = mesh.getUpperBound()[0];
    const UnsignedInteger n  = mesh.getVerticesNumber() - 1;
    fullprint << "mesh = " << mesh << std::endl;
    const Sample meshVertices(mesh.getVertices());
    const EnclosingSimplexMonotonic1D algo(meshVertices);

    RandomGenerator::SetSeed(0);
    const Sample test(Uniform(-3.0, 3.0).getSample(1000));

    Point coordinates;
    for (UnsignedInteger i = 0; i < test.getSize(); ++i)
    {
      UnsignedInteger index = algo.query(test[i]);
      if (test(i, 0) < lowerBound || test(i, 0) > upperBound)
      {
        if (index < n)
        {
          fullprint << "Point " << test[i] << " should be outside but query returned index " << index << std::endl;
          return ExitCode::Error;
        }
      }
      else
      {
        if (index >= n)
        {
          fullprint << "Point " << test[i] << " should be inside, query returned index " << index << std::endl;
          return ExitCode::Error;
        }
        if (!mesh.checkPointInSimplexWithCoordinates(test[i], index, coordinates))
        {
          fullprint << "Wrong simplex found for " << test[i] << " (index=" << index << ") barycentric coordinates=" << coordinates << std::endl;
          return ExitCode::Error;
        }
        if (coordinates[0] < 0.0 || coordinates[0] > 1.0 || coordinates[1] < 0.0 || coordinates[1] > 1.0)
        {
          fullprint << "Wrong barycentric coordinates found found for " << test[i] << " (index=" << index << ") barycentric coordinates=" << coordinates << std::endl;
          return ExitCode::Error;
        }
        if (std::abs(test(i, 0) - coordinates[0] * meshVertices(index, 0) - coordinates[1] * meshVertices(index + 1, 0)) > 1.e-10)
        {
          fullprint << "Wrong barycentric coordinates found found for " << test[i] << " (index=" << index << ") barycentric coordinates=" << coordinates << std::endl;
          return ExitCode::Error;
        }
      }
    }
  }
  return ExitCode::Success;
}
