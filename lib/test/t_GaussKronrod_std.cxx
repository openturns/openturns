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
    // First, a smooth function
    {
      NumericalMathFunction f("x", "sin(x)");
      NumericalScalar a = -2.5;
      NumericalScalar b = 4.5;
      // Default parameters
      GaussKronrod algo;
      GaussKronrodRule::GaussKronrodPair rules[5] = {GaussKronrodRule::G3K7, GaussKronrodRule::G7K15, GaussKronrodRule::G11K23, GaussKronrodRule::G15K31, GaussKronrodRule::G25K51};
      for (UnsignedInteger i = 0; i < 5; ++i)
      {
        algo.setRule(rules[i]);
        fullprint << "Algo=" << algo << std::endl;
        // High-level interface
        NumericalScalar error = -1.0;
        NumericalScalar value = algo.integrate(f, Interval(a, b), error)[0];
        NumericalScalar ref = cos(a) - cos(b);
        fullprint << "value=" << value << ", ref=" << ref << ", true error below bound? " << (std::abs(ref - value) < algo.getMaximumError() ? "true" : "false") << ", estimated error below bound? " << (error < algo.getMaximumError() ? "true" : "false") << std::endl;
        // Low-level interface
//         NumericalPoint ai;
//         NumericalPoint bi;
//         NumericalSample fi;
//         NumericalPoint ei;
//         NumericalScalar value2(algo.integrate(f, a, b, error, ai, bi, fi, ei)[0]);
//         ai.add(b);
//         Graph g(f.draw(a, b, 512));
//         Cloud lower(ai, NumericalPoint(ai.getDimension()));
//         lower.setColor("magenta");
//         g.add(lower);
//         g.draw(String(OSS() << "Smooth_function_adaptation_" << i));
      }
    }
    // Second, a piecewise smooth function
    {
      NumericalMathFunction f("x", "abs(sin(x))");
      NumericalScalar a = -2.5;
      NumericalScalar b = 4.5;
      GaussKronrod algo;
      GaussKronrodRule::GaussKronrodPair rules[5] = {GaussKronrodRule::G3K7, GaussKronrodRule::G7K15, GaussKronrodRule::G11K23, GaussKronrodRule::G15K31, GaussKronrodRule::G25K51};
      for (UnsignedInteger i = 0; i < 5; ++i)
      {
        algo.setRule(rules[i]);
        fullprint << "Algo=" << algo << std::endl;
        NumericalScalar error = -1.0;
        NumericalScalar value = algo.integrate(f, Interval(a, b), error)[0];
        NumericalScalar ref = 4.0 + cos(b) - cos(a);
        fullprint << "value=" << value << ", ref=" << ref << ", true error below bound? " << (std::abs(ref - value) < algo.getMaximumError() ? "true" : "false") << ", estimated error below bound? " << (error < algo.getMaximumError() ? "true" : "false") << std::endl;

        // Low-level interface
//         NumericalPoint ai;
//         NumericalPoint bi;
//         NumericalSample fi;
//         NumericalPoint ei;
//         NumericalScalar value2(algo.integrate(f, a, b, error, ai, bi, fi, ei)[0]);
//         ai.add(b);
//         Graph g(f.draw(a, b, 512));
//         Cloud lower(ai, NumericalPoint(ai.getDimension()));
//         lower.setColor("magenta");
//         g.add(lower);
//         g.draw(String(OSS() << "Nonsmooth_function_adaptation_" << i));
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
