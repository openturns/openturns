//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
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
  setRandomGenerator();

  try
  {
    // Discrete related functions
    // rDiscrete
    Point p(4);
    p[0] = 0.1;
    p[1] = 0.2;
    p[2] = 0.3;
    p[3] = 0.4;
    fullprint << "rDiscrete(" << p << ")=" << DistFunc::rDiscrete(p) << std::endl;
    fullprint << "rDiscrete(" << p << ", 10)=" << DistFunc::rDiscrete(p, 10) << std::endl;
    Point base;
    Indices alias;
    fullprint << "rDiscrete(" << p << ", base, alias)=" << DistFunc::rDiscrete(p, base, alias) << std::endl;
    fullprint << "base=" << base << ", alias=" << alias << std::endl;
    fullprint << "rDiscrete(" << p << ", base, alias, 10)=" << DistFunc::rDiscrete(p, base, alias, 10) << std::endl;
    fullprint << "base=" << base << ", alias=" << alias << std::endl;
    fullprint << "rDiscrete(base=" << base << ", alias=" << alias << ")=" << DistFunc::rDiscrete(base, alias) << std::endl;
    fullprint << "rDiscrete(base=" << base << ", alias=" << alias << ", 10)=" << DistFunc::rDiscrete(base, alias, 10) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
