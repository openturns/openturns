//                                               -*- C++ -*-
/**
 *  @brief The test file of class VonMises for standard methods
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

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    VonMises distribution(0.5, 2.5);
    UnsignedInteger size = 10000;
    Sample sample(distribution.getSample(size));
    // Test with real data
    {
      VonMisesFactory factory(false);
      Distribution estimatedDistribution(factory.build(sample));
      fullprint << "Distribution (raw)           =" << distribution << std::endl;
      fullprint << "Estimated distribution (raw) =" << estimatedDistribution << std::endl;
    }
    // Test with circular data
    {
      VonMisesFactory factory(true);
      Distribution estimatedDistribution(factory.build(sample));
      fullprint << "Distribution (circular)           =" << distribution << std::endl;
      fullprint << "Estimated distribution (circular) =" << estimatedDistribution << std::endl;
      estimatedDistribution = factory.build();
      fullprint << "Default distribution=" << estimatedDistribution << std::endl;
      estimatedDistribution = factory.build(distribution.getParameter());
      fullprint << "Distribution from parameters=" << estimatedDistribution << std::endl;
      VonMises estimatedVonMises(factory.buildAsVonMises(sample));
      fullprint << "VonMises          =" << distribution << std::endl;
      fullprint << "Estimated vonMises=" << estimatedVonMises << std::endl;
      estimatedVonMises = factory.buildAsVonMises();
      fullprint << "Default vonMises=" << estimatedVonMises << std::endl;
      estimatedVonMises = factory.buildAsVonMises(distribution.getParameter());
      fullprint << "VonMises from parameters=" << estimatedVonMises << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
