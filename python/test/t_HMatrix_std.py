#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m


class TestHMatrixRealAssemblyFunction(object):

    def __init__(self, vertices, scaling=1.0):
        self.vertices = vertices
        self.scaling = scaling

    def __call__(self, i, j):
        pt1 = self.vertices[i]
        pt2 = self.vertices[j]
        difference = pt1 - pt2
        val = m.exp(-difference.norm() / self.scaling)
        return val


ot.ResourceMap.SetAsBool('HMatrix-ForceSequential', True)
ot.ResourceMap.SetAsUnsignedInteger('HMatrix-MaxLeafSize', 10)

ot.PlatformInfo.SetNumericalPrecision(3)

n = 2
indices = [n, n]
intervalMesher = ot.IntervalMesher(indices)
interval = ot.Interval([0.0] * 2, [1.0] * 2)
mesh2D = intervalMesher.build(interval)
vertices = mesh2D.getVertices()

factory = ot.HMatrixFactory()
parameters = ot.HMatrixParameters()
parameters.setAssemblyEpsilon(1.e-6)
parameters.setRecompressionEpsilon(1.e-6)
# HMatrix must be symmetric in order to perform Cholesky decomposition
hmat = factory.build(vertices, 1, True, parameters)
simpleAssembly = TestHMatrixRealAssemblyFunction(vertices, 0.1)

hmat.assembleReal(simpleAssembly, 'L')

hmatRef = ot.HMatrix(hmat)

hmat.factorize('LLt')

# Compute A - L*L^T
hmatRef.gemm('N', 'T', -1.0, hmat, hmat, 1.0)

# Check LU factorization
hmat = factory.build(vertices, 1, False, parameters)
hmat.assembleReal(simpleAssembly, 'N')
hmat.factorize('LU')

print('rows=', hmat.getNbRows())
print('columns=', hmat.getNbColumns())
print('norm=', ot.Point(1, hmat.norm()))
if hmatRef.norm() < 1.e-3:
    print('norm(A-LLt) < 1e-3')
else:
    print('norm(A-LLt) =', hmatRef.norm())
print('diagonal=', hmat.getDiagonal())
print('compressionRatio= (%d, %d)' % hmat.compressionRatio())
print('fullrkRatio= (%d, %d)' % hmat.fullrkRatio())

# vector multiply
y = ot.Point(hmat.getNbColumns())
x = [2.0] * hmat.getNbColumns()
hmat.gemv('N', 1.0, x, 3.0, y)
print('y=', y)


# block assembly
class TestHMatrixTensorRealAssemblyFunction(object):

    def __init__(self, covarianceModel, vertices):
        self.covarianceModel = covarianceModel
        self.vertices = vertices

    def __call__(self, i, j):
        pt1 = self.vertices[i]
        pt2 = self.vertices[j]
        val = self.covarianceModel(pt1, pt2)
        return val


covarianceModel = ot.ExponentialModel([0.1] * 2, [1.0] * 2)
hmat = factory.build(
    vertices, covarianceModel.getOutputDimension(), True, parameters)
blockAssembly = TestHMatrixTensorRealAssemblyFunction(
    covarianceModel, vertices)
hmat.assembleTensor(blockAssembly, covarianceModel.getOutputDimension(), 'L')
hmatRef = ot.HMatrix(hmat)
hmat.factorize('LLt')
normL = hmat.norm()
hmatRef.gemm('N', 'T', -1.0, hmat, hmat, 1.0)
if hmatRef.norm() < 1e-3:
    print('norm(A-LLt) < 1e-3')
else:
    print('norm(A-LLt) =', hmatRef.norm())

alpha = 0.1
hmat.scale(alpha)
normScaled = hmat.norm()
if abs(normL - normScaled / alpha) < 1e-10:
    print('|norm(L) - 10 * norm(0.1*L)| < 1e-10')
else:
    print('|norm(L) - 10 * norm(0.1*L)| > 1e-10')
