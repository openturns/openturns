//                                               -*- C++ -*-
/**
 *  @brief The test file of class Gamma for standard methods
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
    UnsignedInteger size = 10000;

    // factory.build from distribution sample
    Gamma distribution(0.2, 1.0, 1.0);
    Sample sample(distribution.getSample(size));
    GammaFactory factory;
    Distribution estimatedDistribution(factory.build(sample));
    assert_almost_equal(estimatedDistribution, distribution, 0.13, 0.0);

    // factory.build from distribution (with other parameters) sample
    distribution = Gamma(2.3, 1.0, 1.0);
    sample = distribution.getSample(size);
    estimatedDistribution = factory.build(sample);
    assert_almost_equal(estimatedDistribution, distribution, 0.07, 0.0);

    // factory.build default
    Gamma defaultDistribution(1.0, 1.0, 0.0);
    estimatedDistribution = factory.build();
    assert_almost_equal(estimatedDistribution, defaultDistribution, 0.0, 0.0);

    // factory.build from parameters
    estimatedDistribution = factory.build(distribution.getParameter());
    assert_almost_equal(estimatedDistribution, distribution, 0.0, 0.0);

    // factory.buildAs from distribution sample
    Gamma estimatedGamma(factory.buildAsGamma(sample));
    assert_almost_equal(estimatedGamma, distribution, 0.07, 0.0);

    // factory.buildAs default
    estimatedGamma = factory.buildAsGamma();
    assert_almost_equal(estimatedGamma, defaultDistribution, 0.0, 0.0);

    // factory.buildAs from distribution parameters
    estimatedGamma = factory.buildAsGamma(distribution.getParameter());
    assert_almost_equal(estimatedGamma, distribution, 0.0, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
