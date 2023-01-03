//                                               -*- C++ -*-
/**
 *  @brief The test file of class KDTree for standard methods
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

  /*Using deltaT constructor */
  const Scalar start = -1.0;
  const Scalar step = 0.1;
  const UnsignedInteger n = 21;
  const RegularGrid regularGrid(start, step, n);
  fullprint << "regularGrid = " << regularGrid << std::endl;
  const RegularGridNearestNeighbour regularGridNearestNeighbourAlgorithm(regularGrid);
  fullprint << "regularGridNearestNeighbourAlgorithm=" << regularGridNearestNeighbourAlgorithm << std::endl;
  const Sample test(Normal(1).getSample(20));
  for (UnsignedInteger i = 0; i < test.getSize(); ++i)
  {
    const UnsignedInteger index = regularGridNearestNeighbourAlgorithm.query(test[i]);
    const Point neighbour(1, regularGrid.getValue(index));
    fullprint << "Nearest neighbour of " << test[i] << "=" << neighbour << " (index=" << index << ")" << std::endl;
  }

  const UnsignedInteger k = 4;
  const Point values(regularGrid.getValues());
  for (UnsignedInteger i = 0; i < test.getSize(); ++i)
  {
    const Indices indices = regularGridNearestNeighbourAlgorithm.queryK(test[i], k, true);
    fullprint << k << " nearest neighbours of " << test[i] << "=" << " (indices=" << indices << ")" << std::endl;
    // Check that returned neighbours are sorted
    Scalar last = 0.0;
    for(UnsignedInteger j = 0; j < indices.getSize(); ++j)
    {
      const Scalar current = (test(i, 0) - values[indices[j]]) * (test(i, 0) - values[indices[j]]);
      if (last > current)
      {
        fullprint << "Wrong nearest neighbour of " << test[i] << " (indices=" << indices << ")" << std::endl;
        return ExitCode::Error;
      }
      last = current;
    }
  }
  Indices indices = regularGridNearestNeighbourAlgorithm.queryK(test[0], n, true);
  fullprint << n << " nearest neighbours of " << test[0] << "=" << " (indices=" << indices << ")" << std::endl;

  return ExitCode::Success;
}
