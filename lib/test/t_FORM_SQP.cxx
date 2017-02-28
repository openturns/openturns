//                                               -*- C++ -*-
/**
 *  @brief The test file of FORM class
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

String printNumericalPoint(const NumericalPoint & point, const UnsignedInteger digits)
{
  OSS oss;
  oss << "[";
  NumericalScalar eps = pow(0.1, 1.0 * digits);
  for (UnsignedInteger i = 0; i < point.getDimension(); i++)
  {
    oss << std::fixed << std::setprecision(digits) << (i == 0 ? "" : ",") << Bulk<double>((std::abs(point[i]) < eps) ? std::abs(point[i]) : point[i]);
  }
  oss << "]";
  return oss;
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* We create a numerical math function */
    Description input(4);
    input[0] = "E";
    input[1] = "F";
    input[2] = "L";
    input[3] = "I";
    NumericalMathFunction myFunction(input, Description(1, "d"), Description(1, "-F*L^3/(3*E*I)"));

    UnsignedInteger dim = myFunction.getInputDimension();
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

    /* We create a NearestPoint algorithm */
    // Test function operator ()
    input[0] = "x1";
    input[1] = "x2";
    input[2] = "x3";
    input[3] = "x4";
    NumericalMathFunction levelFunction(input, Description(1, "d"), Description(1, "x1+2*x2-3*x3+4*x4"));
    NumericalPoint startingPoint(4, 1.0);
    SQP myAlgorithm(OptimizationProblem(levelFunction, 3.0));
    myAlgorithm.setStartingPoint(startingPoint);
    myAlgorithm.setMaximumIterationNumber(100);
    myAlgorithm.setMaximumAbsoluteError(1.0e-10);
    myAlgorithm.setMaximumRelativeError(1.0e-10);
    myAlgorithm.setMaximumResidualError(1.0e-10);
    myAlgorithm.setMaximumConstraintError(1.0e-10);

    /* We create a FORM algorithm */
    /* The first parameter is an OptimizationAlgorithm */
    /* The second parameter is an event */
    /* The third parameter is a starting point for the design point research */
    FORM myAlgo(myAlgorithm, myEvent, mean);

    fullprint << "FORM=" << myAlgo << std::endl;

    /* Perform the simulation */
    myAlgo.run();

    /* Stream out the result */
    FORMResult result(myAlgo.getResult());
    UnsignedInteger digits = 5;
    fullprint << "event probability=" << result.getEventProbability() << std::endl;
    fullprint << "generalized reliability index=" << std::setprecision(digits) << result.getGeneralisedReliabilityIndex() << std::endl;
    fullprint << "standard space design point=" << printNumericalPoint(result.getStandardSpaceDesignPoint(), digits) << std::endl;
    fullprint << "physical space design point=" << printNumericalPoint(result.getPhysicalSpaceDesignPoint(), digits) << std::endl;
    fullprint << "is standard point origin in failure space? " << (result.getIsStandardPointOriginInFailureSpace() ? "true" : "false") << std::endl;
    fullprint << "importance factors=" << printNumericalPoint(result.getImportanceFactors(), digits) << std::endl;
    fullprint << "importance factors (classical)=" << printNumericalPoint(result.getImportanceFactors(AnalyticalResult::CLASSICAL), digits) << std::endl;
    fullprint << "Hasofer reliability index=" << std::setprecision(digits) << result.getHasoferReliabilityIndex() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
