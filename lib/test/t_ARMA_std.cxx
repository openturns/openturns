//                                               -*- C++ -*-
/**
 *  @brief The test file of class ARMA
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /*2 D case */
    RandomGenerator::SetSeed(0);

    /* Matrices of the process */
    const UnsignedInteger dim = 2;
    SquareMatrix squareMatrix1(dim);
    squareMatrix1(0, 0) = 0.2 ;
    squareMatrix1(1, 0) = 0.3 ;
    squareMatrix1(0, 1) = 0.7 ;
    squareMatrix1(1, 1) = 0.4 ;


    /* Second matrix to add to the ARMACoefficients*/
    SquareMatrix squareMatrix2(dim) ;
    squareMatrix2(0, 0) = 0.1;
    squareMatrix2(1, 0) = 0.0;
    squareMatrix2(0, 1) = 0.0;
    squareMatrix2(1, 1) = 0.5;

    /* ARMA(p, q) */
    const UnsignedInteger p = 1;
    const UnsignedInteger q = 1;

    /* ARMACoefficients with default constructor */

    /* AR coefficients */
    ARMACoefficients coefficientsP(p, dim);
    coefficientsP[0] = squareMatrix1 ;

    /* MA coefficients */
    ARMACoefficients coefficientsQ(q, dim);
    coefficientsQ[0] = squareMatrix2 ;

    fullprint << "coefficientsP = " << coefficientsP << std::endl;
    fullprint << "coefficientsQ = " << coefficientsQ << std::endl;


    /* Time grid creation and White Noise */
    Scalar Tmin = 0.0;
    Scalar deltaT = 0.1;
    UnsignedInteger steps = 11;

    /* Initialization of the time grid timeGrid1*/
    RegularGrid timeGrid(Tmin, deltaT, steps);

    /* Distributions for the  choice */
    Distribution dist1 = Normal(0.0, 0.01);
    Distribution dist2 = Normal(0.0, 0.02);

    // Create a collection of distribution
    ComposedDistribution::DistributionCollection aCollection;
    aCollection.add( dist1 );
    aCollection.add( dist2 );

    Distribution dist  = ComposedDistribution(aCollection);
    fullprint << "dist = " << dist << std::endl;

    WhiteNoise whiteNoise(dist);

    /*Setting the timeGrid*/
    whiteNoise.setTimeGrid(timeGrid);

    /* Last coefficients values */
    Sample xValues(p, dim);
    Sample epsilonValues(q, dim);

    for(UnsignedInteger j = 0 ; j < dim ; ++j)
    {
      // Fill the AR-part (the last p-coefficients X_{-1}, X{-2},..., X_{-p})
      for(UnsignedInteger i = 0 ; i < p ; ++i)
      {
        xValues[i][j] = RandomGenerator::Generate();
      }

      // Fill the MA-part (the last p-coefficients \epsilon_{-1}, \epsilon_{-2},..., \epsilon_{-p})
      for(UnsignedInteger i = 0 ; i < q ; ++i)
      {
        epsilonValues[i][j] = RandomGenerator::Generate();
      }
    }

    /** Print the initial state of the ARMA : coefficients*/
    fullprint << "Last values of the process = " << xValues << std::endl;
    fullprint << "Last innovations of the process = " << epsilonValues << std::endl;

    // ARMAState creation
    ARMAState state(xValues, epsilonValues);
    Process process1 = ARMA(coefficientsP, coefficientsQ, whiteNoise);
    fullprint << "process1 = " << process1 << std::endl;
    ARMA process2(coefficientsP, coefficientsQ, whiteNoise);
    fullprint << "process2 = " << process2 << std::endl;
    ARMA process3(coefficientsP, coefficientsQ, whiteNoise, state);
    fullprint << "process3 = " << process3 << std::endl;

    // Test realization
    fullprint << "One realization=" << process2.getRealization() << std::endl;

    // Some steps further
    UnsignedInteger stepNumber = 4;
    fullprint << "One future=" << process2.getFuture(stepNumber) << std::endl;
    UnsignedInteger size = 3;
    fullprint << "Some futures=" << process2.getFuture(stepNumber, size) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
