//                                               -*- C++ -*-
/**
 *  @brief The test file of MonteCarlo class
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

static Bool stop(void * p)
{
  std::cerr << "-- stop?" << std::endl;
  return false;
}

static void progress(NumericalScalar percent, void * p)
{
  std::cerr << "-- progress=" << percent << "%" << std::endl;
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    /* We create a numerical math function */
    Description input(4);
    input[0] = "E";
    input[1] = "F";
    input[2] = "L";
    input[3] = "I";
    NumericalMathFunction myFunction(input, Description(1, "d"), Description(1, "-F*L^3/(3*E*I)"));

    UnsignedInteger dim(myFunction.getInputDimension());
    /* We create a normal distribution point of dimension 1 */
    NumericalPoint mean(dim, 0.0);
    mean[0] = 50.0; // E
    mean[1] =  1.0; // F
    mean[2] = 10.0; // L
    mean[3] =  5.0; // I
    NumericalPoint sigma(dim, 1.0);
    IdentityMatrix R(dim);
    Normal myDistribution(mean, sigma, R);

    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect(myDistribution);

    /* We create a composite random vector */
    RandomVector output(myFunction, vect);

    /* We create an Event from this RandomVector */
    Event myEvent(output, Less(), -3.0);

    /* We create a Monte Carlo algorithm */
    MonteCarlo myAlgo(myEvent);
    myAlgo.setMaximumOuterSampling(250);
    myAlgo.setBlockSize(4);
    myAlgo.setMaximumCoefficientOfVariation(0.1);
    myAlgo.setProgressCallback(&progress);
    myAlgo.setStopCallback(&stop);

    fullprint << "MonteCarlo=" << myAlgo << std::endl;

    /* Perform the simulation */
    myAlgo.run();

    /* Stream out the result */
    fullprint << "MonteCarlo result=" << myAlgo.getResult() << std::endl;
    fullprint << "Confidence length at level 99%=" << myAlgo.getResult().getConfidenceLength(0.99) << std::endl;
    fullprint << "Confidence length at level 80%=" << myAlgo.getResult().getConfidenceLength(0.8) << std::endl;

    /* Use the standard deviation as a stoping rule */
    myAlgo = MonteCarlo(myEvent);
    myAlgo.setMaximumOuterSampling(250);
    myAlgo.setBlockSize(4);
    myAlgo.setMaximumCoefficientOfVariation(0.0);
    myAlgo.setMaximumStandardDeviation(0.1);

    fullprint << "MonteCarlo=" << myAlgo << std::endl;

    /* Perform the simulation */
    myAlgo.run();

    /* Stream out the result */
    fullprint << "MonteCarlo result=" << myAlgo.getResult() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
