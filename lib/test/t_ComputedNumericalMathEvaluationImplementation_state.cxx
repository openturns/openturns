//                                               -*- C++ -*-
/**
 * @brief The test file of class ComputedNumericalMathEvaluationImplementation with two different internal states
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    try
    {
      // Test function operator ()
      // ComputedNumericalMathEvaluationImplementation myFunction("wrapper");
      ComputedNumericalMathEvaluationImplementation myFunctionImpl = ComputedNumericalMathEvaluationImplementation( "wrapper", WrapperFile::FindWrapperByName( "wrapper" ) );
      NumericalPoint inPoint1(2);
      inPoint1[0] = 10;
      inPoint1[1] = 10;
      NumericalPoint outPoint1 = myFunctionImpl( inPoint1 );
      outPoint1 = myFunctionImpl( inPoint1 );
      fullprint << myFunctionImpl.getName() << "( " << inPoint1 << " ) = " << outPoint1 << std::endl;




      // Now we copy the object
      const ComputedNumericalMathEvaluationImplementation & ref_myFunctionImpl = myFunctionImpl;
      ComputedNumericalMathEvaluationImplementation newFunctionImpl(ref_myFunctionImpl);
      NumericalPoint inPoint2(2);
      inPoint2[0] = 100;
      inPoint2[1] = 100;
      NumericalPoint outPoint2 = newFunctionImpl( inPoint2 );
      outPoint2 = newFunctionImpl( inPoint2 );
      outPoint2 = newFunctionImpl( inPoint2 );
      fullprint << newFunctionImpl.getName() << "( " << inPoint2 << " ) = " << outPoint2 << std::endl;
    }
    catch (NoWrapperFileFoundException & ex)
    {
      throw TestFailed(ex.__repr__());
    }


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
