//                                               -*- C++ -*-
/**
 *  @brief The test file of class NaiveNearestNeighbour for standard methods
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

namespace
{
UnsignedInteger debug_squared_minimum_distance(const Point & point, const Sample & sample)
{
  Scalar result = SpecFunc::MaxScalar;
  UnsignedInteger bestIndex = sample.getSize();
  for(UnsignedInteger i = 0; i < sample.getSize(); ++i)
  {
    const Scalar distance2 = Point(sample[i] - point).normSquare();
    if (distance2 < result)
    {
      result = distance2;
      bestIndex = i;
    }
  }
  return bestIndex;
}
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  const Sample sample(Normal().getSample(10));
  const NearestNeighbour1D tree(sample);
  fullprint << "tree=" << tree << std::endl;
  const Sample test(Normal().getSample(20));
  for (UnsignedInteger i = 0; i < test.getSize(); ++i)
  {
    const UnsignedInteger expected = debug_squared_minimum_distance(test[i], sample);
    const UnsignedInteger index = tree.query(test[i]);
    const Point neighbour(sample[tree.query(test[i])]);
    fullprint << "Nearest neighbour of " << test[i] << "=" << neighbour << " (index=" << index << ")" << std::endl;
    if (index != expected)
    {
      fullprint << "Wrong nearest neighbour of " << test[i] << " (index=" << index << ", expected=" << expected << ")" << std::endl;
      return ExitCode::Error;
    }
  }

  const UnsignedInteger k = 4;
  for (UnsignedInteger i = 0; i < test.getSize(); ++i)
  {
    Indices indices = tree.queryK(test[i], k, true);
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
