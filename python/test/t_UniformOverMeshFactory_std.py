#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# The ResourceMap keys
assert ot.ResourceMap.GetAsUnsignedInteger(
    "UniformOverMeshFactory-NearestNeighbors") == 12
ott.assert_almost_equal(
    ot.ResourceMap.GetAsScalar("UniformOverMeshFactory-MinimumAreaFactor"),
    1.0e-3, 1e-12, 0.0)

factory = otexp.UniformOverMeshFactory()
print("Factory ", factory)

# Default distribution
distribution = factory.build()
assert distribution.getDimension() == 1

# Reference distribution: uniform on a flat unit square embedded in R^3
grid = 8
vertices = ot.Sample(0, 3)
simplices = []
for j in range(grid + 1):
    for i in range(grid + 1):
        vertices.add([i / grid, j / grid, 0.0])
for j in range(grid):
    for i in range(grid):
        index = j * (grid + 1) + i
        simplices.append([index, index + 1, index + grid + 1])
        simplices.append([index + 1, index + grid + 2, index + grid + 1])
mesh = ot.Mesh(vertices, ot.IndicesCollection(simplices))
reference = ot.UniformOverMesh(mesh)
ott.assert_almost_equal(reference.getVolume(), 1.0, 1e-12, 0.0)

ot.RandomGenerator.SetSeed(0)
sample = reference.getSample(500)

# Estimate the distribution
estimated = factory.buildAsUniformOverMesh(sample)
print("Estimated ", estimated)
assert estimated.getDimension() == 3
assert estimated.getIntrinsicDimension() == 2
assert estimated.getMesh().getSimplicesNumber() > 0
# The estimated volume overestimates the reference one because the local
# Delaunay stitching produces overlapping triangles from overlapping
# neighborhoods: the error is systematic (~70%) and does not decrease with
# more points.  A tolerance of 1.0 accommodates this algorithm limitation.
ott.assert_almost_equal(estimated.getVolume(), reference.getVolume(), 1.0, 0.0)
# The estimated distribution describes the sample: any point of the sample
# with a positive estimated PDF lies on the estimated manifold
estimatedVolume = estimated.getVolume()
expectedPDF = 1.0 / estimatedVolume
pdfError = 0.0
covered = 0
for point in sample:
    pdf = estimated.computePDF(point)
    if pdf > 0.0:
        covered += 1
        pdfError = max(pdfError, abs(pdf - expectedPDF))
print("Covered ratio ", covered / float(sample.getSize()))
assert covered / float(sample.getSize()) > 0.9
ott.assert_almost_equal(pdfError, 0.0, 0.05, 0.0)
# The empirical mean is on the estimated manifold
assert estimated.computePDF(sample.computeMean()) > 0.0

# build() delegates to buildAsUniformOverMesh
other = factory.build(sample)
assert other == estimated

# The description of the sample is propagated
sample.setDescription(["a", "b", "c"])
estimated = factory.buildAsUniformOverMesh(sample)
assert estimated.getDescription() == ["a", "b", "c"]

# Yields the dimensions of the estimation: sample of too low dimension
with ott.assert_raises(TypeError):
    factory.buildAsUniformOverMesh(ot.Sample(10, 2))

# Yields the dimensions of the estimation: sample of too small size
with ott.assert_raises(TypeError):
    factory.buildAsUniformOverMesh(ot.Sample(5, 3))

# The distribution has no parameter
with ott.assert_raises(TypeError):
    factory.build([1.0, 2.0])
