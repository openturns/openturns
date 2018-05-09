#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # Create a KarhunenLoeveResult
    mesh = IntervalMesher([9]).build(Interval(-1.0, 1.0))
    cov1D = AbsoluteExponential([1.0])
    algo = KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
    algo.run()
    result = algo.getResult()
    projection = KarhunenLoeveProjection(result)
    # Construction based on a FieldFunction followed by a FieldToPointFunction
    fieldFunction = ValueFunction(SymbolicFunction("x", "x"), mesh)
    # Create an instance
    myFunc = FieldToPointConnection(projection, fieldFunction)

    print("myFunc=", myFunc)
    # Get the input and output description
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Connection on a field
    field = result.getModesAsProcessSample().computeMean()
    print("field=", field)
    print("myFunc(field)=", myFunc(field.getValues()))
    # Get the number of calls
    print("called ", myFunc.getCallsNumber(), " times")

    # Construction based on a FieldToPointFunction followed by a Function
    dimension = projection.getOutputDimension()
    function = SymbolicFunction(Description.BuildDefault(
        dimension, "x"), Description.BuildDefault(dimension, "x"))
    # Create an instance
    myFunc = FieldToPointConnection(function, projection)

    print("myFunc=", myFunc)
    # Get the input and output description
    print("myFunc input description=", myFunc.getInputDescription())
    print("myFunc output description=", myFunc.getOutputDescription())
    # Get the input and output dimension
    print("myFunc input dimension=", myFunc.getInputDimension())
    print("myFunc output dimension=", myFunc.getOutputDimension())
    # Connection on a field
    field = result.getModesAsProcessSample().computeMean()
    print("field=", field)
    print("myFunc(field)=", myFunc(field.getValues()))
    # Get the number of calls
    print("called ", myFunc.getCallsNumber(), " times")

except:
    import sys
    print("t_FieldToPointConnection_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
