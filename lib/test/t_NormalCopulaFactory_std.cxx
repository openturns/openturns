//                                               -*- C++ -*-
/**
 *  @brief The test file of class NormalCopula for standard methods
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

  try
  {
    UnsignedInteger dim = 3;
    CorrelationMatrix R(dim);
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      for (UnsignedInteger j = 0; j < i; j++)
      {
        R(i, j) = 0.5 * (1.0 + i) / dim;
      }
    }
    NormalCopula distribution(R);
    UnsignedInteger size = 10000;
    Sample sample(distribution.getSample(size));
    NormalCopulaFactory factory;
    CovarianceMatrix covariance;
    // Distribution estimatedDistribution(factory.build(sample, covariance));
    Distribution estimatedDistribution(factory.build(sample));
    fullprint << "Distribution          =" << distribution << std::endl;
    fullprint << "Estimated distribution=" << estimatedDistribution << std::endl;
    // fullprint << "Covariance=" << covariance << std::endl;
    estimatedDistribution = factory.build();
    fullprint << "Default distribution=" << estimatedDistribution << std::endl;
    NormalCopula estimatedNormalCopula(factory.buildAsNormalCopula(sample));
    fullprint << "NormalCopula          =" << distribution << std::endl;
    fullprint << "Estimated normalCopula=" << estimatedNormalCopula << std::endl;
    estimatedNormalCopula = factory.buildAsNormalCopula();
    fullprint << "Default normalCopula=" << estimatedNormalCopula << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
