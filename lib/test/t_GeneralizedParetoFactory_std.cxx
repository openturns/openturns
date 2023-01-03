//                                               -*- C++ -*-
/**
 *  @brief The test file of class GeneralizedPareto for standard methods
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
  PlatformInfo::SetNumericalPrecision(4);
  try
  {
    Point xi(3);
    xi[0] = -0.75;
    xi[1] = 0.0;
    xi[2] = 0.75;
    UnsignedInteger size = 10000;
    CovarianceMatrix covariance;
    GeneralizedParetoFactory factory;
    GeneralizedPareto distribution;
    for (UnsignedInteger i = 0; i < 3; ++i)
    {
      distribution = GeneralizedPareto(2.5, xi[i], 0.5);
      Sample sample(distribution.getSample(size));
      // Distribution estimatedDistribution(factory.build(sample, covariance));
      Distribution estimatedDistribution(factory.build(sample));
      fullprint << "Distribution          =" << distribution << std::endl;
      fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
      GeneralizedPareto estimatedGeneralizedPareto(factory.buildAsGeneralizedPareto(sample));
      fullprint << "GeneralizedPareto          =" << distribution << std::endl;
      fullprint << "Estimated generalizedPareto=" << estimatedGeneralizedPareto << std::endl;
      // fullprint << "Covariance=" << covariance << std::endl;
    }
    Distribution estimatedDistribution(factory.build());
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build(distribution.getParameter());
    fullprint << "Distribution from parameters=" << estimatedDistribution << std::endl;
    GeneralizedPareto estimatedGeneralizedPareto(factory.buildAsGeneralizedPareto());
    fullprint << "Default generalizedPareto=" << estimatedGeneralizedPareto << std::endl;
    estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto(distribution.getParameter());
    fullprint << "GeneralizedPareto from parameters=" << estimatedGeneralizedPareto << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
