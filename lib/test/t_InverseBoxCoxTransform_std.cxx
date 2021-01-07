//                                               -*- C++ -*-
/**
 *  @brief The test file of class InverseBoxCoxTransform for standard methods
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
    // Realization issued from a Normal distribution
    const UnsignedInteger dimension = 1;

    // Fix the realization as a Normal
    // Parameters are done such as the values are positive
    Uniform myDistribution(-1, 1);

    // get a realization
    const UnsignedInteger n = 51;
    const Scalar timeStart = 0.;
    const Scalar timeStep = 0.1;
    Sample mySample(myDistribution.getSample(n));
    RegularGrid myTimeGrid(timeStart, timeStep, n);

    /* Create a TimeSeries */
    const TimeSeries myRealization(myTimeGrid, mySample);

    // Create the lambda parameter
    Point lambda(dimension);
    for (UnsignedInteger index = 0 ; index < dimension; ++index)
    {
      lambda[index] = (index + 2) * 0.1;
    }

    InverseBoxCoxTransform myInverseBoxCox(lambda);

    fullprint << "myInverseBoxCox=" << myInverseBoxCox << std::endl;

    /* Get the input and output dimension */
    fullprint << "myInverseBoxCox input dimension=" << myInverseBoxCox.getInputDimension() << std::endl;
    fullprint << "myInverseBoxCox output dimension=" << myInverseBoxCox.getOutputDimension() << std::endl;

    fullprint << "input time series=" << myRealization << std::endl;
    fullprint << "output time series=" << myInverseBoxCox(myRealization) << std::endl;

    /* InverseBoxCoxTransform's inverse accessor */
    BoxCoxTransform myBoxCox(myInverseBoxCox.getInverse());
    fullprint << "myBoxCox=" << myBoxCox << std::endl;

    /* Get the number of calls */
    fullprint << "number of call(s) : " << myInverseBoxCox.getCallsNumber() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
