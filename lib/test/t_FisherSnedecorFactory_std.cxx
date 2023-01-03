//                                               -*- C++ -*-
/**
 *  @brief The test file of class FisherSnedecor for standard methods
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
    PlatformInfo::SetNumericalPrecision(5);
    FisherSnedecor distribution(4.5, 8.4);
    UnsignedInteger size = 10000;
    Sample sample(distribution.getSample(size));
    FisherSnedecorFactory factory;
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
    FisherSnedecor estimatedFisherSnedecor(factory.buildAsFisherSnedecor(sample));
    fullprint << "FisherSnedecor          =" << distribution << std::endl;
    fullprint << "Estimated fisherSnedecor=" << estimatedFisherSnedecor << std::endl;
    estimatedFisherSnedecor = factory.buildAsFisherSnedecor();
    fullprint << "Default fisherSnedecor=" << estimatedFisherSnedecor << std::endl;
    estimatedFisherSnedecor = factory.buildAsFisherSnedecor(distribution.getParameter());
    fullprint << "FisherSnedecor from parameters=" << estimatedFisherSnedecor << std::endl;
    // buildMethodOfMoments
    estimatedFisherSnedecor = factory.buildMethodOfMoments(sample);
    fullprint << "Estimated from moments=" << estimatedFisherSnedecor << std::endl;
    const Scalar sample_mu = sample.computeMean()[0];
    const Scalar sample_sigma2 = sample.computeCovariance()(0, 0);
    const Scalar computed_mu = estimatedFisherSnedecor.getMean()[0];
    const Scalar computed_sigma2 = estimatedFisherSnedecor.getCovariance()(0, 0);
    assert_almost_equal(sample_mu, computed_mu, 1e-15, 1e-15);
    assert_almost_equal(sample_sigma2, computed_sigma2, 1e-15, 1e-15);
    // buildMethodOfLikelihoodMaximization
    estimatedFisherSnedecor = factory.buildMethodOfLikelihoodMaximization(sample);
    fullprint << "Estimated from likelihoodMaximization=" << estimatedFisherSnedecor << std::endl;
    const Scalar exact_d1 = distribution.getD1();
    const Scalar exact_d2 = distribution.getD2();
    const Scalar computed_d1 = estimatedFisherSnedecor.getD1();
    const Scalar computed_d2 = estimatedFisherSnedecor.getD2();
    assert_almost_equal(computed_d1, exact_d1, 0.0, 20.0 / sqrt(size));
    assert_almost_equal(computed_d2, exact_d2, 0.0, 20.0 / sqrt(size));
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
