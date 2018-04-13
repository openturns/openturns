#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Construction based on two functions
    # Create an instance
    grid = RegularGrid(0.0, 0.1, 11)
    myFunc = FieldToFieldConnection(
        ValueFunction(SymbolicFunction("x", "sin(x)"), grid), ValueFunction(SymbolicFunction("x", "cos(x)"), grid))

    print("myFunc=", myFunc)
    # Get the input and output description
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Connection on a point
    field = Field(grid, grid.getVertices() * [2.0])
    print("field=", field)
    print("myFunc(field)=", myFunc(field))
    # Get the number of calls
    print("called ", myFunc.getCallsNumber(), " times")

    # Construction based on a FieldToPointFunction followed by a PointToFieldFunction
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
    myFunc = FieldToFieldConnection(lifting, projection)

    print("myFunc=", myFunc)
    # Get the input and output description
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Connection on a point
    field = result.getModesAsProcessSample().computeMean()
    print("field=", field)
    print("myFunc(field)=", myFunc(field))
    # Get the number of calls
    print("called ", myFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_FieldToFieldConnection_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
