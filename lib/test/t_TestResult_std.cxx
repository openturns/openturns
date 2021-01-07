//                                               -*- C++ -*-
/**
 *  @brief The test file of class TestResult
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

  try
  {
    /** TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test number zero : default constructor and string converter" << std::endl;

    /* Default constructor */
    String type = "testResult";
    Bool testBool = true;
    Scalar testVal = 0.372;
    Scalar testThres = 0.42;
    Scalar statistic = 0.59;
    TestResult testResultTest(type, testBool, testVal, testThres, statistic);

    /* String converter */
    fullprint << "testResultTest = " << testResultTest << std::endl;


    /** TEST NUMBER ONE : COPY CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test number one : copy constructor and string converter" << std::endl;

    /* Copy constructor */
    TestResult testResultTest2(testResultTest);

    /* String converter */
    fullprint << "testResultTest2 = " << testResultTest2 << std::endl;


    /** TEST NUMBER TWO : ELEMENTS ACCESSORS */
    fullprint << "test number two : elements accessors" << std::endl;
    fullprint << "testResultTest2 binary quality measure = " << testResultTest2.getBinaryQualityMeasure() << std::endl;
    fullprint << "testResultTest2 p value = " << testResultTest2.getPValue() << std::endl;
    fullprint << "testResultTest2 p value threshold = " << testResultTest2.getThreshold() << std::endl;
    fullprint << "testResultTest2 test type = " << testResultTest2.getTestType() << std::endl;

    /** TEST NUMBER THREE : EQUALITY */
    fullprint << "testResultTest2 = testResultTest " << (testResultTest2 == testResultTest) << std::endl;
    String typeDiff = "testResult2";
    Bool testBoolDiff = 0;
    Scalar testValDiff = 0.02;
    Scalar testThresDiff = 0.12;
    Scalar statisticDiff = 0.19;
    TestResult testResultTestDiff(typeDiff, testBoolDiff, testValDiff, testThresDiff, statisticDiff);
    fullprint << "testResultTestDiff = testResultTest " << (testResultTestDiff == testResultTest) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
