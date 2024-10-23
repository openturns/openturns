//                                               -*- C++ -*-
/**
 *  @brief The test file of class IndependentMetropolisHastings
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
typedef JointDistribution::DistributionCollection DistributionCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Test IndependentMetropolisHastings on Beta-Binomial conjugate model

    // Define Beta-binomial model
    Scalar a = 1.0;
    Scalar b = 1.0;
    Scalar lower = 0.0;
    Scalar upper = 1.0;
    Beta prior(a, b, lower, upper);

    UnsignedInteger n = 10;
    Scalar p = 0.5;
    Binomial model(n, p);

    // Simulate data and compute analytical posterior
    // cf. Wikipedia table of conjugate distributions
    // https://en.wikipedia.org/wiki/Conjugate_prior#Table_of_conjugate_distributions
    Sample X(model.getSample(1));
    Beta posterior(a + X(0, 0), b + n - X(0, 0), lower, upper);

    // Define IMH sampler
    IndependentMetropolisHastings imh_sampler(prior, {p}, Uniform(-1.0, 1.0), Indices(1));
    Description inVar({"x"});
    Description outVar({std::to_string(n), "x"});
    SymbolicFunction slf(inVar, outVar);
    imh_sampler.setLikelihood(model, X, slf);

    // Generate posterior distribution sample
    UnsignedInteger N = 10000;
    Sample Xsample(imh_sampler.getSample(N));

    // Compare empirical to theoretical moments
    assert_almost_equal(Xsample.computeMean(), posterior.getMean(), 0.0, 10.0 / std::sqrt(N));
    assert_almost_equal(Xsample.computeStandardDeviation(), posterior.getStandardDeviation(), 0.0, 10.0 / std::sqrt(N));

    // End Beta-Binomial conjugate model test

    RandomGenerator::SetSeed(1);
    const Point lower_bound(1);
    const Point upper_bound({2.0 * M_PI});
    JointDistribution::DistributionCollection distributionCollection(2);
    distributionCollection[0] = Exponential(1.0);
    distributionCollection[1] = Normal(5.3, 0.4);
    const Point weights({0.9, 0.1});
    const Mixture instrumentalDistribution(distributionCollection, weights);

    // MetropolisHastings classes expect to receive the logarithm to the target density.
    const SymbolicFunction log_density ("x", "log(2 + sin(x)^2) - (2 + cos(3*x)^3 + sin(2*x)^3) * x");

    const Point initialState({3.0});  // not important in this case
    const Interval support(lower_bound, upper_bound);
    IndependentMetropolisHastings independentMH(log_density, support, initialState, instrumentalDistribution, {0});

    // Get a sample
    const Scalar sampleSize = 1000000;
    const Sample sample(independentMH.getSample(sampleSize));


    // Compute posterior mean

    const Scalar mean_ref = 1.22498;
    const Scalar posterior_mean = sample.computeMean()[0];

    assert_almost_equal(posterior_mean, mean_ref, 1e-5, 0.0);

    const Scalar std_ref = 1.61558;
    const Scalar posterior_std = sample.computeStandardDeviation()[0];

    assert_almost_equal(posterior_std, std_ref, 1e-5, 0.0);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
