#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(0)


def buildMesh(vertices, vertexIndices):
    # Build a mesh following the Mesh convention: the simplex of a mesh of
    # intrinsic dimension d embedded in an ambient space of dimension D has
    # d+1 distinct leading vertex indices followed by trailing entries equal
    # to the last distinct vertex index, so that the number of vertex indices
    # per simplex is D+1
    dimension = vertices.getDimension()
    padded = []
    for row in vertexIndices:
        padded.append([row[min(j, len(row) - 1)] for j in range(dimension + 1)])
    return ot.Mesh(vertices, ot.IndicesCollection(padded))


# Uniform over a unit right triangle embedded in R^3
vertices = ot.Sample([[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [0.0, 1.0, 0.0]])
mesh = buildMesh(vertices, [[0, 1, 2]])
print("Mesh intrinsic dimension ", mesh.getIntrinsicDimension())
print("Mesh volume ", mesh.computeSimplicesVolume())
distribution = ot.UniformOverMesh(mesh)
print("Distribution ", distribution)

assert distribution.getDimension() == 3
assert distribution.getIntrinsicDimension() == 2
ott.assert_almost_equal(mesh.computeSimplicesVolume(), [0.5], 1e-12, 0.0)
ott.assert_almost_equal(distribution.getVolume(), 0.5, 1e-12, 0.0)
# The range is the bounding box of the vertices in the ambient space
assert distribution.getRange() == ot.Interval([0.0] * 3, [1.0, 1.0, 0.0])

# PDF on the mesh is the inverse of the intrinsic volume
ott.assert_almost_equal(distribution.computePDF([0.2, 0.3, 0.0]), 2.0, 1e-12, 0.0)
ott.assert_almost_equal(distribution.computeLogPDF([0.2, 0.3, 0.0]), 0.6931471805599453, 1e-12, 0.0)

# PDF off the mesh is zero
assert distribution.computePDF([1.5, 0.0, 0.0]) == 0.0
assert distribution.computePDF([0.5, 0.5, 0.5]) == 0.0
# A point too far from the mesh is not considered on the mesh
assert distribution.computePDF([0.2, 0.3, 1.0e-9]) == 0.0
assert distribution.computeLogPDF([1.5, 0.0, 0.0]) == -float("inf")

# The mean and the covariance of the uniform on the right triangle, the
# third coordinate being deterministic equal to zero
mean = distribution.getMean()
ott.assert_almost_equal(mean, [1.0 / 3.0, 1.0 / 3.0, 0.0], 1e-12, 0.0)
covariance = distribution.getCovariance()
expectedCovariance = ot.CovarianceMatrix(
    [[1.0 / 18.0, -1.0 / 36.0, 0.0], [-1.0 / 36.0, 1.0 / 18.0, 0.0], [0.0, 0.0, 0.0]])
ott.assert_almost_equal(covariance, expectedCovariance, 1e-12, 0.0)

# The realizations lie on the mesh
size = 10000
sample = distribution.getSample(size)
empiricalMean = sample.computeMean()
ott.assert_almost_equal(empiricalMean, mean, 0.02, 0.0)
ott.assert_almost_equal(sample.computeCovariance(), covariance, 0.05, 0.0)
# The empirical covariance of the first two coordinates is singular: the
# sample spans only the triangle
assert sample.getMarginal(2).getMax()[0] == 0.0
assert sample.getMarginal(2).getMin()[0] == 0.0
assert sample.computeMean()[2] == 0.0
assert sample.getMin()[0] >= -1.0e-12
assert sample.getMin()[1] >= -1.0e-12
assert sample.getMax()[0] - 1.0e-12 <= 1.0 + 1.0e-12
assert sample.getMax()[1] - 1.0e-12 <= 1.0 + 1.0e-12

# The probability of an interval is not implemented for embedded meshes
with ott.assert_raises(RuntimeError):
    distribution.computeProbability(ot.Interval([0.0] * 3, [0.5] * 3))

# Uniform over a segment embedded in R^3 (intrinsic dimension 1)
segmentVertices = ot.Sample([[0.0, 0.0, 0.0], [2.0, 1.0, 0.0]])
segmentMesh = buildMesh(segmentVertices, [[0, 1]])
assert segmentMesh.getIntrinsicDimension() == 1
segment = ot.UniformOverMesh(segmentMesh)
assert segment.getIntrinsicDimension() == 1
ott.assert_almost_equal(segmentMesh.computeSimplicesVolume(), [5.0**0.5], 1e-12, 0.0)
ott.assert_almost_equal(segment.getVolume(), 5.0**0.5, 1e-12, 0.0)
ott.assert_almost_equal(segment.getMean(), [1.0, 0.5, 0.0], 1e-12, 0.0)
ott.assert_almost_equal(
    segment.computePDF([1.0, 0.5, 0.0]), 1.0 / 5.0**0.5, 1e-12, 0.0)
assert segment.computePDF([1.0, 0.5, 0.5]) == 0.0

# Uniform over a triangulated square embedded in R^3
grid = 8
verts = ot.Sample(0, 3)
simplicesCollection = []
for j in range(grid + 1):
    for i in range(grid + 1):
        verts.add([i / grid, j / grid, 0.0])
for j in range(grid):
    for i in range(grid):
        index = j * (grid + 1) + i
        simplicesCollection.append([index, index + 1, index + grid + 1])
        simplicesCollection.append([index + 1, index + grid + 2, index + grid + 1])
squareMesh = buildMesh(verts, simplicesCollection)
assert squareMesh.getIntrinsicDimension() == 2
square = ot.UniformOverMesh(squareMesh)
assert square.getIntrinsicDimension() == 2
volumes = squareMesh.computeSimplicesVolume()
ott.assert_almost_equal(sum(volumes[i] for i in range(len(volumes))), 1.0, 1e-12, 0.0)
ott.assert_almost_equal(square.getVolume(), 1.0, 1e-12, 0.0)
ott.assert_almost_equal(square.computePDF([0.5, 0.5, 0.0]), 1.0, 1e-12, 0.0)
ott.assert_almost_equal(square.getMean(), [0.5, 0.5, 0.0], 1e-12, 0.0)
squareSample = square.getSample(10000)
ott.assert_almost_equal(squareSample.computeMean(), [0.5, 0.5, 0.0], 0.02, 0.0)

# Uniform over a StandardSimplicialMesher mesh, as in the factory use case:
# the estimated mesh has the intrinsic dimension embedded in the ambient space
print("OK")
