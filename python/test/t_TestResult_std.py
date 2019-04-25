#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER
    print("test number zero : default constructor and string converter")

    # Default constructor
    type = "testResult"
    testBool = True
    testVal = 0.372
    testThres = 0.42
    statistic = 0.59
    testResultTest = TestResult(type, testBool, testVal, testThres, statistic)

    # String converter
    print("testResultTest = ", testResultTest)

    # TEST NUMBER ONE : STRING CONVERTER
    print("test number one : string converter")

    # String converter
    print("testResultTest = ", testResultTest)

    # TEST NUMBER TWO : ELEMENTS ACCESSORS
    print("test number two : elements accessors")
    print("testResultTest2 binary quality measure = ",
          testResultTest.getBinaryQualityMeasure())
    print("testResultTest2 p value = %.6f" % testResultTest.getPValue())
    print("testResultTest2 p value threshold = %.6f" %
          testResultTest.getThreshold())
    print("testResultTest2 test type = ", testResultTest.getTestType())

    # TEST NUMBER THREE : EQUALITY
    print("test number three : equality")
    typeDiff = "testResult2"
    testBoolDiff = False
    testValDiff = 0.02
    testThresDiff = 0.12
    statisticDiff = 0.19
    testResultTestDiff = TestResult(
        typeDiff, testBoolDiff, testValDiff, testThresDiff, statisticDiff)
    print("testResultTestDiff = testResultTest ", (
        testResultTestDiff == testResultTest))

except:
    import sys
    print("t_TestResult_std.py", sys.exc_info()[0], sys.exc_info()[1])
