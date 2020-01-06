//                                               -*- C++ -*-
/**
 *  @brief The test file of class TruncatedNormal for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    Point mu(4, 0.0);
    Point sigma(4, 1.0);
    Point a(4);
    Point b(4);
    a[0] = -4.0;
    b[0] = 4.0;
    a[1] = -1.0;
    b[1] = 4.0;
    a[2] = 1.0;
    b[2] = 2.0;
    a[3] = 3.0;
    b[3] = 6.0;
    for (UnsignedInteger i = 0; i < 4; i++)
    {
      PlatformInfo::SetNumericalPrecision(i == 2 ? 1 : 2);
      TruncatedNormal distribution(mu[i], sigma[i], a[i], b[i]);
      UnsignedInteger size = 10000;
      Sample sample(distribution.getSample(size));
      TruncatedNormalFactory factory;
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
      TruncatedNormal estimatedTruncatedNormal(factory.buildAsTruncatedNormal(sample));
      fullprint << "TruncatedNormal          =" << distribution << std::endl;
      fullprint << "Estimated TruncatedNormal=" << estimatedTruncatedNormal << std::endl;
      estimatedTruncatedNormal = factory.buildAsTruncatedNormal();
      fullprint << "Default TruncatedNormal=" << estimatedTruncatedNormal << std::endl;
      estimatedTruncatedNormal = factory.buildAsTruncatedNormal(distribution.getParameter());
      fullprint << "TruncatedNormal from parameters=" << estimatedTruncatedNormal << std::endl;
    }
    // Test for constant sample
    TruncatedNormalFactory factory;
    UnsignedInteger size = 10000;
    Sample sample(size, Point(1, 0.0));
    // Test for constant sample
    sample = Sample(size, Point(1, 0.0));
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    sample = Sample(size, Point(1, 1.0));
    estimatedDistribution = factory.build(sample);
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
