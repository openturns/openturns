//                                               -*- C++ -*-
/**
 *  @brief The test file of class MultivariateHypergeometricFactory for standard methods
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
    MultivariateHypergeometric distribution(5, Point(3, 3));
    UnsignedInteger size = 10000;
    Sample sample(distribution.getSample(size));
    MultivariateHypergeometricFactory factory;
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Distribution          =" << distribution << std::endl;
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    MultivariateHypergeometric estimatedMultivariateHypergeometric(factory.buildAsMultivariateHypergeometric(sample));
    fullprint << "MultivariateHypergeometric          =" << distribution << std::endl;
    fullprint << "Estimated multivariate hypergeometric=" << estimatedMultivariateHypergeometric << std::endl;
    estimatedMultivariateHypergeometric = factory.buildAsMultivariateHypergeometric();
    fullprint << "Default multivariate hypergeometric=" << estimatedMultivariateHypergeometric << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}