//                                               -*- C++ -*-
/**
 *  @brief The test file of class Trapezoidal for standard methods
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
  setRandomGenerator();

  try
  {
    Trapezoidal distribution( 1.0, 2.3, 4.5, 5.0 );
    UnsignedInteger size = 10000;
    Sample sample(distribution.getSample(size));
    TrapezoidalFactory factory;
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Distribution          =" << distribution << std::endl;
    int oldPrecision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision( 4 );
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    PlatformInfo::SetNumericalPrecision( oldPrecision );
    estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build(distribution.getParameter());
    fullprint << "Distribution from parameters=" << estimatedDistribution << std::endl;
    Trapezoidal estimatedTrapezoidal(factory.buildAsTrapezoidal(sample));
    fullprint << "Trapezoidal          =" << distribution << std::endl;
    oldPrecision = PlatformInfo::GetNumericalPrecision();
    PlatformInfo::SetNumericalPrecision( 4 );
    fullprint << "Estimated trapezoidal=" << estimatedTrapezoidal << std::endl;
    PlatformInfo::SetNumericalPrecision( oldPrecision );
    estimatedTrapezoidal = factory.buildAsTrapezoidal();
    fullprint << "Default trapezoidal=" << estimatedTrapezoidal << std::endl;
    estimatedTrapezoidal = factory.buildAsTrapezoidal(distribution.getParameter());
    fullprint << "Trapezoidal from parameters=" << estimatedTrapezoidal << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
