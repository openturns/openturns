//                                               -*- C++ -*-
/**
 *  @brief The test file of class AdaptiveDirectionalSampling for standard methods
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
//   Log::Show(Log::ALL);

  OStream fullprint(std::cout);

  const UnsignedInteger dim = 4;
  Description input(dim);
  input[0] = "E";
  input[1] = "F";
  input[2] = "L";
  input[3] = "I";

  /* We create a numerical math function */
  SymbolicFunction function(input, Description(1, "F*L^3/(3.*E*I)"));

  /* We create a normal distribution point of dimension 1 */
  Point mean(dim, 0.0);
  mean[0] = 50.0; // E
  mean[1] =  1.0; // F
  mean[2] = 10.0; // L
  mean[3] =  5.0; // I
  const Point sigma(dim, 1.0);
  IdentityMatrix R(dim);
  Normal myDistribution(mean, sigma, R);

  /* We create a 'usual' RandomVector from the Distribution */
  RandomVector vect(myDistribution);

  /* We create a composite random vector */
  CompositeRandomVector output(function, vect);

  /* We create an Event from this RandomVector */
  ThresholdEvent myEvent(output, Less(), -3.0);

  const UnsignedInteger n = 1000;

  {
    /* We create a monte carlo algorithm */
    MonteCarloExperiment experiment;
    ProbabilitySimulationAlgorithm myAlgo(myEvent, experiment);
    myAlgo.setMaximumOuterSampling(100 * n);
    myAlgo.setBlockSize(1);

    /* Perform the simulation */
    myAlgo.run();

    /* Stream out the result */
    fullprint << "result=" << myAlgo.getResult() << std::endl;
  }


  /* We create a Directional Sampling algorithm */
  AdaptiveDirectionalSampling myAlgo(myEvent);
//   myAlgo.setPartialStratification(true);
//   myAlgo.setMaximumStratificationDimension(2);
  myAlgo.setMaximumOuterSampling(n);
  myAlgo.setBlockSize(1);
  UnsignedInteger calls0 = function.getEvaluationCallsNumber();
  /* Perform the simulation */
  myAlgo.run();

  UnsignedInteger calls = function.getEvaluationCallsNumber() - calls0;

  /* Stream out the result */
  fullprint << "result=" << myAlgo.getResult() << std::endl;
  fullprint << "callsNumber=" << calls << std::endl;


  return 0;
}

