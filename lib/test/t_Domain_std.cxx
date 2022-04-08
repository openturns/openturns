//                                               -*- C++ -*-
/**
 *  @brief The test file of class Domain for standard methods
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
    // The 1D interface
    UnsignedInteger dim = 2;
    Point a(dim, -1.0);
    Point b(dim, 2.0);
    Domain domain  = Interval(a, b);
    Point p1(dim, 0.5);
    Point p2(dim, 2.5);
    fullprint << "Domain= " << domain << std::endl;
    // Accessors
    fullprint << "Dimension= " << domain.getDimension() << std::endl;
    // Contains
    fullprint << "is point " << p1 << " inside " << domain << "? " << (domain.contains(p1) ? "yes" : "no") << std::endl;
    fullprint << "is point " << p2 << " inside " << domain << "? " << (domain.contains(p2) ? "yes" : "no") << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
