//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussKronrod
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

namespace OT
{
class UniVariateSymbolicFunction: public UniVariateFunctionImplementation
{
public:
  UniVariateSymbolicFunction(const SymbolicFunction & f)
    : UniVariateFunctionImplementation()
    , f_(f)
  {
    // Nothing to do
  }

  UniVariateSymbolicFunction * clone() const
  {
    return new UniVariateSymbolicFunction(*this);
  }

  Scalar operator()(const Scalar x) const
  {
    return f_(Point(1, x))[0];
  }

private:
  SymbolicFunction f_;

};

} // OT

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // First, a smooth function
    {
      SymbolicFunction f("x", "sin(x)");
      Scalar a = -2.5;
      Scalar b = 4.5;
      // Default parameters
      GaussKronrod algo;
      GaussKronrodRule::GaussKronrodPair rules[5] = {GaussKronrodRule::G3K7, GaussKronrodRule::G7K15, GaussKronrodRule::G11K23, GaussKronrodRule::G15K31, GaussKronrodRule::G25K51};
      for (UnsignedInteger i = 0; i < 5; ++i)
      {
        algo.setRule(rules[i]);
	fullprint << "Algo=" << algo << std::endl;
	{
          // High-level interface
          Scalar error = -1.0;
          Scalar value = algo.integrate(f, Interval(a, b), error)[0];
          Scalar ref = cos(a) - cos(b);
          fullprint << "value    (general)=" << value << ", ref=" << ref << ", true error below bound? " << (std::abs(ref - value) < algo.getMaximumError() ? "true" : "false") << ", estimated error below bound? " << (error < algo.getMaximumError() ? "true" : "false") << std::endl;
        }
        {
          // High-level interface
          Scalar value = algo.integrate(UniVariateFunction(UniVariateSymbolicFunction(f)), a, b);
          Scalar ref = cos(a) - cos(b);
          fullprint << "value (univariate)=" << value << ", ref=" << ref << ", true error below bound? " << (std::abs(ref - value) < algo.getMaximumError() ? "true" : "false") << std::endl;
        }
        // Low-level interface
        //         Point ai;
        //         Point bi;
        //         Sample fi;
        //         Point ei;
        //         Scalar value2(algo.integrate(f, a, b, error, ai, bi, fi, ei)[0]);
        //         ai.add(b);
        //         Graph g(f.draw(a, b, 512));
        //         Cloud lower(ai, Point(ai.getDimension()));
        //         lower.setColor("magenta");
        //         g.add(lower);
        //         g.draw(String(OSS() << "Smooth_function_adaptation_" << i));
      }
    }
    // Second, a piecewise smooth function
    {
      SymbolicFunction f("x", "abs(sin(x))");
      Scalar a = -2.5;
      Scalar b = 4.5;
      GaussKronrod algo;
      GaussKronrodRule::GaussKronrodPair rules[5] = {GaussKronrodRule::G3K7, GaussKronrodRule::G7K15, GaussKronrodRule::G11K23, GaussKronrodRule::G15K31, GaussKronrodRule::G25K51};
      for (UnsignedInteger i = 0; i < 5; ++i)
      {
        algo.setRule(rules[i]);
        fullprint << "Algo=" << algo << std::endl;
        Scalar error = -1.0;
        Scalar value = algo.integrate(f, Interval(a, b), error)[0];
        Scalar ref = 4.0 + cos(b) - cos(a);
        fullprint << "value=" << value << ", ref=" << ref << ", true error below bound? " << (std::abs(ref - value) < algo.getMaximumError() ? "true" : "false") << ", estimated error below bound? " << (error < algo.getMaximumError() ? "true" : "false") << std::endl;

        // Low-level interface
        //         Point ai;
        //         Point bi;
        //         Sample fi;
        //         Point ei;
        //         Scalar value2(algo.integrate(f, a, b, error, ai, bi, fi, ei)[0]);
        //         ai.add(b);
        //         Graph g(f.draw(a, b, 512));
        //         Cloud lower(ai, Point(ai.getDimension()));
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
