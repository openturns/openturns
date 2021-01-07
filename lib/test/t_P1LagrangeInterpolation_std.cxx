//                                               -*- C++ -*-
/**
 * @brief The test file of class P1LagrangeInterpolation for standard methods
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
    const RegularGrid mesh(0.0, 1.0, 4);
    const Point outPoint(1, 2.3);
    const Sample outputPoints(1, outPoint);
    const Point points(mesh.getValues());
    Sample values(mesh.getVerticesNumber(), 3);
    for(UnsignedInteger i = 0; i < values.getSize(); ++i)
    {
      values(i, 0) = points[i];
      values(i, 1) = 2.0 * points[i];
      values(i, 2) = points[i] * points[i];
    }
    P1LagrangeInterpolation interpolation(mesh, Mesh(outputPoints), values.getDimension());
    fullprint << "Interpolation=" << interpolation << std::endl;
    fullprint << "Values at " << outPoint << "=" << interpolation(values) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
