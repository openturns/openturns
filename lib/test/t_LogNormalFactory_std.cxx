//                                               -*- C++ -*-
/**
 *  @brief The test file of class LogNormal for standard methods
 *
 *  Copyright 2005-2027 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {
    const LogNormal distribution(1.5, 2.5, -1.5);
    const UnsignedInteger size = 10000;
    const Sample sample(distribution.getSample(size));
    LogNormalFactory factory;
    // Default estimator: local likelihood maximization
    Distribution estimatedDistribution(factory.build(sample));
    assert_almost_equal(estimatedDistribution.getParameter(), Point({1.4653145870837043, 2.503644452403846, -1.4997189373077306}));
    estimatedDistribution = factory.build(sample, 0);
    assert_almost_equal(estimatedDistribution.getParameter(), Point({1.4653145870837043, 2.503644452403846, -1.4997189373077306}));
    estimatedDistribution = factory.build(sample, 1);
    assert_almost_equal(estimatedDistribution.getParameter(), Point({2.4277587012992106, 2.0424293078874363, -1.5037171697567402}));
    estimatedDistribution = factory.build(sample, 2);
    assert_almost_equal(estimatedDistribution.getParameter(), Point({4.641674507180531, 1.4215894511254732, -195.15997062681078}));
    estimatedDistribution = factory.build();
    assert_almost_equal(estimatedDistribution.getParameter(), Point({0.0, 1.0, 0.0}));
    estimatedDistribution = factory.build(distribution.getParameter());
    assert_almost_equal(estimatedDistribution.getParameter(), Point({1.5, 2.5, -1.5}));
    const LogNormal estimatedLogNormal(factory.buildAsLogNormal(sample));
    assert_almost_equal(estimatedLogNormal.getParameter(), Point({1.4653145870837043, 2.503644452403846, -1.4997189373077306}));
    assert_almost_equal(factory.buildAsLogNormal().getParameter(), Point({0.0, 1.0, 0.0}));
    assert_almost_equal(factory.buildAsLogNormal(distribution.getParameter()).getParameter(), Point({1.5, 2.5, -1.5}));

    // Local likelihood with no interior stationary point: the profile
    // log-likelihood increases towards the sample minimum, so the local
    // likelihood estimator is undefined and the modified moments estimator
    // is used instead (a spurious far-left root is discarded)
    Sample degenerateSample(6, 1);
    degenerateSample[0][0] = 1.5;
    degenerateSample[1][0] = 3.0;
    degenerateSample[2][0] = 10.0;
    degenerateSample[3][0] = 15.0;
    degenerateSample[4][0] = 23.0;
    degenerateSample[5][0] = 59.0;
    const LogNormal estimatedDegenerate(factory.buildAsLogNormal(degenerateSample, 0));
    assert_almost_equal(estimatedDegenerate.getParameter(), Point({2.872298307618225, 0.7688215878291091, -5.172820948624551}));
    // The default estimator takes the same path
    assert_almost_equal(factory.buildAsLogNormal(degenerateSample).getParameter(), Point({2.872298307618225, 0.7688215878291091, -5.172820948624551}));
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
