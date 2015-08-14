//                                               -*- C++ -*-
/**
 *  @brief The test file of class ConfidenceInterval
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

  /** TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER */
  fullprint << "test number zero : default constructor and string converter" << std::endl;

  /* Default constructor */
  NumericalScalar begin = 1.0, end = 36.92;
  ConfidenceInterval testInterval(begin, end);
  testInterval.setName("testInterval");

  /* String converter */
  fullprint << "testInterval = " << testInterval << std::endl;


  /** TEST NUMBER ONE : COPY CONSTRUCTOR AND STRING CONVERTER */
  fullprint << "test number one : copy constructor and string converter" << std::endl;

  /* Copy constructor */
  ConfidenceInterval testIntervalCopy(testInterval);

  /* String converter */
  fullprint << "testIntervalCopy = " << testIntervalCopy << std::endl;


  /** TEST NUMBER TWO : ELEMENTS ACCESSORS */
  fullprint << "test number two : elements accessors" << std::endl;

  NumericalScalar begin2 = 12.0, end2 = 92.3898754;
  ConfidenceInterval testIntervalCopy2(testInterval);
  testIntervalCopy2.setValues(begin2, end2);
  fullprint << "testIntervalCopy2 values = " << testIntervalCopy2.getFirst() << " ; " << testIntervalCopy2.getLast() << std::endl;

  /** TEST NUMBER THREE : EQUALITY TEST */
  fullprint << "testInterval == testIntervalCopy " << (testInterval == testIntervalCopy) << std::endl;
  fullprint << "testInterval == testIntervalCopy2 " << (testInterval == testIntervalCopy2) << std::endl;


  return ExitCode::Success;
}
