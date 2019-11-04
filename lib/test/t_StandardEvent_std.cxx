//                                               -*- C++ -*-
/**
 *  @brief The test file of StandardEvent class
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
    /* We create a normal distribution point of dimension dim */
    Point mean(dim, 0.0);
    Point sigma(dim, 1.0);
    Normal myDistribution(mean, sigma, IdentityMatrix(dim));

    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect(myDistribution);

    /* We create a composite random vector */
    CompositeRandomVector output(myFunction, vect);

    /* We create a StandardEvent from this RandomVector */
    StandardEvent myStandardEvent(output, Less(), 1.0);
    fullprint << "myStandardEvent=" << myStandardEvent << std::endl;
    /* We compute one realization of the event */
    fullprint << "myStandardEvent realization (as a RandomVector)=" << myStandardEvent.RandomVector::getRealization() << std::endl;
    fullprint << "myStandardEvent antecedent realization (as a RandomVector)=" << myStandardEvent.RandomVector::getImplementation()->getAntecedent().getRealization() << std::endl;
    fullprint << "myStandardEvent realization=" << myStandardEvent.getRealization() << std::endl;
    fullprint << "myStandardEvent antecedent realization=" << myStandardEvent.getImplementation()->getAntecedent().getRealization() << std::endl;
    fullprint << "myStandardEvent antecedent distribution cdf=" << myStandardEvent.getImplementation()->getAntecedent().getDistribution().computeCDF(Point(dim, 1.0)) << std::endl;
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
    CompositeRandomVector output2(myFunction, vect2);

    /* We create an Event from this RandomVector */
    ThresholdEvent myEvent(output2, Less(), 1.0);

    /* Create a StandardEvent based on this Event */
    StandardEvent stdEvent(myEvent);

    /* Check if the StandardEvent is really a StandardEvent */
    /* Get a sample from the second antecedent of the standard event */
    UnsignedInteger size = 2000;
    Sample sample(stdEvent.getImplementation()->getAntecedent().getSample(size));
    /* Check if the sample mean is nearly the null vector */
    fullprint << "sample mean=" << sample.computeMean() << std::endl;
    /* Check if the sample covariance is nearly the identity matrix */
    fullprint << "sample covariance=" << sample.computeCovariance() << std::endl;
    /* Check if the failure probabilities are the same */
    fullprint << "Failure probability (Event)=" << myEvent.getSample(size).computeMean() << std::endl;
    fullprint << "Failure probability (StandardEvent)=" << stdEvent.getSample(size).computeMean() << std::endl;

    Sample x(3, dim);
    Point point(dim);
    point[0] = 1.0;
    point[1] = 0.5;
    point[2] = 1.0;
    point[3] = 0.5;
    x[0] = point;
    point[0] = 2.0;
    point[1] = 1.0;
    point[2] = 2.0;
    point[3] = 1.0;
    x[1] = point;
    point[0] = 3.0;
    point[1] = 1.5;
    point[2] = 3.0;
    point[3] = 1.5;
    x[2] = point;
    Point p(3);
    p[0] = 0.0;
    p[1] = 0.3;
    p[2] = 0.325;
    UserDefined myDistribution3(x, p);
    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect3(myDistribution3);

    /* We create a composite random vector */
    CompositeRandomVector output3(myFunction, vect3);

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
