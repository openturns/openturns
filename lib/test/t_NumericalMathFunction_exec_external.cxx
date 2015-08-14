//                                               -*- C++ -*-
/**
 *  @brief The test file of class NumericalMathFunction for standard methods
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
#include <iostream>
#include <sstream>
#include <exception>
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

#ifndef WIN32
  String wrapperName("poutre_external_LINUX");
#else
  String wrapperName("poutre_external_WIN32");
#endif



  try
  {
    String inFile1("poutre_external_infile1");
    String inFile2("poutre_external_infile2");
    {
      std::ofstream file(inFile1.c_str(), std::ios::out);
      file << "F=1111." << std::endl;
      file << "E=222.E9" << std::endl;
      file.close();
    }
    {
      std::ofstream file(inFile2.c_str(), std::ios::out);
      file << "L=1.1" << std::endl;
      file << "I=2.22E-6" << std::endl;
      file.close();
    }

    /** Instance creation */
    NumericalMathFunction deviation( wrapperName );

    NumericalPoint inPoint(4);
    inPoint[0] = 210.e9;
    inPoint[1] = 1000;
    inPoint[2] = 1.5;
    inPoint[3] = 2.e-6;

    NumericalPoint outPoint = deviation( inPoint );

    fullprint << "deviation = " << outPoint << std::endl;

    Os::Remove(inFile1);
    Os::Remove(inFile2);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
