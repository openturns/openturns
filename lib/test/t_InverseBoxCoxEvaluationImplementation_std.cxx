//                                               -*- C++ -*-
/**
 * @brief The test file of class InverseBoxCoxEvaluationImplementation for standard methods
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

  try
  {

    const UnsignedInteger dimension = 3;

    // Lambda
    NumericalPoint lambda(dimension);
    lambda[0] = 0.;
    lambda[1] = 0.5;
    lambda[2] = 1;

    InverseBoxCoxEvaluationImplementation myFunction(lambda);
    myFunction.setName("inverseBoxCoxFunction");

    // In this test, we check that for a NumericalPoint with same values, the function is done by component
    NumericalPoint inPoint(dimension, 2.0);
    // result of the function
    NumericalPoint outPoint = myFunction( inPoint );
    fullprint << "myFunction=" << myFunction << std::endl;
    fullprint << myFunction.getName() << "( " << inPoint << " ) = " << outPoint << std::endl;

    // Creation of a NumericalSample
    const UnsignedInteger size = 10;
    NumericalSample inSample(size, dimension);
    for (UnsignedInteger index = 0 ; index < size; ++index)
    {
      inSample[index] = NumericalPoint(dimension, 1.0 / (index + 1));
    }

    // result of the function
    NumericalSample outSample = myFunction( inSample );
    fullprint << myFunction.getName() << "( " << inSample << " ) = " << outSample << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
