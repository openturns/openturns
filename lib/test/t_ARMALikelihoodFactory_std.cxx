//                                               -*- C++ -*-
/**
 *  @brief The test file of class ARMALikelihoodFactory
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DistFunc.hxx"

using namespace OT;
using namespace OT::Test;


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // ARMA(p, q)
    const UnsignedInteger p = 2;
    const UnsignedInteger q = 1;
    const UnsignedInteger dimension = 2;

    // Make a realization of an ARMA model
    // Tmin , Tmax and N points for TimeGrid
    const Scalar dt = 1.0;
    const UnsignedInteger size = 400;
    RegularGrid timeGrid(0.0, dt, size);

    // Fixing the distributions for the WhiteNoise
    const Scalar sigma = 0.1;
    CovarianceMatrix cov(dimension);
    cov(0, 0) = sigma;
    cov(1, 1) = 2.0 * sigma;
    Normal whiteNoiseDistribution(Point(dimension), cov);

    // Building a process from a White Noise
    WhiteNoise whiteNoise(whiteNoiseDistribution);
    whiteNoise.setTimeGrid(timeGrid);


    Collection <SquareMatrix> arCoefficients(p);
    Collection <SquareMatrix> maCoefficients(q);

    SquareMatrix alpha(dimension);
    alpha(0, 0) = -0.5;
    alpha(0, 1) = -0.1;
    alpha(1, 0) = -0.4;
    alpha(1, 1) = -0.5;


    arCoefficients[0] = alpha;

    alpha(0, 0) = 0.0;
    alpha(0, 1) = 0.0;
    alpha(1, 0) = -0.25;
    alpha(1, 1) = 0.0;

    arCoefficients[1] = alpha;

    alpha(0, 0) = -0.4;
    alpha(0, 1) = 0.0;
    alpha(1, 0) = 0.0;
    alpha(1, 1) = -0.4;

    maCoefficients[0] = alpha;

    ARMACoefficients phi(arCoefficients);
    ARMACoefficients theta(maCoefficients);

    // ARMA model creation
    ARMA myARMA(phi, theta, whiteNoise);
    fullprint << "myARMA process = " << myARMA << std::endl;

    // Create a realization
    TimeSeries timeSeries(myARMA.getRealization());

    cov(0, 0) += 0.01 * DistFunc::rNormal();
    cov(1, 1) += 0.01 * DistFunc::rNormal();
    for (UnsignedInteger k = 0; k < p; ++k)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        for (UnsignedInteger i = 0; i < dimension; ++i)
          alpha(i, j) = 0.01 * DistFunc::rNormal() ;
      }
      phi[k] = phi[k] + alpha;
    }
    //
    for (UnsignedInteger k = 0; k < q; ++k)
    {
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        for (UnsignedInteger i = 0; i < dimension; ++i)
          alpha(i, j) = 0.01 * DistFunc::rNormal() ;
      }
      theta[k] = theta[k] + alpha;
    }

    ARMALikelihoodFactory factory(p, q, dimension);
    fullprint << "factory=" << factory << std::endl;
    factory.setInitialConditions(phi, theta, cov);

    Process result(factory.build(timeSeries));
    //fullprint << "Estimated ARMA=" << result << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
