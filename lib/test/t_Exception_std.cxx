//                                               -*- C++ -*-
/**
 *  @brief The test file of class Exception for standard methods
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

template <class EXC>
ExitCodeValue check()
{
  ExitCodeValue rc = ExitCode::Error;

  try
  {
    throw EXC(HERE) << "This is the reason of the exception";
    std::cerr << "--- Exception not thrown !" << std::endl;
  }
  catch (const EXC & ex)
  {
    rc = ExitCode::Success;
    std::cerr << "+++ Correct exception catched." << std::endl;
  }
  catch (const Exception & ex)
  {
    rc = ExitCode::Error;
    std::cerr << "--- General exception catched !" << std::endl;
  }
  catch (const std::exception & ex)
  {
    rc = ExitCode::Error;
    std::cerr << "--- Standard exception catched !" << std::endl;
  }
  catch (...)
  {
    rc = ExitCode::Error;
    std::cerr << "--- Unknown exception catched !" << std::endl;
  };

  return rc;
}

template <>
ExitCodeValue check< Exception >()
{
  ExitCodeValue rc = ExitCode::Error;

  try
  {
    throw Exception(HERE) << "This is the reason of the exception";
    std::cerr << "--- Exception not thrown !" << std::endl;
  }
  catch (const Exception & ex)
  {
    rc = ExitCode::Success;
    std::cerr << "+++ Correct exception catched." << std::endl;
  }
  catch (const std::exception & ex)
  {
    rc = ExitCode::Error;
    std::cerr << "--- Standard exception catched !" << std::endl;
  }
  catch (...)
  {
    rc = ExitCode::Error;
    std::cerr << "--- Unknown exception catched !" << std::endl;
  };

  return rc;
}


int main(int, char *[])
{
  Log::Show( Log::ALL );

  TESTPREAMBLE;
  OStream fullprint(std::cout);

  ExitCodeValue rc = ExitCode::Success;

  rc |= check< Exception >();
  rc |= check< FileNotFoundException >();
  rc |= check< InternalException >();
  rc |= check< InvalidArgumentException >();
  rc |= check< InvalidDimensionException >();
  rc |= check< NotYetImplementedException >();
  rc |= check< OutOfBoundException >();
  rc |= check< XMLException >();
  rc |= check< XMLParserException >();
  rc |= check< DynamicLibraryException >();
  rc |= check< NotSymmetricDefinitePositiveException >();
  rc |= check< InvalidRangeException >();
  rc |= check< NotDefinedException >();
  rc |= check< FileOpenException >();
  rc |= check< StudyFileParsingException >();
  rc |= check< ObjectNotInStudyException >();
  rc |= check< ConfigurationFileParsingException >();

  return rc;
}
