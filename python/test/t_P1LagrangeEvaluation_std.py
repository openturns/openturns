#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott


mesh = ot.RegularGrid(0.0, 1.0, 4)
values = ot.Sample([[0.5], [1.5], [1.0], [-0.5]])
field = ot.Field(mesh, values)
evaluation = ot.P1LagrangeEvaluation(field)
x = [2.3]
y = evaluation(x)
print(y)
ott.assert_almost_equal(y, [0.55])


# Learning sample on meshD
mesher = ot.IntervalMesher([7, 7])
lowerbound = [-1.0, -1.0]
upperBound = [1, 1]
interval = ot.Interval(lowerbound, upperBound)
meshD = mesher.build(interval)
sample = ot.ProcessSample(meshD, 10, 1)
field = ot.Field(meshD, 1)
for k in range(sample.getSize()):
    field.setValues(ot.Normal().getSample(64))
    sample[k] = field
lagrange = ot.P1LagrangeEvaluation(sample)
# New mesh
mesh = ot.Mesh(
    ot.MonteCarloExperiment(
        ot.ComposedDistribution([ot.Uniform(-1, 1)] * 2), 200
    ).generate()
)

point = mesh.getVertices()[0]
y = lagrange(point)
print(y)
index = lagrange.getEnclosingSimplexAlgorithm().query(point)
print(index)
assert index == 12, "wrong index"
