//                                               -*- C++ -*-
/**
 *  @brief The test file of class KDTree for standard methods
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

namespace
{
Scalar debug_squared_minimum_distance(const Point & point, const Sample & sample)
{
  Scalar result = SpecFunc::MaxScalar;
  for(UnsignedInteger i = 0; i < sample.getSize(); ++i)
  {
    const Scalar distance2 = Point(sample[i] - point).normSquare();
    if (distance2 < result) result = distance2;
  }
  return result;
}
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  const Sample sample(Normal(3).getSample(10));
  const KDTree tree(sample);
  fullprint << "tree=" << tree << std::endl;
  const Sample test(Normal(3).getSample(20));
  for (UnsignedInteger i = 0; i < test.getSize(); ++i)
  {
    const Scalar expected = debug_squared_minimum_distance(test[i], sample);
    UnsignedInteger index = tree.getNearestNeighbourIndex(test[i]);
    Point neighbour(tree.getNearestNeighbour(test[i]));
    fullprint << "Nearest neighbour of " << test[i] << "=" << neighbour << " (index=" << index << ")" << std::endl;
    if (std::abs(Point(test[i] - sample[index]).normSquare() - expected) > 1.e-5)
    {
      fullprint << "Wrong nearest neighbour of " << test[i] << " (index=" << index << ")" << std::endl;
      return ExitCode::Error;
    }
    if (std::abs(Point(test[i] - neighbour).normSquare() - expected) > 1.e-5)
    {
      fullprint << "Wrong nearest neighbour of " << test[i] << "=" << neighbour << std::endl;
      return ExitCode::Error;
    }
  }

  const UnsignedInteger k = 4;
  for (UnsignedInteger i = 0; i < test.getSize(); ++i)
  {
    Indices indices = tree.getNearestNeighboursIndices(test[i], k, true);
    fullprint << k << " nearest neighbours of " << test[i] << "=" << " (indices=" << indices << ")" << std::endl;
    // Check that returned neighbours are sorted
    Scalar last = 0.0;
    for(UnsignedInteger j = 0; j < indices.getSize(); ++j)
    {
      const Scalar current = Point(test[i] - sample[indices[j]]).normSquare();
      if (last > current)
      {
        fullprint << "Wrong nearest neighbour of " << test[i] << " (indices=" << indices << ")" << std::endl;
        return ExitCode::Error;
      }
      last = current;
    }
  }

  return ExitCode::Success;
}
