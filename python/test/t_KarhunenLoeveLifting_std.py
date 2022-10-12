#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
lifting = ot.KarhunenLoeveLifting(result)
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
