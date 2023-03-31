//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomWalkMetropolisHastings
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
#include <iostream>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;
typedef ComposedDistribution::DistributionCollection DistributionCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    Scalar mu = 5000.0;
    Normal prior(mu, 1.0);
    Point initialState(1);
    Normal instrumental(0.0, 1.0);
    RandomWalkMetropolisHastings sampler(prior, initialState, instrumental);
    sampler.setBurnIn(1000);
    Sample s1(sampler.getSample(2000));
    // create list of indices
    Indices postBurnIn(s1.getSize() - sampler.getBurnIn());
    postBurnIn.fill(sampler.getBurnIn());
    assert_almost_equal(s1.select(postBurnIn).computeMean()[0], mu, 1e-2, 0.0);
    Collection<Point> dataList(
    {
        {53.0, 1.0},
        {57.0, 1.0},
        {58.0, 1.0},
        {63.0, 1.0},
        {66.0, 0.0},
        {67.0, 0.0},
        {67.0, 0.0},
        {67.0, 0.0},
        {68.0, 0.0},
        {69.0, 0.0},
        {70.0, 0.0},
        {70.0, 0.0},
        {70.0, 1.0},
        {70.0, 1.0},
        {72.0, 0.0},
        {73.0, 0.0},
        {75.0, 0.0},
        {75.0, 1.0},
        {76.0, 0.0},
        {76.0, 0.0},
        {78.0, 0.0},
        {79.0, 0.0},
        {81.0, 0.0},
    }
    );
    Sample data(dataList);    
    
    SymbolicFunction fun(
        {"alpha", "beta", "x"}, {"exp(alpha + beta * x) / (1 + exp(alpha + beta * x))"}
    );
    ParametricFunction linkFunction(fun, {2}, {0.0});
    Normal instrumental2({0.0, 0.0}, {0.5, 0.05}, IdentityMatrix(2));

    ComposedDistribution target({Uniform(-100.0, 100.0), Uniform(-100.0, 100.0)});
    RandomWalkMetropolisHastings rwmh(target, {0.0, 0.0}, instrumental2);
    Bernoulli conditional;
    Sample observations(data.getMarginal(1));
    Sample covariates(data.getMarginal(0));
    rwmh.setLikelihood(conditional, observations, linkFunction, covariates);

    // try to generate a sample
    Sample sample(rwmh.getSample(10000));
    Indices postBurnIn2(sample.getSize() - rwmh.getBurnIn());
    postBurnIn2.fill(rwmh.getBurnIn());
    Point muPost(sample.select(postBurnIn2).computeMean());
    Point sigma(sample.computeStandardDeviation());
     
    //std::cout << "mu=" << muPost << ", sigma=" << sigma << std::endl;
    assert_almost_equal(muPost, {10.3854, -0.164881});
    assert_almost_equal(sigma, {3.51975, 0.0517796});
    
    //std::cout << "acceptance rate=" << rwmh.getAcceptanceRate() << std::endl;
    assert_almost_equal(rwmh.getAcceptanceRate(), 0.3345);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
