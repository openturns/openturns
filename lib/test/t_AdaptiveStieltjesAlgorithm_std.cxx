//                                               -*- C++ -*-
/**
 *  @brief The test file of class AdaptiveStieltjesAlgorithm for standard methods
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

using namespace OT;
using namespace OT::Test;

Point clean(Point in)
{
  UnsignedInteger dim = in.getDimension();
  for(UnsignedInteger i = 0; i < dim; i++)
  {
    if (std::abs(in[i]) < 1.e-10) in[i] = 0.0;
    in[i] = 1.0e-4 * round(1.0e4 * in[i]);
  }
  return in;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    const UnsignedInteger iMax = 5;
    {
      Uniform distribution;
      LegendreFactory algo0;
      AdaptiveStieltjesAlgorithm algo1(distribution);
      fullprint << algo1 << std::endl;
      // Centered case
      for (UnsignedInteger i = 0; i < iMax; ++i)
      {
        fullprint << distribution.getClassName() << " Reference(" << i << ")=" << clean(algo0.getRecurrenceCoefficients(i)) << std::endl;
        fullprint << distribution.getClassName() << " AdaStielj(" << i << ")=" << clean(algo1.getRecurrenceCoefficients(i)) << std::endl;
      }
    }
    {
      // Non-centered case
      Beta distribution(0.5, 2.5, -1.0, 1.0);
      JacobiFactory algo0(-0.5, 1.5);
      AdaptiveStieltjesAlgorithm algo1(distribution);
      fullprint << algo1 << std::endl;
      for (UnsignedInteger i = 0; i < iMax; ++i)
      {
        fullprint << distribution.getClassName() << " Reference(" << i << ")=" << clean(algo0.getRecurrenceCoefficients(i)) << std::endl;
        fullprint << distribution.getClassName() << " AdaStielj(" << i << ")=" << clean(algo1.getRecurrenceCoefficients(i)) << std::endl;
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
