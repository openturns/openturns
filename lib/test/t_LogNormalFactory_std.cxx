//                                               -*- C++ -*-
/**
 *  @brief The test file of class LogNormal for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    LogNormal distribution(1.5, 2.5, -1.5);
    fullprint << "Distribution          =" << distribution << std::endl;
    UnsignedInteger size = 10000;
    Sample sample(distribution.getSample(size));
    LogNormalFactory factory;
    CovarianceMatrix covariance;
    // Distribution estimatedDistribution(factory.build(sample, covariance));
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build(sample, 0);
    fullprint << "Estimated distribution (local likelihood)=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build(sample, 1);
    fullprint << "Estimated distribution (modified moments)=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build(sample, 2);
    fullprint << "Estimated distribution (moments)=" << estimatedDistribution << std::endl;
    // fullprint << "Covariance=" << covariance << std::endl;
    estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build(distribution.getParameter());
    fullprint << "Distribution from parameters=" << estimatedDistribution << std::endl;
    LogNormal estimatedLogNormal(factory.buildAsLogNormal(sample));
    fullprint << "LogNormal          =" << distribution << std::endl;
    fullprint << "Estimated logNormal=" << estimatedLogNormal << std::endl;
    estimatedLogNormal = factory.buildAsLogNormal();
    fullprint << "Default logNormal=" << estimatedLogNormal << std::endl;
    estimatedLogNormal = factory.buildAsLogNormal(distribution.getParameter());
    fullprint << "LogNormal from parameters=" << estimatedLogNormal << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
