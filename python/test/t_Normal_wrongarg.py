#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    try:

        # Instanciate one distribution object
        meanPoint = Point(1)
        meanPoint[0] = 1.0
        sigma = Point(1)
        sigma[0] = 1.0
        R = CorrelationMatrix(1)
        distribution = Normal(meanPoint, sigma, R)
        print("Distribution ", repr(distribution))

        # We try to set an erroneous covariance matrix (wrong dimension) into
        # distribution
        newR = CorrelationMatrix(2)
        distribution.setCorrelationMatrix(newR)

        # Normally, we should never go here
        raise

    except:
        pass

# except TestFailed, ex :
except:
    import sys
    print("t_Normal_wrongarg.py", sys.exc_info()[0], sys.exc_info()[1])
