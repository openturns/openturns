//                                               -*- C++ -*-
/**
 *  @brief The test file of class TruncatedNormal for standard methods
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
    // buildMethodOfMoments
    fullprint << "buildMethodOfMoments" << std::endl;
    TruncatedNormal distribution(2.0, 3.0, -1.0, 4.0);
    Sample sample2(distribution.getSample(size));
    TruncatedNormal estimatedTN(factory.buildMethodOfMoments(sample2));
    fullprint << "Estimated from moments=" << estimatedTN << std::endl;
    const Scalar sample_mu = sample2.computeMean()[0];
    const Scalar sample_sigma2 = sample2.computeCovariance()(0, 0);
    const Scalar sample_a = sample2.getMin()[0];
    const Scalar sample_b = sample2.getMax()[0];
    Scalar computed_mu = estimatedTN.getMean()[0];
    Scalar computed_sigma2 = estimatedTN.getCovariance()(0, 0);
    Scalar computed_a = estimatedTN.getA();
    Scalar computed_b = estimatedTN.getB();
    assert_almost_equal(sample_mu, computed_mu, 1.e-2, 0.0);
    assert_almost_equal(sample_sigma2, computed_sigma2, 1.e-3, 0.0);
    assert_almost_equal(sample_a, computed_a, 0.0, 10.0 / size);
    assert_almost_equal(sample_b, computed_b, 0.0, 10.0 / size);
    // buildMethodOfLikelihoodMaximization
    fullprint << "buildMethodOfLikelihoodMaximization" << std::endl;
    TruncatedNormal distribution3(2.0, 3.0, -1.0, 4.0);
    Sample sample3(distribution3.getSample(size));
    TruncatedNormal estimatedTN3(factory.buildMethodOfLikelihoodMaximization(sample3));
    fullprint << "Estimated from likelihoodMaximization=" << estimatedTN3 << std::endl;
    const Scalar exact_mu = distribution3.getMu();
    const Scalar exact_sigma = distribution3.getSigma();
    const Scalar exact_a = distribution3.getA();
    const Scalar exact_b = distribution3.getB();
    computed_mu = estimatedTN3.getMu();
    const Scalar computed_sigma = estimatedTN3.getSigma();
    computed_a = estimatedTN3.getA();
    computed_b = estimatedTN3.getB();
    assert_almost_equal(computed_mu, exact_mu, 0.0, 20.0 / sqrt(size));
    assert_almost_equal(computed_sigma, exact_sigma, 0.0, 20.0 / sqrt(size));
    assert_almost_equal(computed_a, exact_a, 0.0, 20.0 / size);
    assert_almost_equal(computed_b, exact_b, 0.0, 20.0 / size);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
