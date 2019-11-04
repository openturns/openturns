//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomWalkMetropolisHastings
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
#include <iostream>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;
typedef ComposedDistribution::DistributionCollection DistributionCollection;
typedef RandomWalkMetropolisHastings::CalibrationStrategyCollection CalibrationStrategyCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // this analytical example is taken from "Bayesian Modeling Using WinBUGS" - Ioannis Ntzoufras
    // 1.5.3: Inference for the mean or normal data with known variance

    // Variable of interest: Y=N(mu, sigma)
    // Prior for mu: Normal(mu0, sigma0), sigma is known
    // Posterior for mu: E(mu|y)=w*y_mean+(1-w)*mu0, and Var(mu|y)=w*(sigmay^2)/n
    // => weighted average of the prior an the sample mean
    // with w = n*sigma0^2 / (n*sigma0^2 + sigma^2)


    // observations
    UnsignedInteger size = 10;
    Normal realDist(31.0, 1.2);

    Sample data(realDist.getSample(size));

    // calibration parameters
    CalibrationStrategyCollection calibrationColl(2);

    // proposal distribution
    DistributionCollection proposalColl;
    Uniform mean_proposal(-2.0, 2.0);
    Uniform std_proposal(-2.0, 2.0);
    proposalColl.add(mean_proposal);
    proposalColl.add(std_proposal);

    // prior distribution
    Scalar mu0 = 25.0;

    Point sigma0s;
    sigma0s.add(0.1);
    sigma0s.add(1.0);
    //sigma0s.add(2.0);


    // play with the variance of the prior:
    // if the prior variance is low (information concerning the mu parameter is strong)
    // then the posterior mean will be equal to the prior mean
    // if large, the the posterior distribution is equivalent to the distribution of the sample mean
    for ( UnsignedInteger i = 0; i < sigma0s.getDimension(); ++ i )
    {

      Scalar sigma0 = sigma0s[i];
      Normal mean_prior(mu0, sigma0);
      Dirac std_prior(2.0); // standard dev is known
      DistributionCollection priorColl;
      priorColl.add(mean_prior);
      priorColl.add(std_prior);
      Distribution prior = ComposedDistribution( priorColl );

      // choose the initial state within the prior
      Point initialState(prior.getRealization());

      // conditional distribution
      Distribution conditional = Normal();

      // create a metropolis-hastings sampler
      RandomWalkMetropolisHastings sampler(prior, conditional, data, initialState, proposalColl);
      sampler.setVerbose(true);
      sampler.setThinning(2);
      sampler.setBurnIn(500);
      sampler.setCalibrationStrategyPerComponent(calibrationColl);

      Scalar sigmay = ConditionalDistribution(Normal(), prior).getStandardDeviation()[0];
      Scalar w = size * pow(sigma0, 2.) / (size * pow(sigma0, 2.) + pow(sigmay, 2.0));

      std::cout << "prior variance=" << pow(sigma0, 2.) << std::endl;

      Point realization(sampler.getRealization());
      std::cout << "  realization=" << realization << std::endl;

      std::cout << "  w=" << w << std::endl;

      // the posterior for mu is analytical
      std::cout << "  expected posterior ~N(" << w*data.computeMean()[0] + (1. - w)*mu0 << ", " << sqrt(w * pow(sigmay, 2.0) / size) << ")" << std::endl;

      // try to generate a sample
      Sample sample(sampler.getSample(50));

      std::cout << "  obtained posterior ~N(" << sample.computeMean()[0] << ", " << sample.computeStandardDeviationPerComponent()[0] << ")" << std::endl;

      std::cout << "  acceptance rate=" << sampler.getAcceptanceRate() << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
