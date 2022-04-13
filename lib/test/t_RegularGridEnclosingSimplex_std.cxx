//                                               -*- C++ -*-
/**
 *  @brief The test file of class RegularGridEnclosingSimplex for standard methods
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

  Collection<RegularGrid> grids;
  // Ascending grid with positive values
  grids.add(RegularGrid(1.0, 0.1, 20));
  // Ascending grid with negative values
  grids.add(RegularGrid(-3.0, 0.1, 20));
  // Decending grid with positive values
  grids.add(RegularGrid(3.0, -0.1, 20));
  // Decending grid with negative values
  grids.add(RegularGrid(-1.0, -0.1, 20));
  // Ascending grid with positive and negative values
  grids.add(RegularGrid(-1.0, 0.13, 20));
  // Decending grid with positive and negative values
  grids.add(RegularGrid(1.0, -0.13, 20));
  for(UnsignedInteger g = 0; g < grids.getSize(); ++g)
  {
    const RegularGrid regularGrid = grids[g];
    const Scalar lowerBound = regularGrid.getLowerBound()[0];
    const Scalar upperBound = regularGrid.getUpperBound()[0];
    const UnsignedInteger n  = regularGrid.getSimplicesNumber();
    fullprint << "regularGrid = " << regularGrid << " lowerBound=" << lowerBound << " upperBound=" << upperBound << " " << n << " simplices" << std::endl;
    const RegularGridEnclosingSimplex algo(regularGrid);

    RandomGenerator::SetSeed(0);
    const Sample test(Uniform(lowerBound - 0.2 * (upperBound - lowerBound), upperBound + 0.2 * (upperBound - lowerBound)).getSample(1000));

    Point coordinates;
    const Sample vertices(regularGrid.getVertices());
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
        if (!regularGrid.checkPointInSimplexWithCoordinates(test[i], index, coordinates))
        {
          fullprint << "Wrong simplex found for " << test[i] << " (index=" << index << ") barycentric coordinates=" << coordinates << std::endl;
          return ExitCode::Error;
        }
        if (coordinates[0] < 0.0 || coordinates[0] > 1.0 || coordinates[1] < 0.0 || coordinates[1] > 1.0)
        {
          fullprint << "Wrong barycentric coordinates found found for " << test[i] << " (index=" << index << ") barycentric coordinates=" << coordinates << std::endl;
          return ExitCode::Error;
        }
        if (std::abs(test(i, 0) - coordinates[0] * vertices(index, 0) - coordinates[1] * vertices(index + 1, 0)) > 1.e-10)
        {
          fullprint << "Wrong barycentric coordinates found found for " << test[i] << " (index=" << index << ") barycentric coordinates=" << coordinates << std::endl;
          return ExitCode::Error;
        }
      }
    }
  }
  return ExitCode::Success;
}
