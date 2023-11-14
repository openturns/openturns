//                                               -*- C++ -*-
/**
 *  @brief The test file of class Chi for standard methods
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
    Chi distribution(0.5);
    UnsignedInteger size = 10000;
    Sample sample(distribution.getSample(size));
    ChiFactory factory;
    CovarianceMatrix covariance;
    // Distribution estimatedDistribution(factory.build(sample, covariance));
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Distribution          =" << distribution << std::endl;
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    // fullprint << "Covariance=" << covariance << std::endl;
    distribution = Chi(1.0);
    sample = distribution.getSample(size);
    // estimatedDistribution = factory.build(sample, covariance);
    estimatedDistribution = factory.build(sample);
    fullprint << "Distribution          =" << distribution << std::endl;
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    // fullprint << "Covariance=" << covariance << std::endl;
    distribution = Chi(2.5);
    sample = distribution.getSample(size);
    // estimatedDistribution = factory.build(sample, covariance);
    estimatedDistribution = factory.build(sample);
    fullprint << "Distribution          =" << distribution << std::endl;
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    // fullprint << "Covariance=" << covariance << std::endl;
    estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    estimatedDistribution = factory.build(distribution.getParameter());
    fullprint << "Distribution from parameters=" << estimatedDistribution << std::endl;
    Chi estimatedChi(factory.buildAsChi(sample));
    fullprint << "Chi          =" << distribution << std::endl;
    fullprint << "Estimated chi=" << estimatedChi << std::endl;
    // fullprint << "Covariance=" << covariance << std::endl;
    estimatedChi = factory.buildAsChi();
    fullprint << "Default chi=" << estimatedChi << std::endl;
    estimatedChi = factory.buildAsChi(distribution.getParameter());
    fullprint << "Chi from parameters=" << estimatedChi << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
