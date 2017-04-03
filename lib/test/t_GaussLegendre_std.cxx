//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussKronrod
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

typedef Collection<NumericalComplex> NumericalComplexCollection;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    NumericalMathFunction f("x", "sin(x)");
    NumericalScalar a = -2.5;
    NumericalScalar b = 4.5;
    // Default parameters
    GaussLegendre algo;
    fullprint << "Algo=" << algo << std::endl;
    // High-level interface
    algo = GaussLegendre(Indices(1, 20));
    NumericalScalar value = algo.integrate(f, Interval(a, b))[0];
    NumericalScalar ref = cos(a) - cos(b);
    fullprint << "value=" << value << ", ref=" << ref << std::endl;
    // Low-level interface
    NumericalSample adaptedNodes;
    value = algo.integrateWithNodes(f, Interval(a, b), adaptedNodes)[0];
    fullprint << "value=" << value << ", ref=" << ref << ", adaptedNodes=" << adaptedNodes << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
