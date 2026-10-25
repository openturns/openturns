#! /usr/bin/env python

import math
import os

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

# Moments are deterministic SimplicialCubature integrals over the reference
# mesh, not Monte-Carlo estimates. The uniform reference triangle has mean
# (1/3, 1/3) and the map adds z = x + y
mean = distribution.getMean()
print("mean=", mean)
ott.assert_almost_equal(mean, [1.0 / 3.0, 1.0 / 3.0, 2.0 / 3.0], 1e-12,
                        1e-12)

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

# Covariances are deterministic cubature integrals
ott.assert_almost_equal(uniformOverMesh.getCovariance(),
                        ot.CovarianceMatrix([[1.0 / 12, 0.0], [0.0, 1.0 / 12]]),
                        1e-12, 1e-12)
ott.assert_almost_equal(uniformClass.getCovariance(),
                        uniformOverMesh.getCovariance(), 1e-12, 1e-12)
# The map (x, y) -> (x, y, x + y) over the unit right triangle gives
# Var = 1/18, Cov(X, Y) = -1/36 and Cov(X, Z) = Cov(Y, Z) = 1/36
print("covariance=", otexp.PushForwardOverMesh(field).getCovariance())
ott.assert_almost_equal(
    otexp.PushForwardOverMesh(field).getCovariance(),
    ot.CovarianceMatrix([[1.0 / 18, -1.0 / 36, 1.0 / 36],
                         [-1.0 / 36, 1.0 / 18, 1.0 / 36],
                         [1.0 / 36, 1.0 / 36, 1.0 / 18]]),
    1e-12, 1e-12)

# Parameters are delegated to the antecedent, which has none for UniformOverMesh
assert uniformClass.getParametersCollection().getSize() == 1
assert uniformClass.getParametersCollection()[0].getDimension() == 0
assert uniformClass.getParameterDescription() == \
    ot.UniformOverMesh(squareMesh).getParameterDescription()
uniformClass.setParameter([])
assert uniformClass.getParameter().getSize() == 0
uniformClass.setParametersCollection(
    uniformClass.getParametersCollection())

# Persistence through the Study mechanism
if ot.PlatformInfo.HasFeature("libxml2"):
    studyFile = "study_push_forward_over_mesh.xml"
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(studyFile))
    study.add("distribution", distribution)
    study.save()
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(studyFile))
    study.load()
    loaded = otexp.PushForwardOverMesh()
    study.fillObject("distribution", loaded)
    assert loaded == distribution
    os.remove(studyFile)

# Embedded reference mesh: triangle in R^3 (intrinsic 2, ambient 3) mapped
# by the same piecewise-affine map onto the plane x + y = z. The simplex
# loops must use the intrinsic dimension, not the ambient one.
embeddedVertices = ot.Sample(
    [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [0.0, 1.0, 0.0]]
)
embeddedMesh = ot.Mesh(
    embeddedVertices, ot.IndicesCollection([[0, 1, 2, 2]])
)
assert embeddedMesh.getDimension() == 3
assert embeddedMesh.getIntrinsicDimension() == 2
embeddedAntecedent = ot.UniformOverMesh(embeddedMesh)
embeddedValues = ot.Sample(
    [[0.0, 0.0, 0.0], [1.0, 0.0, 1.0], [0.0, 1.0, 1.0]]
)
embeddedDistribution = otexp.PushForwardOverMesh(
    embeddedAntecedent, ot.Field(embeddedMesh, embeddedValues)
)
assert embeddedDistribution.getDimension() == 3
assert embeddedDistribution.getIntrinsicDimension() == 2
ott.assert_almost_equal(
    embeddedDistribution.computePDF([0.1, 0.2, 0.3]), expectedPdf, 1e-6, 0.0
)
ott.assert_almost_equal(
    embeddedDistribution.computePDF([0.7, 0.7, 0.7]), 0.0, 0.0, 1e-12
)

# Co-area MVP (2D->1D, uniform): unit square projected onto the x-axis.
# The fiber at x is the vertical unit segment, J=1, so the push-forward is
# Uniform(0, 1).
squareProjVertices = ot.Sample([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [0.0, 1.0]])
squareProjMesh = ot.Mesh(
    squareProjVertices, ot.IndicesCollection([[0, 1, 2], [0, 2, 3]])
)
squareProjValues = ot.Sample([[0.0], [1.0], [1.0], [0.0]])
squareProjDistribution = otexp.PushForwardOverMesh(
    ot.Field(squareProjMesh, squareProjValues)
)
assert squareProjDistribution.getDimension() == 1
assert squareProjDistribution.getIntrinsicDimension() == 1
for x in [0.1, 0.25, 0.5, 0.75, 0.9]:
    ott.assert_almost_equal(
        squareProjDistribution.computePDF([x]), 1.0, 1e-6, 0.0
    )
ott.assert_almost_equal(
    squareProjDistribution.computePDF([-0.5]), 0.0, 0.0, 1e-12
)
ott.assert_almost_equal(
    squareProjDistribution.computePDF([1.5]), 0.0, 0.0, 1e-12
)
# The co-area case also has deterministic moments: the projection of the
# uniform unit square on its first coordinate has mean 1/2 and variance 1/12
ott.assert_almost_equal(squareProjDistribution.getMean(), [0.5], 1e-12, 1e-12)
ott.assert_almost_equal(squareProjDistribution.getCovariance(),
                        ot.CovarianceMatrix(1, [1.0 / 12.0]), 1e-12, 1e-12)

# Intrinsic-preserving embedded map: segment in R^2 (intrinsic 1) projected
# onto its x-coordinate. The weighted piecewise-affine path applies (output
# dimension equals the intrinsic dimension), giving Uniform(0, 2).
segmentProjMesh = ot.Mesh(
    ot.Sample([[0.0, 0.0], [2.0, 0.0]]), ot.IndicesCollection([[0, 1, 1]])
)
assert segmentProjMesh.getDimension() == 2
assert segmentProjMesh.getIntrinsicDimension() == 1
segmentProjDistribution = otexp.PushForwardOverMesh(
    ot.Field(segmentProjMesh, ot.Sample([[0.0], [2.0]]))
)
assert segmentProjDistribution.getDimension() == 1
assert segmentProjDistribution.getIntrinsicDimension() == 1
ott.assert_almost_equal(
    segmentProjDistribution.computePDF([1.0]), 0.5, 1e-10, 0.0
)
ott.assert_almost_equal(
    segmentProjDistribution.computePDF([3.0]), 0.0, 0.0, 1e-12
)

# The image mesh of the co-area case cannot be built, since the image of a
# reference triangle is a collapsed segment rather than a simplex of the same
# dimension: it is reported as not yet implemented
with ott.assert_raises(RuntimeError):
    squareProjDistribution.getMesh()

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
with ott.assert_raises(RuntimeError):
    # co-area with non-uniform antecedent is not implemented yet
    otexp.PushForwardOverMesh(
        ot.Normal(2), ot.Field(squareProjMesh, squareProjValues)
    )
