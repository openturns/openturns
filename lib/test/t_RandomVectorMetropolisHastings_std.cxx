//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomVectorMetropolisHastings
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
    // Test RandomVectorMetropolisHastings on Beta-Binomial conjugate model

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
    Sample X(model.getSample(1));
    Beta posterior(a + X(0, 0), b + n - X(0, 0), lower, upper);

    // Define IMH sampler
    IndependentMetropolisHastings rvmh_sampler(prior, {p}, Uniform(-1.0, 1.0), Indices(1));
    Description inVar({"x"});
    Description outVar({std::to_string(n), "x"});
    SymbolicFunction slf(inVar, outVar);
    rvmh_sampler.setLikelihood(model, X, slf);

    // Generate posterior distribution sample
    UnsignedInteger N = 10000;
    Sample Xsample(rvmh_sampler.getSample(N));

    // Compare empirical to theoretical moments
    assert_almost_equal(Xsample.computeMean(), posterior.getMean(), 0.0, 10.0 / std::sqrt(N));
    assert_almost_equal(Xsample.computeStandardDeviation(), posterior.getStandardDeviation(), 0.0, 10.0 / std::sqrt(N));

    // End Beta-Binomial conjugate model test

    // Elementary test
    Normal normal;
    const Normal & ref_normal(normal);
    RandomVector randomVector(ref_normal);
    RandomVectorMetropolisHastings sampler(randomVector, {0.0});
    Sample x(sampler.getSample(N));
    Point mean(x.computeMean());
    Point stddev(x.computeStandardDeviation());
    assert_almost_equal(mean, {0.0}, 0.0, 0.03);
    assert_almost_equal(stddev, {1.0}, 0.03, 0.0);

    // with link function

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
