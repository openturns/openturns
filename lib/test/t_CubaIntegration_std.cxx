//                                               -*- C++ -*-
/**
 *  @brief The test file of class CubaIntegration
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CubaIntegration.hxx"
#include <cmath>

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Testing four algorithms on a function from R^3 to R.
    {
      const SymbolicFunction f({"x", "y", "z"}, {"sin(x) * cos(y) * exp(z)"});
      const Point lbIntegration({0.0, 0.0, 0.0});
      const Point ubIntegration({1.0, 1.0, 1.0});
      const Description routines(CubaIntegration::GetAlgorithmNames());
      const Scalar valueRef = -sin(1.) * (cos(1.) - 1.) * (M_E - 1.);
      for (UnsignedInteger i = 0 ; i < routines.getSize() ; i++)
      {
          CubaIntegration algo(routines[i]);
          const Point value(algo.integrate(f, Interval(lbIntegration, ubIntegration)));
          assert_almost_equal(value[0], valueRef, 1.e-3, 1.e-3);
      }
    }

    // Testing three algorithms on a function from R^3 to R^2.
    {
      const SymbolicFunction f({"x", "y", "z"},
                               {"exp(-(x * x + y * y + z *z))",
                                "(x / 2 * x / 2 + y / 3 * y / 3 + z / 4 * z / 4 - 1.125)"}
                              );
      const Point lbIntegration({0.0, 0.0, 1.0});
      const Point ubIntegration({2.0, 3.0, 4.0});
      const Description routines(CubaIntegration::GetAlgorithmNames());
      const Point valueRef({0.108972129575688278, -0.375});
      for (UnsignedInteger i = 0 ; i < routines.getSize() ; i++)
      {
          if (routines[i] == "suave")
          {
            // Suave is quite inaccurate for these integrands, skipping.
            continue;
          }

          CubaIntegration algo(routines[i]);
          if (routines[i] == "vegas")
          {
            /* Vegas seemingly needs more favorable parameters there */
            algo.setMaximumRelativeError(5.e-3);
            algo.setMaximumAbsoluteError(5.e-3);
            algo.setMaximumEvaluationNumber(10000000);
          }
          const Point value(algo.integrate(f, Interval(lbIntegration, ubIntegration)));
          assert_almost_equal(value, valueRef, 1.e-3, 1.e-3);
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
