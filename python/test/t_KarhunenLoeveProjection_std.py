#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
projection = ot.KarhunenLoeveProjection(result)
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

