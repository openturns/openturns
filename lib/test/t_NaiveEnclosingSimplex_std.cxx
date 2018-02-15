//                                               -*- C++ -*-
/**
 *  @brief The test file of class BoundingVolumeHierarchy for standard methods
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

  Interval interval(Point(2, 0.0), Point(2, 10.0));
  Mesh mesh(IntervalMesher(Indices(2, 30)).build(interval));
  Description inputVar;
  inputVar.add("x");
  inputVar.add("y");
  Description formula;
  formula.add("x + 0.5*sin(y)");
  formula.add("y-0.1*x*sin(x)");
  SymbolicFunction f(inputVar, formula);
  mesh.setVertices(f(mesh.getVertices()));

  const Collection<Indices> simplices(mesh.getSimplices());
  const NaiveEnclosingSimplex naive(mesh.getVertices(), simplices);
  fullprint << "naive=" << naive << std::endl;

  RandomGenerator::SetSeed(0);
  Collection<Distribution> coll;
  coll.add(Uniform(-1.0, 11.0));
  coll.add(Uniform(-1.0, 11.0));
  const Sample test(ComposedDistribution(coll).getSample(100));

  Point coordinates(4);
  for (UnsignedInteger i = 0; i < test.getSize(); ++i)
  {
    UnsignedInteger index = naive.getEnclosingSimplexIndex(test[i]);
    if (index >= simplices.getSize())
      fullprint << i << " is outside" << std::endl;
    else
    {
      if (!mesh.checkPointInSimplexWithCoordinates(test[i], index, coordinates))
      {
        fullprint << "Wrong simplex found for " << test[i] << " (index=" << index << ") " << simplices[index] << " barycentric coordinates=" << coordinates << std::endl;
        return ExitCode::Error;
      }
    }
  }
  const Indices result(naive.getEnclosingSimplexIndex(test));
  for (UnsignedInteger i = 0; i < result.getSize(); ++i)
  {
    if (result[i] >= simplices.getSize())
      fullprint << i << " is outside" << std::endl;
    else
    {
      if (!mesh.checkPointInSimplexWithCoordinates(test[i], result[i], coordinates))
      {
        fullprint << "Wrong simplex found for " << test[i] << " (index=" << result[i] << ")" << simplices[result[i]] << std::endl;
        return ExitCode::Error;
      }
    }
  }

  return ExitCode::Success;
}
