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
typedef ComposedDistribution::DistributionCollection                DistributionCollection;
typedef RandomWalkMetropolisHastings::CalibrationStrategyCollection CalibrationStrategyCollection;

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

    Description fullVariables(0);
    fullVariables.add("p1");
    fullVariables.add("p2");
    fullVariables.add("p3");
    fullVariables.add("x1");
    fullVariables.add("x2");
    fullVariables.add("x3");
    Description formulas(0);
    formulas.add("p1*x1+p2*x2+p3*x3");
    formulas.add("1.0");
    SymbolicFunction fullModel(fullVariables, formulas);
    Indices parametersPosition(chainDim);
    parametersPosition.fill();
    Point parametersValue(parametersPosition.getSize(), 0.0);
    ParametricFunction model(fullModel, parametersPosition, parametersValue);

    // calibration parameters
    CalibrationStrategyCollection calibrationColl(chainDim);

    // proposal distribution
    DistributionCollection proposalColl;
    for (UnsignedInteger i = 0; i < chainDim; ++ i)
    {
      proposalColl.add(Uniform(-1.0, 1.0));
    }

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
    // proposalColl=
    RandomWalkMetropolisHastings sampler(prior, conditional, model, p, y_obs, mu0, proposalColl);
    sampler.setVerbose(true);
    sampler.setThinning(4);
    sampler.setBurnIn(2000);
    sampler.setCalibrationStrategyPerComponent(calibrationColl);

    // get a realization
    Point realization(sampler.getRealization());
    std::cout << "y1=" << realization << std::endl;

    // try to generate a sample
    UnsignedInteger sampleSize = 1000;
    Sample sample(sampler.getSample(sampleSize));

    Point x_mu(sample.computeMean());
    Point x_sigma(sample.computeStandardDeviationPerComponent());

    // print acceptance rate
    std::cout << "acceptance rate=" << sampler.getAcceptanceRate() << std::endl;

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

    std::cout << "covariance=" << x_cov << std::endl;
    std::cout << "expected covariance=" << Qn_inv << std::endl;


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
