//                                               -*- C++ -*-
/**
 *  @brief The test file of class PlackettCopula for standard methods
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
    PlackettCopula distribution(1.5);
    UnsignedInteger size = 1000;
    Sample sample(distribution.getSample(size));
    PlackettCopulaFactory factory;
    CovarianceMatrix covariance;
    // Distribution estimatedDistribution(factory.build(sample, covariance));
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Distribution          =" << distribution << std::endl;
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    // fullprint << "Covariance=" << covariance << std::endl;
    estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build(distribution.getParameter());
    fullprint << "Distribution from parameters=" << estimatedDistribution << std::endl;
    PlackettCopula estimatedPlackettCopula(factory.buildAsPlackettCopula(sample));
    fullprint << "PlackettCopula          =" << distribution << std::endl;
    fullprint << "Estimated plackettCopula=" << estimatedPlackettCopula << std::endl;
    estimatedPlackettCopula = factory.buildAsPlackettCopula();
    fullprint << "Default plackettCopula=" << estimatedPlackettCopula << std::endl;
    estimatedPlackettCopula = factory.buildAsPlackettCopula(distribution.getParameter());
    fullprint << "PlackettCopula from parameters=" << estimatedPlackettCopula << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
