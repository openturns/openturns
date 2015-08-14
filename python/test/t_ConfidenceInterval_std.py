#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    # TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER
    print("test number zero : default constructor and string converter")

    # Default constructor
    begin = 1.0
    end = 36.92
    testInterval = ConfidenceInterval(begin, end)
    testInterval.setName("testInterval")

    # TEST NUMBER ONE : STRING CONVERTER
    print("test number one : string converter")

    # String converter
    print("testInterval = ", testInterval)

    # TEST NUMBER TWO : ELEMENTS ACCESSORS
    print("test number two : elements accessors")

    begin2 = 12.0
    end2 = 92.3898754
    testInterval2 = ConfidenceInterval(begin2, end2)

    print("testInterval2 values = ", testInterval2.getFirst(),
          "  ", testInterval2.getLast())
    print("testInterval  values = ", testInterval.getFirst(),
          "  ", testInterval.getLast())

    # TEST NUMBER THREE : EQUALITY TEST
    print("testInterval == testInterval2 ", (testInterval == testInterval2))

except:
    import sys
    print("t_ConfidenceInterval_std.py", sys.exc_info()[0], sys.exc_info()[1])
