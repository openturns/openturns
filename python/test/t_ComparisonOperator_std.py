#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    less = Less()
    print("Less(10,20) = ", less(10, 20))
    print("Less(20,20) = ", less(20, 20))
    print("Less(30,20) = ", less(30, 20))

    lessOrEqual = LessOrEqual()
    print("LessOrEqual(10,20) = ", lessOrEqual(10, 20))
    print("LessOrEqual(20,20) = ", lessOrEqual(20, 20))
    print("LessOrEqual(30,20) = ", lessOrEqual(30, 20))

    equal = ComparisonOperator(Equal())
    print("Equal(10,20) = ", equal(10, 20))
    print("Equal(20,20) = ", equal(20, 20))
    print("Equal(30,20) = ", equal(30, 20))

    greaterOrEqual = GreaterOrEqual()
    print("GreaterOrEqual(10,20) = ", greaterOrEqual(10, 20))
    print("GreaterOrEqual(20,20) = ", greaterOrEqual(20, 20))
    print("GreaterOrEqual(30,20) = ", greaterOrEqual(30, 20))

    greater = Greater()
    print("Greater(10,20) = ", greater(10, 20))
    print("Greater(20,20) = ", greater(20, 20))
    print("Greater(30,20) = ", greater(30, 20))

except:
    import sys
    print("t_ComparisonOperator_std.py", sys.exc_info()[0], sys.exc_info()[1])
