//                                               -*- C++ -*-
/**
 *  @brief The test file of StandardEvent class
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

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
    /* We create a normal distribution point of dimension dim */
    NumericalPoint mean(dim, 0.0);
    NumericalPoint sigma(dim, 1.0);
    Normal myDistribution(mean, sigma, IdentityMatrix(dim));

    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect(myDistribution);

    /* We create a composite random vector */
    RandomVector output(myFunction, vect);

    /* We create a StandardEvent from this RandomVector */
    StandardEvent myStandardEvent(output, Less(), 1.0);
    fullprint << "myStandardEvent=" << myStandardEvent << std::endl;
    /* We compute one realization of the event */
    fullprint << "myStandardEvent realization (as a RandomVector)=" << myStandardEvent.RandomVector::getRealization() << std::endl;
    fullprint << "myStandardEvent antecedent realization (as a RandomVector)=" << myStandardEvent.RandomVector::getImplementation()->getAntecedent()->getRealization() << std::endl;
    fullprint << "myStandardEvent realization=" << myStandardEvent.getRealization() << std::endl;
    fullprint << "myStandardEvent antecedent realization=" << myStandardEvent.getImplementation()->getAntecedent()->getRealization() << std::endl;
    fullprint << "myStandardEvent antecedent distribution cdf=" << myStandardEvent.getImplementation()->getAntecedent()->getDistribution().computeCDF(NumericalPoint(dim, 1.0)) << std::endl;
    /* We compute a sample of the event */
    fullprint << "myStandardEvent sample=" << myStandardEvent.getSample(10) << std::endl;

    // Build a standard event based on an event
    CorrelationMatrix R = IdentityMatrix(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.5;
    }
    Normal myDistribution2(mean, sigma, R);
    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect2(myDistribution2);

    /* We create a composite random vector */
    RandomVector output2(myFunction, vect2);

    /* We create an Event from this RandomVector */
    Event myEvent(output2, Less(), 1.0);

    /* Create a StandardEvent based on this Event */
    StandardEvent stdEvent(myEvent);

    /* Check if the StandardEvent is really a StandardEvent */
    /* Get a sample from the second antecedent of the standard event */
    UnsignedInteger size(2000);
    NumericalSample sample(stdEvent.getImplementation()->getAntecedent()->getSample(size));
    /* Check if the sample mean is nearly the null vector */
    fullprint << "sample mean=" << sample.computeMean() << std::endl;
    /* Check if the sample covariance is nearly the identity matrix */
    fullprint << "sample covariance=" << sample.computeCovariance() << std::endl;
    /* Check if the failure probabilities are the same */
    fullprint << "Failure probability (Event)=" << myEvent.getSample(size).computeMean() << std::endl;
    fullprint << "Failure probability (StandardEvent)=" << stdEvent.getSample(size).computeMean() << std::endl;

    Collection<UserDefinedPair> collection(3, UserDefinedPair(NumericalPoint(dim), 0.0));
    NumericalPoint point(dim);
    point[0] = 1.0;
    point[1] = 0.5;
    point[2] = 1.0;
    point[3] = 0.5;
    collection[0] = UserDefinedPair(point, 0.3);
    point[0] = 2.0;
    point[1] = 1.0;
    point[2] = 2.0;
    point[3] = 1.0;
    collection[1] = UserDefinedPair(point, 0.325);
    point[0] = 3.0;
    point[1] = 1.5;
    point[2] = 3.0;
    point[3] = 1.5;
    collection[2] = UserDefinedPair(point, 0.375);
    UserDefined myDistribution3(collection);
    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect3(myDistribution3);

    /* We create a composite random vector */
    RandomVector output3(myFunction, vect3);

    /* We try to create a StandardEvent from this RandomVector */
    try
    {
      StandardEvent myStandardEvent3(output3, Less(), 1.0);
    }
    catch(InvalidArgumentException &ex)
    {
      fullprint << "(This error is on purpose) Error trying to build myStandardEvent3" << std::endl;
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
