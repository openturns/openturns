#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Create an instance
    myFunc = PointToPointEvaluation(
        SymbolicFunction("x", "sin(x)"), SymbolicFunction("x", "cos(x)"))

    print("myFunc=", myFunc)
    # Get the input and output description
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Evaluation on a point
    point = [1.0] * myFunc.getInputDimension()
    print("point=", point)
    print("myFunc(point)=", myFunc(point))
    # Get the number of calls
    print("called ", myFunc.getCallsNumber(), " times")

    # Construction based on a PointToFieldFunction followed by a FieldToPointFunction
    # Create a KarhunenLoeveResult
    mesh = IntervalMesher([9]).build(Interval(-1.0, 1.0))
    cov1D = AbsoluteExponential([1.0])
    algo = KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
    algo.run()
    result = algo.getResult()
    # Create a PointToFieldFunction
    lifting = KarhunenLoeveLifting(result)
    # Create a FieldToPointFunction
    projection = KarhunenLoeveProjection(result)
    # Create an instance
    myFunc = PointToPointEvaluation(projection, lifting)

    print("myFunc=", myFunc)
    # Get the input and output description
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Evaluation on a point
    point = [1.0] * myFunc.getInputDimension()
    print("point=", point)
    print("myFunc(point)=", myFunc(point))
    # Get the number of calls
    print("called ", myFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_PointToPointEvaluation_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
