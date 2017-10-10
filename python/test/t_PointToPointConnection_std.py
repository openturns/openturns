#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Construction based on two functions
    # Create an instance */
    myFunc = PointToPointConnection(
        SymbolicFunction("x", "sin(x)"), SymbolicFunction("x", "cos(x)"))

    print("myFunc=", myFunc)
    # Get the input and output description */
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension */
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Connection on a point */
    point = [1.0] * myFunc.getInputDimension()
    print("point=", point)
    print("myFunc(point)=", myFunc(point))
    print("myFunc.gradient(point)=", myFunc.gradient(point))
    print("myFunc.hessian(point)=", myFunc.hessian(point))
    # Get the number of calls */
    print("called ", myFunc.getCallsNumber(), " times")

    # Construction based on a PointToFieldFunction followed by a FieldToPointFunction
    # Create a KarhunenLoeveResult
    mesh = IntervalMesher([9]).build(Interval(-1.0, 1.0))
    cov1D = AbsoluteExponential([1.0])
    algo = KarhunenLoeveP1Algorithm(mesh, cov1D, 1.0e-2)
    algo.run()
    result = algo.getResult()
    # Create a PointToFieldFunction
    lifting = KarhunenLoeveLifting(result)
    # Create a FieldToPointFunction
    projection = KarhunenLoeveProjection(result)
    # Create an instance */
    myFunc = PointToPointConnection(projection, lifting)

    print("myFunc=", myFunc)
    # Get the input and output description */
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension */
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Connection on a point */
    point = [1.0] * myFunc.getInputDimension()
    print("point=", point)
    print("myFunc(point)=", myFunc(point))
    print("myFunc.gradient(point)=", myFunc.gradient(point).clean(1.0e-8))
    print("myFunc.hessian(point)=", myFunc.hessian(point).clean(1.0e-7))
    # Get the number of calls */
    print("called ", myFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_PointToPointConnection_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
