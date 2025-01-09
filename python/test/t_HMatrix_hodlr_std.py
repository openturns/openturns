#! /usr/bin/env python

import openturns as ot
import math as m
import openturns.testing as ott


class TestHMatrixRealAssemblyFunction:
    def __init__(self, vertices, scaling=1.0):
        self.vertices = vertices
        self.scaling = scaling

    def __call__(self, i, j):
        pt1 = self.vertices[i]
        pt2 = self.vertices[j]
        difference = pt1 - pt2
        val = m.exp(-difference.norm() / self.scaling)
        return val

    def eval(self, pt1, pt2):
        difference = pt1 - pt2
        val = m.exp(-difference.norm() / self.scaling)
        return val


ot.ResourceMap.SetAsBool("HMatrix-ForceSequential", True)
ot.ResourceMap.SetAsUnsignedInteger("HMatrix-MaxLeafSize", 225)
ot.ResourceMap.SetAsUnsignedInteger("HMatrix-ClusteringMaxDof", 225)
ot.ResourceMap.SetAsString("HMatrix-Admissibility", "hodlr")

ot.PlatformInfo.SetNumericalPrecision(3)

n = 29
indices = [n, n]
intervalMesher = ot.IntervalMesher(indices)
interval = ot.Interval([0.0] * 2, [1.0] * 2)
mesh2D = intervalMesher.build(interval)
vertices = mesh2D.getVertices()

factory = ot.HMatrixFactory()
parameters = ot.HMatrixParameters()
parameters.setAssemblyEpsilon(1.0e-8)
parameters.setRecompressionEpsilon(1.0e-8)
parameters.setAdmissibility("hodlr")
# HMatrix must be symmetric in order to perform Cholesky-like decomposition
hmat = factory.build(vertices, 1, True, parameters)
simpleAssembly = TestHMatrixRealAssemblyFunction(vertices, 0.1)

hmat.assembleReal(simpleAssembly, "L")

hmatRef = ot.HMatrix(hmat)
hmat.factorize("hodlrsym")

# Compute A - L*L^T
# hmatRef.gemm("N", "T", -1.0, hmat, hmat, 1.0)
refNorm = hmatRef.norm()
# Compute the norm of the difference between the original matrix and the product of the factors using a stochastic method.
nSamples = 1000
error = ot.Point(hmat.getNbRows())
normDiff = 0.0
for i in range(nSamples):
    # Generate a random sample of vectors (Normal distribution)
    randomSample = ot.DistFunc.rNormal(hmat.getNbRows())
    # hmat * (hmat^T * randomSample[i]) -->  first (hmat^T * randomSample[i])
    hmat.gemv('T', 1., randomSample, 0., error)
    # Then (hmat * error) --> error
    hmat.gemv('N', 1., error, 0., error)
    # hmatRef * randomSample[i]
    hmatRef.gemv('N', 1., randomSample, -1.0, error)
    normDiff += error.normSquare()

normDiff = m.sqrt(normDiff / nSamples)
threshold = 2.5e-3
ott.assert_almost_equal(normDiff / refNorm, 2.54e-3, 1e-5, 1e-5)

# vector multiply
y = ot.Point(hmat.getNbColumns())
x = [2.0] * hmat.getNbColumns()
hmat.gemv("N", 1.0, x, 3.0, y)

normL = hmat.norm()
alpha = 0.1
hmat.scale(alpha)
normScaled = hmat.norm()
ott.assert_almost_equal(normL, normScaled / alpha, 1e-10, 1e-7)

# Use the HODLR factorization
hmat = factory.build(vertices, 1, True, parameters)
hmat.assembleReal(simpleAssembly, "N")
hmat.factorize("hodlr")

mean = vertices.computeMean()
rhs = ot.Point(vertices.getSize())

for i in range(vertices.getSize()):
    rhs[i] = simpleAssembly.eval(vertices[i], mean)

rhsCopy = ot.Point(rhs)
rhsCopyNorm = rhsCopy.norm()

result = hmat.solve(rhs)

for i in range(vertices.getSize()):
    for j in range(vertices.getSize()):
        rhsCopy[i] -= simpleAssembly.eval(vertices[i], vertices[j]) * result[j]

diffNorm = rhsCopy.norm()
threshold = 2.e-3
ott.assert_almost_equal(abs(diffNorm - threshold * rhsCopyNorm), 2.4613318835756803e-3, 1e-5, 1e-5)
