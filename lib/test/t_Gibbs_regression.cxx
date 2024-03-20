//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomWalkMetropolisHastings
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
typedef JointDistribution::DistributionCollection                DistributionCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    // observations
    UnsignedInteger chainDim = 3;
    UnsignedInteger obsDim = 1;
    //UnsignedInteger outputDim = 1;

    UnsignedInteger obsSize = 10;

    Sample y_obs(obsSize, obsDim);
    y_obs[0][0] = -9.50794871493506;
    y_obs[1][0] = -3.83296694500105;
    y_obs[2][0] = -2.44545713047953;
    y_obs[3][0] = 0.0803625289211318;
    y_obs[4][0] = 1.01898069723583;
    y_obs[5][0] = 0.661725805623086;
    y_obs[6][0] = -1.57581204592385;
    y_obs[7][0] = -2.95308465670895;
    y_obs[8][0] = -8.8878164296758;
    y_obs[9][0] = -13.0812290405651;
    std::cout << "y_obs=" << y_obs << std::endl;

    Sample p(obsSize, chainDim);
    for (UnsignedInteger i = 0; i < obsSize; ++ i)
    {
      for (UnsignedInteger j = 0; j < chainDim; ++ j)
      {
        p[i][j] = pow(-2 + 5.*i / 9., 1.0 * j);
      }
    }
    std::cout << "p=" << p << std::endl;

    const Description fullVariables = {"p1", "p2", "p3", "x1", "x2", "x3"};
    const Description formulas = {"p1*x1+p2*x2+p3*x3", "1.0"};
    SymbolicFunction fullModel(fullVariables, formulas);
    Indices parametersPosition(chainDim);
    parametersPosition.fill();
    Point parametersValue(parametersPosition.getSize(), 0.0);
    ParametricFunction model(fullModel, parametersPosition, parametersValue);

    // instrumental distribution
    Uniform instrumental(-1.0, 1.0);

    // prior distribution
    Point sigma0(chainDim, 10.0);// sigma0= (10.,10.,10.)
    CorrelationMatrix Q0(chainDim);// precision matrix
    CorrelationMatrix Q0_inv(chainDim);// covariance matrix
    for ( UnsignedInteger i = 0; i < chainDim; ++ i )
    {
      Q0_inv (i, i) = sigma0[i] * sigma0[i];
      Q0(i, i) = 1.0 / Q0_inv (i, i);
    }
    std::cout << "Q0=" << Q0 << std::endl;
    Point mu0(chainDim, 0.0);// mu0 = (0.0, 0.0, 0.0)
    Distribution prior(Normal( mu0, Q0_inv ) );// x0 ~ N(mu0, sigma0)
    std::cout << "x~" << prior << std::endl;

    // start from te mean x0=(0.0, 0.0, 0.0)
    std::cout << "x0=" << mu0 << std::endl;

    // conditional distribution y~N(z, 1.0)
    Distribution conditional = Normal();
    std::cout << "y~" << conditional << std::endl;

    // create a metropolis-hastings sampler
    // prior=a distribution of dimension chainDim, the a priori distribution of the parameter
    // conditional=a distribution of dimension 1, the observation error on the output
    // model=the link between the parameter and the output
    // y_obs=noisy observations of the output
    // mu0=starting point of the chain
    Gibbs::MetropolisHastingsCollection coll;
    for (UnsignedInteger j = 0; j < chainDim; ++ j)
    {
      RandomWalkMetropolisHastings mh(prior, mu0, instrumental, Indices(1, j));
      mh.setLikelihood(conditional, y_obs, model, p);
      coll.add(mh);
    }
    Gibbs sampler(coll);

    // get a realization
    Point realization(sampler.getRealization());
    std::cout << "y1=" << realization << std::endl;

    // try to generate a sample
    UnsignedInteger sampleSize = 5000;
    Sample sample(sampler.getSample(sampleSize));
    Indices indices(2000);
    indices.fill(3000, 1);

    Point x_mu(sample.select(indices).computeMean());
    Point x_sigma(sample.select(indices).computeStandardDeviation());

    // compute covariance
    CovarianceMatrix x_cov(sample.computeCovariance());
    Matrix P(obsSize, chainDim);
    for (UnsignedInteger i = 0; i < obsSize; ++ i)
    {
      for (UnsignedInteger j = 0; j < chainDim; ++ j)
      {
        P(i, j) = p[i][j];
      }
    }
    Matrix Qn(P.transpose()*P + Q0);
    SquareMatrix Qn_inv(chainDim);
    for ( UnsignedInteger j = 0; j < chainDim; ++j )
    {
      Point I_j(chainDim);
      I_j[j] = 1.0;
      Point Qn_inv_j(Qn.solveLinearSystem(I_j));
      for ( UnsignedInteger i = 0; i < chainDim; ++i )
      {
        Qn_inv(i, j) = Qn_inv_j[i];
      }
    }
    Point sigma_exp(chainDim);
    for ( UnsignedInteger i = 0; i < chainDim; ++i )
    {
      sigma_exp[i] = sqrt(Qn_inv(i, i));
    }
    Point y_vec(obsSize);
    for ( UnsignedInteger i = 0; i < obsSize; ++i )
    {
      y_vec[i] = y_obs[i][0];
    }
    Point x_emp(Qn.solveLinearSystem(P.transpose()*y_vec));
    Point mu_exp(Qn.solveLinearSystem(P.transpose()*P * x_emp + Q0 * mu0));

    std::cout << "sample mean=" << x_mu << std::endl;
    std::cout << "expected mean=" << mu_exp << std::endl;
    assert_almost_equal(x_mu, mu_exp, 1e-1, 0.0);

    std::cout << "covariance=" << x_cov << std::endl;
    std::cout << "expected covariance=" << Qn_inv << std::endl;
    assert_almost_equal(x_cov, Qn_inv, 1e-1, 0.0);

    // check log-pdf is recomputed by the correct blocks
    const Point initialState({0.5, 0.5, 0.5, 0.5});
    RandomVectorMetropolisHastings rvmh1(RandomVector(Dirac({0.5, 0.5})), initialState, Indices({0, 1}));
    RandomVectorMetropolisHastings rvmh2(RandomVector(Uniform(0.0, 1.0)), initialState, Indices({2}));
    RandomWalkMetropolisHastings rwmh(SymbolicFunction({"x", "y", "z", "t"}, {"1"}), Interval(4), initialState, Uniform(), Indices({3}));
    // De-activate adaptation to retain step size
    rwmh.setBurnIn(0);
    Gibbs::MetropolisHastingsCollection coll2;
    coll2.add(rvmh1);
    coll2.add(rvmh2);
    coll2.add(rwmh);
    Gibbs gibbs(coll2);
    gibbs.getRealization();
    Indices recompute(gibbs.getRecomputeLogPosterior());
    assert_almost_equal(recompute[0], 1);
    assert_almost_equal(recompute[1], 0);
    assert_almost_equal(recompute[2], 1);
    gibbs.setUpdatingMethod(Gibbs::UpdatingMethod::RANDOM_UPDATING);
    gibbs.getRealization();
    recompute = gibbs.getRecomputeLogPosterior();
    assert_almost_equal(recompute[0], 1);
    assert_almost_equal(recompute[1], 1);
    assert_almost_equal(recompute[2], 1);

    // Check all blocks are called equally often under the random order option.
    // Here there are 3 blocks:
    // 1) a Dirac RandomVectorMetropolisHastings -- never moves
    // 2) a Uniform RandomVectorMetropolisHastings -- always moves
    // 3) a RandomWalkMetropolisHastings with average acceptance probability 1/2
    // If 1) is selected or 3) is selected and the proposal is rejected, the chain does not move
    // This happens with probability 1/3 + 1/3 * 1/2 = 1/2.
    sample = gibbs.getSample(10000);
    UnsignedInteger count_nomove = 0;
    for ( UnsignedInteger j = 1; j < sample.getSize(); ++j )
    {
      if (sample[j] == sample[j - 1]) count_nomove++;
    }
    const Scalar frequency_nomove = (Scalar)count_nomove / (Scalar)sample.getSize();
    assert_almost_equal(frequency_nomove, 0.5, 0.02, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
