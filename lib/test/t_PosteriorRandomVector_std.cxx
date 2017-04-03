//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomWalkMetropolisHastings
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
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

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

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
    Scalar mu0 = 25.;

    Scalar sigma0 = 0.1;
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
    sampler.setThinning(100);
    sampler.setBurnIn(1000);
    sampler.setCalibrationStrategyPerComponent(calibrationColl);

    // create a PosteriorRandomVector
    PosteriorRandomVector randomVector(sampler);
    std::cout << "randomVector=" << randomVector << std::endl;

    // get the dimension
    UnsignedInteger dimension = randomVector.getDimension();
    std::cout << "dimension=" << dimension << std::endl;

    // get a realization
    Point realization(randomVector.getRealization());
    std::cout << "realization=" << realization << std::endl;

    // get a sample
    Sample sample(randomVector.getSample(10));
    std::cout << "sample=" << sample << std::endl;


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
