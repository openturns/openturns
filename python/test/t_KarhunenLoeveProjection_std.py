#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    mesh = IntervalMesher([9]).build(Interval(-1.0, 1.0))
    cov1D = AbsoluteExponential([1.0])
    algo = KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
    algo.run()
    result = algo.getResult()
    projection = KarhunenLoeveProjection(result)
    print("projection=", projection)
    # Get the input and output description
    print("projection input description=", projection.getInputDescription())
    print("projection output description=", projection.getOutputDescription())
    # Get the spatial, input and output dimension
    print("projection input dimension=",
          projection.getInputMesh().getDimension())
    print("projection input dimension=", projection.getInputDimension())
    print("projection output dimension=", projection.getOutputDimension())
    field = result.getModesAsProcessSample().computeMean()
    point = projection(field)
    print("field=", field)
    print("point=", point)
    # Get the number of calls
    print("called ", projection.getCallsNumber(), " times")

except:
    import sys
    print("t_KarhunenLoeveProjection_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
