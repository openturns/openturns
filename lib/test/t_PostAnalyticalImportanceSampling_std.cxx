//                                               -*- C++ -*-
/**
 *  @brief The test file of PostAnalyticalImportanceSampling class
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
  setRandomGenerator();

  try
  {

    /* We create a numerical math function */
    Description input(4);
    input[0] = "E";
    input[1] = "F";
    input[2] = "L";
    input[3] = "I";
    SymbolicFunction myFunction(input, Description(1, "-F*L^3/(3*E*I)"));

    UnsignedInteger dim = myFunction.getInputDimension();
    /* We create a normal distribution point of dimension 1 */
    Point mean(dim, 0.0);
    mean[0] = 50.0; // E
    mean[1] =  1.0; // F
    mean[2] = 10.0; // L
    mean[3] =  5.0; // I
    Point sigma(dim, 1.0);
    IdentityMatrix R(dim);
    Normal myDistribution(mean, sigma, R);

    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect(myDistribution);

    /* We create a composite random vector */
    CompositeRandomVector output(myFunction, vect);

    /* We create an Event from this RandomVector */
    ThresholdEvent myEvent(output, Less(), -3.0);

    /* We create a FORM algorithm */
    /* The first parameter is an OptimizationAlgorithm */
    /* The second parameter is an event */
    /* The third parameter is a starting point for the design point research */
    Cobyla myCobyla;
    myCobyla.setMaximumEvaluationNumber(400);
    FORM myAlgo(myCobyla, myEvent, mean);

    /* Perform the simulation */
    myAlgo.run();

    /* Create a PostAnalyticalImportanceSampling algorithm based on the previous FORM result */
    FORMResult formResult(myAlgo.getResult());
    PostAnalyticalImportanceSampling mySamplingAlgo(formResult);
    fullprint << "FORM probability=" << formResult.getEventProbability() << std::endl;
    mySamplingAlgo.setMaximumOuterSampling(250);
    mySamplingAlgo.setBlockSize(4);
    mySamplingAlgo.setMaximumCoefficientOfVariation(0.001);

    fullprint << "PostAnalyticalImportanceSampling=" << mySamplingAlgo << std::endl;

    mySamplingAlgo.run();

    /* Stream out the result */
    fullprint << "PostAnalyticalImportanceSampling result=" << mySamplingAlgo.getResult() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
