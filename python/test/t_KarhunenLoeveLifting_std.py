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
    lifting = KarhunenLoeveLifting(result)
    print("lifting=", lifting)
    # Get the input and output description
    print("lifting input description=", lifting.getInputDescription())
    print("lifting output description=", lifting.getOutputDescription())
    # Get the input and output dimension
    print("lifting input dimension=", lifting.getInputDimension())
    print("lifting output dimension=", lifting.getOutputDimension())
    point = [1.0] * lifting.getInputDimension()
    field = lifting(point)
    print("point=", point)
    print("field=", field)
    # Get the number of calls
    print("called ", lifting.getCallsNumber(), " times")

except:
    import sys
    print("t_KarhunenLoeveLifting.py", sys.exc_info()[0], sys.exc_info()[1])
