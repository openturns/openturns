#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    levelFunction = NumericalMathFunction(
        ["x1", "x2", "x3", "x4"], ["y1"], ["x1+2*x2-3*x3+4*x4"])
    specific = TNCSpecificParameters()
    startingPoint = NumericalPoint(4, 1.0)
    bounds = Interval(NumericalPoint(4, -3.0), NumericalPoint(4, 5.0))
    myAlgorithm = TNCObsolete(specific, levelFunction, bounds, startingPoint,
                      BoundConstrainedAlgorithmImplementationResult.MINIMIZATION)
    myAlgorithm.setMaximumEvaluationsNumber(100)
    myAlgorithm.setMaximumAbsoluteError(1.0e-10)
    myAlgorithm.setMaximumRelativeError(1.0e-10)
    myAlgorithm.setMaximumObjectiveError(1.0e-10)
    myAlgorithm.setMaximumConstraintError(1.0e-10)
    print("myAlgorithm = ", myAlgorithm)
except:
    import sys
    print("t_TNC_std.py", sys.exc_info()[0], sys.exc_info()[1])
