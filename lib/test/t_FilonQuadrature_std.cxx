//                                               -*- C++ -*-
/**
 *  @brief The test file of class FilonQuadrature
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

typedef Collection<Complex> ComplexCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // First, weight = cos(omega*t)
    {
      SymbolicFunction f("t", "log(1+t)");
      Scalar a = 0.5;
      Scalar b = a + 8.0 * M_PI;
      UnsignedInteger n = 100;
      Scalar omega = 1000.0;
      UnsignedInteger kind = 0;
      FilonQuadrature algo(n, omega, kind);
      Point value(algo.integrate(f, Interval(a, b)));
      fullprint << "kind=" << kind << ", value=" << value << std::endl;
    }
    // Second, weight = sin(omega*t)
    {
      SymbolicFunction f("t", "log(1+t)");
      Scalar a = 0.5;
      Scalar b = a + 8.0 * M_PI;
      UnsignedInteger n = 100;
      Scalar omega = 1000.0;
      UnsignedInteger kind = 1;
      FilonQuadrature algo(n, omega, kind);
      Point value(algo.integrate(f, Interval(a, b)));
      fullprint << "kind=" << kind << ", value=" << value << std::endl;
    }
    // First, weight = exp(I*omega*t)
    {
      SymbolicFunction f("t", "log(1+t)");
      Scalar a = 0.5;
      Scalar b = a + 8.0 * M_PI;
      UnsignedInteger n = 100;
      Scalar omega = 1000.0;
      UnsignedInteger kind = 2;
      FilonQuadrature algo(n, omega, kind);
      Point value(algo.integrate(f, Interval(a, b)));
      fullprint << "kind=" << kind << ", value=" << value << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
