//                                               -*- C++ -*-
/**
 *  @brief The test file of class BoundingVolumeHierarchy for standard methods
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

  Description inputVar;
  inputVar.add("x");
  inputVar.add("y");
  inputVar.add("z");
  Description formula;
  formula.add("0.05+0.95*x");
  formula.add("y-0.1*x*sin(x)");
  formula.add("z+0.1*x*sin(x)");
  for (UnsignedInteger dimension = 1;  dimension < 4; ++dimension)
  {
    const Interval interval(Point(dimension, 0.0), Point(dimension, 10.0));
    const UnsignedInteger nrIntervals = std::exp(std::log(10000) / dimension);
    Mesh mesh(IntervalMesher(Indices(dimension, nrIntervals)).build(interval));
    SymbolicFunction f(Description(Collection<String>(inputVar.begin(), inputVar.begin() + dimension)),
                       Description(Collection<String>(formula.begin(), formula.begin() + dimension)));
    fullprint << "f=" << f << std::endl;
    const Sample meshVertices(f(mesh.getVertices()));
    mesh.setVertices(meshVertices);

    const IndicesCollection simplices(mesh.getSimplices());
    const BoundingVolumeHierarchy bvh(meshVertices, simplices, 3);
    fullprint << "bvh=" << bvh << std::endl;

    RandomGenerator::SetSeed(0);
    Collection<Distribution> coll(dimension, Uniform(-1.0, 11.0));
    const Sample test(ComposedDistribution(coll).getSample(100));

    Point coordinates;
    for (UnsignedInteger i = 0; i < test.getSize(); ++i)
    {
      UnsignedInteger index = bvh.query(test[i]);
      if (index >= simplices.getSize())
        fullprint << i << " is outside" << std::endl;
      else
      {
        if (!mesh.checkPointInSimplexWithCoordinates(test[i], index, coordinates))
        {
          fullprint << "Wrong simplex found for " << test[i] << " (index=" << index << ") " << " barycentric coordinates=" << coordinates << std::endl;
          return ExitCode::Error;
        }
        Point difference(test[i] - coordinates[dimension] * meshVertices[simplices(index, dimension)]);
        for (UnsignedInteger d = 0; d < dimension; ++d)
        {
          if (coordinates[d] < 0.0 || coordinates[d] > 1.0)
          {
            fullprint << "Wrong barycentric coordinates found found for " << test[i] << " (index=" << index << ") barycentric coordinates=" << coordinates << std::endl;
            return ExitCode::Error;
          }
          difference -= coordinates[d] * meshVertices[simplices(index, d)];
        }
        if (difference.norm1() > 1.e-10)
        {
          fullprint << "Wrong barycentric coordinates found found for " << test[i] << " (index=" << index << ") barycentric coordinates=" << coordinates << std::endl;
          return ExitCode::Error;
        }
      }
    }
    const Indices result(bvh.query(test));
    for (UnsignedInteger i = 0; i < result.getSize(); ++i)
    {
      if (result[i] >= simplices.getSize())
        fullprint << i << " is outside" << std::endl;
      else
      {
        if (!mesh.checkPointInSimplexWithCoordinates(test[i], result[i], coordinates))
        {
          fullprint << "Wrong simplex found for " << test[i] << " (index=" << result[i] << ")" << std::endl;
          return ExitCode::Error;
        }
      }
    }
  }

  return ExitCode::Success;
}
