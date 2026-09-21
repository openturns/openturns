#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object: X uniform over a reference triangle,
# mapped by the piecewise-affine map (x, y) -> (x, y, x + y) onto a plane
# embedded in R^3. The intrinsic density is uniform and equal to
# 1/vol(D) / sqrt(det(J J^T)) = 2 / sqrt(3)
mesh = ot.Mesh(ot.Sample([[0.0, 0.0], [1.0, 0.0], [0.0, 1.0]]),
               ot.IndicesCollection([[0, 1, 2]]))
values = ot.Sample([[0.0, 0.0, 0.0], [1.0, 0.0, 1.0], [0.0, 1.0, 1.0]])
field = ot.Field(mesh, values)
distribution = otexp.PushForwardOverMesh(field)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())
assert distribution.isContinuous()

# Dimension of the distribution
print("Dimension = ", distribution.getDimension())
assert distribution.getDimension() == 3
print("Intrinsic dimension = ", distribution.getIntrinsicDimension())
assert distribution.getIntrinsicDimension() == 2

# Image mesh
imageMesh = distribution.getMesh()
print("Image mesh vertices = ", imageMesh.getVerticesNumber(),
      " simplices = ", imageMesh.getSimplicesNumber())
assert imageMesh.getVerticesNumber() == 3

# PDF: uniform intrinsic density equal to 2 / sqrt(3) on the image
expectedPdf = 2.0 / (3.0 ** 0.5)
for point in [[0.1, 0.2, 0.3], [0.5, 0.4, 0.9], [0.05, 0.05, 0.1]]:
    pdf = distribution.computePDF(point)
    print("pdf(", point, ")=", pdf)
    ott.assert_almost_equal(pdf, expectedPdf, 1e-6, 0.0)

# Points outside the image have a zero density
for point in ([-0.1, 0.0, 0.0], [0.7, 0.7, 0.7], [1.0, 0.0, 0.0],
              [0.0, 0.0, 0.1]):
    print("pdf off image(", point, ")=", distribution.computePDF(point))
    ott.assert_almost_equal(distribution.computePDF(point), 0.0, 0.0, 1e-12)

# Range contains the image
print("range=", distribution.getRange())
assert distribution.getRange().contains([0.25, 0.25, 0.5])
assert not distribution.getRange().contains([5.0, 5.0, 5.0])

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)
assert oneRealization.getDimension() == 3
# The realization lies on the image plane x + y = z
ott.assert_almost_equal(oneRealization[0] + oneRealization[1],
                        oneRealization[2], 1e-10, 0.0)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
# All the samples lie on the image
count = 0
for i in range(size):
    if distribution.computePDF(oneSample[i]) > 0.0:
        count += 1
print("on image fraction=", count / size)
assert count == size

# Identity P1 map over a square reference mesh: the distribution reduces to
# the uniform distribution over the mesh
xs = [0.0, 0.5, 1.0]
squareVertices = ot.Sample([[xs[i], xs[j]] for j in range(3)
                            for i in range(3)])
squareSimplices = ot.IndicesCollection(
    [[0, 1, 4], [0, 4, 3], [1, 2, 5], [1, 5, 4], [3, 4, 7], [3, 7, 6],
     [4, 5, 8], [4, 8, 7]]
)
squareMesh = ot.Mesh(squareVertices, squareSimplices)
squareField = ot.Field(squareMesh, squareVertices)
uniformClass = otexp.PushForwardOverMesh(squareField)
uniformOverMesh = ot.UniformOverMesh(squareMesh)
for point in [[0.25, 0.25], [0.75, 0.75], [0.5, 0.5]]:
    print("identity pdf(", point, ")=", uniformClass.computePDF(point))
    ott.assert_almost_equal(uniformClass.computePDF(point),
                            uniformOverMesh.computePDF(point), 1e-10, 0.0)

# Non-uniform antecedent: X ~ Dirichlet(2, 2, 2) supported on the reference
# triangle, pushed by the affine map. The density at the preimage u is
# f_X(u) * sqrt(det(G_ref) / det(G_img)) = f_X(u) / sqrt(3)
dirichletAntecedent = ot.Dirichlet([2.0, 2.0, 2.0])
dirichletDistribution = otexp.PushForwardOverMesh(dirichletAntecedent, field)
u = [0.2, 0.3]
expectedDirichletPdf = dirichletAntecedent.computePDF(u) * (
    3.0 ** -0.5
)
print("Dirichlet pdf =", dirichletDistribution.computePDF([0.2, 0.3, 0.5]))
ott.assert_almost_equal(
    dirichletDistribution.computePDF([0.2, 0.3, 0.5]),
    expectedDirichletPdf, 1e-6, 0.0)

# Folding map in dimension 1: two reference segments [0, 1] and [1, 2] are
# both mapped onto the segment [0, 1], the second one being contracted by a
# factor 2. Points in (0.5, 1] have two preimages, the others have one.
foldMesh = ot.Mesh(ot.Sample([[0.0], [1.0], [2.0]]),
                   ot.IndicesCollection([[0, 1], [1, 2]]))
foldValues = ot.Sample([[0.0], [1.0], [0.5]])
foldDistribution = otexp.PushForwardOverMesh(ot.Field(foldMesh, foldValues))
# pdf(0.75) = p_X(u1) / |det J1| + p_X(u2) / |det J2|
#           = 0.5 / 1 + 0.5 / 0.5
print("fold pdf(0.75)=", foldDistribution.computePDF([0.75]))
ott.assert_almost_equal(foldDistribution.computePDF([0.75]), 1.5, 1e-10, 0.0)
# pdf(0.4) = 0.5 / 1 (single preimage in the first segment)
print("fold pdf(0.4)=", foldDistribution.computePDF([0.4]))
ott.assert_almost_equal(foldDistribution.computePDF([0.4]), 0.5, 1e-10, 0.0)
# pdf(1.5) = 0 (outside the image)
print("fold pdf(1.5)=", foldDistribution.computePDF([1.5]))
ott.assert_almost_equal(foldDistribution.computePDF([1.5]), 0.0, 0.0, 1e-12)
# The whole probability mass integrates to one
foldSample = foldDistribution.getSample(1000)
allInImage = all(0.0 <= x[0] <= 1.0 for x in foldSample)
print("fold sample in image=", allInImage)
assert allInImage

# Moments estimated by Monte-Carlo, consistent with the sampled moments
mean = distribution.getMean()
print("mean=", mean)
sampledMean = oneSample.computeMean()
for k in range(3):
    ott.assert_almost_equal(mean[k], sampledMean[k], 1e-1, 1e-1)

# Accessors
antecedent = ot.UniformOverMesh(mesh)
distribution.setAntecedent(antecedent)
assert distribution.getAntecedent() == antecedent
assert distribution.getValues() == field
newValues = ot.Sample([[0.0, 0.0, 0.0], [2.0, 0.0, 2.0], [0.0, 2.0, 2.0]])
newField = ot.Field(mesh, newValues)
distribution.setValues(newField)
assert distribution.getValues() == newField
expectedPdfScaled = 1.0 / (2.0 * math.sqrt(3.0))
print("scaled pdf=", distribution.computePDF([0.5, 0.5, 1.0]))
ott.assert_almost_equal(distribution.computePDF([0.5, 0.5, 1.0]),
                        expectedPdfScaled, 1e-6, 0.0)

# Default constructor
defaultDistribution = otexp.PushForwardOverMesh()
print("default pdf(0.5)=", defaultDistribution.computePDF([0.5]))
ott.assert_almost_equal(defaultDistribution.computePDF([0.5]), 1.0, 1e-10, 0.0)

# Exception tests: methods that throw exceptions
with ott.assert_raises(TypeError):
    # the antecedent must be absolutely continuous
    otexp.PushForwardOverMesh(ot.Dirac(ot.Point([0.0, 0.0])), field)
with ott.assert_raises(TypeError):
    # the antecedent dimension must match the reference mesh dimension
    otexp.PushForwardOverMesh(ot.Normal(1), field)
with ott.assert_raises(TypeError):
    # the field must not be empty
    otexp.PushForwardOverMesh(ot.Field())
