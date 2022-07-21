//                                               -*- C++ -*-
/**
 * @brief The test file of class P1LagrangeEvaluation for standard methods
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

  try
  {
    RegularGrid mesh(0.0, 1.0, 4);
    Sample values(0, 1);
    values.add(Point(1, 0.5));
    values.add(Point(1, 1.5));
    values.add(Point(1, 1.0));
    values.add(Point(1, -0.5));
    P1LagrangeEvaluation evaluation(Field(mesh, values));
    fullprint << "Evaluation=" << evaluation << std::endl;
    Point inPoint(1, 2.3);
    fullprint << "Value at " << inPoint << "=" << evaluation(inPoint) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
