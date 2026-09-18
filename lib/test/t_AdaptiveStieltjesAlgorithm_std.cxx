//                                               -*- C++ -*-
/**
 *  @brief The test file of class AdaptiveStieltjesAlgorithm for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
    const UnsignedInteger iMax = 5;

    // Test 1: Uniform -> LegendreFactory (centered, symmetric case)
    {
      Uniform distribution;
      LegendreFactory reference;
      AdaptiveStieltjesAlgorithm ada(distribution);
      fullprint << ada << std::endl;
      for (UnsignedInteger i = 0; i < iMax; ++i)
      {
        Point refCoeff(reference.getRecurrenceCoefficients(i));
        Point adaCoeff(ada.getRecurrenceCoefficients(i));
        fullprint << distribution.getClassName() << " Reference(" << i << ")=" << refCoeff << std::endl;
        fullprint << distribution.getClassName() << " AdaStielj(" << i << ")=" << adaCoeff << std::endl;
        assert_almost_equal(adaCoeff, refCoeff);
      }
    }

    // Test 2: Beta(0.5, 2.5, -1, 1) -> JacobiFactory (non-centered case)
    {
      Beta distribution(0.5, 2.5, -1.0, 1.0);
      JacobiFactory reference(0.5, 2.5);
      AdaptiveStieltjesAlgorithm ada(distribution);
      fullprint << ada << std::endl;
      for (UnsignedInteger i = 0; i < iMax; ++i)
      {
        Point refCoeff(reference.getRecurrenceCoefficients(i));
        Point adaCoeff(ada.getRecurrenceCoefficients(i));
        fullprint << distribution.getClassName() << " Reference(" << i << ")=" << refCoeff << std::endl;
        fullprint << distribution.getClassName() << " AdaStielj(" << i << ")=" << adaCoeff << std::endl;
        assert_almost_equal(adaCoeff, refCoeff);
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
