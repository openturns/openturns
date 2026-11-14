#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# The nonlinear co-area densities with multidimensional fibres are checked
# here, separately from t_PushForwardDistribution_std.py to keep each
# coverage test short.
continuationKey = "PushForwardDistribution-NonlinearCoAreaContinuation"
chiSquare3 = ot.ChiSquare(3)

# A level surface that coincides with a mesh plane exercises the face
# deduplication: Y = X0^2 on R^3 has two flat branes x0 = +/- sqrt(y), and its
# density is the chi-square distribution with one degree of freedom.
surfaceDiscretizationKey = "PushForwardDistribution-NonlinearCoAreaSurfaceDiscretization"
previousSurfaceDiscretization = ot.ResourceMap.GetAsUnsignedInteger(
    surfaceDiscretizationKey
)
# A discretization of 20 keeps a validated 6x margin on the 2e-3 assertions
# below at half the extraction cost
ot.ResourceMap.SetAsUnsignedInteger(surfaceDiscretizationKey, 20)
planeCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2"]), ot.Normal(3)
)
for level in [0.7, 1.0]:
    expected = ot.Normal().computePDF([math.sqrt(level)]) / math.sqrt(level)
    pdf = planeCoArea.computePDF([level])
    print(f"planeCoArea pdf({level})={pdf:.12g}")
    ott.assert_almost_equal(pdf, expected, 2e-3, 0.0)
ot.ResourceMap.SetAsUnsignedInteger(
    surfaceDiscretizationKey, previousSurfaceDiscretization
)

# A level surface whose triangles are shared by many tetrahedra exercises the
# global vertex numbering used by the face deduplication.  The level x2 = 0 of
# f(x) = x2 exp(x0) is a mesh plane, and its density is
# phi(0) E[exp(-X0)] = phi(0) exp(1 / 2).
ot.ResourceMap.SetAsUnsignedInteger(surfaceDiscretizationKey, 16)
faceDedup = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x2 * exp(x0)"]), ot.Normal(3)
)
expectedFaceDedup = ot.Normal().computePDF(0.0) * math.exp(0.5)
pdf = faceDedup.computePDF([0.0])
print(f"faceDedup pdf(0)={pdf:.12g}")
ott.assert_almost_equal(pdf, expectedFaceDedup, 1e-6, 0.0)
ot.ResourceMap.SetAsUnsignedInteger(
    surfaceDiscretizationKey, previousSurfaceDiscretization
)

# A scalar nonlinear map on R^3 without an analytical gradient uses the
# finite difference fallback for the co-area factor.
pythonSurface = otexp.PushForwardDistribution(
    ot.PythonFunction(3, 1, lambda x: [x[0] ** 2 + x[1] ** 2 + x[2] ** 2]),
    ot.Normal(3),
)
ott.assert_almost_equal(pythonSurface.computePDF([1.0]),
                        chiSquare3.computePDF([1.0]), 2e-2, 0.0)

# A nonlinear map from R^3 to R^2 has a one dimensional fibre, the
# intersection of the two level surfaces. For the uniform cube and
# Y = (X0^2 + X1^2, X2), the fibre at (r^2, y1) with 0 < r < 1 is the
# quarter circle of radius r in the positive quadrant. Its length is
# pi * r / 2, the two dimensional Jacobian is 2 * r, and the antecedent
# density is one, so the density is pi / 4.
uniformCube = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 3)
ot.ResourceMap.SetAsUnsignedInteger(surfaceDiscretizationKey, 24)
curveCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2 + x1^2", "x2"]),
    uniformCube,
)
for point in [[0.5, 0.5], [0.2, 0.3]]:
    pdf = curveCoArea.computePDF(point)
    print(f"curveCoArea pdf({point})={pdf:.12g}")
    ott.assert_almost_equal(pdf, math.pi / 4.0, 1e-3, 0.0)
# Same single-point comparison as below: the second point is already
# covered in normal mode above
ot.ResourceMap.SetAsBool(continuationKey, False)
curveMeshPDF = curveCoArea.computePDF([0.5, 0.5])
ot.ResourceMap.SetAsBool(continuationKey, True)
continuationPDF = curveCoArea.computePDF([0.5, 0.5])
print(f"curveCoArea continuation pdf([0.5, 0.5])={continuationPDF:.12g}")
ott.assert_almost_equal(continuationPDF, math.pi / 4.0, 1e-5, 0.0)
assert abs(continuationPDF - math.pi / 4.0) < abs(curveMeshPDF - math.pi / 4.0)

# Points outside the image of the two components have an empty fibre.
assert curveCoArea.computePDF([-0.1, 0.5]) == 0.0
assert curveCoArea.computePDF([0.5, 1.5]) == 0.0

# With independent standard normal inputs, the same map has the closed-form
# density 0.5 * exp(-y0 / 2) * phi(y1), which also exercises the finite
# search box and the non-constant antecedent density.
normalCurve = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2 + x1^2", "x2"]),
    ot.Normal(3),
)
curvePoint = [0.7, 0.4]
expectedCurvePDF = (0.5 * math.exp(-0.5 * curvePoint[0])
                    * ot.Normal().computePDF([curvePoint[1]]))
pdf = normalCurve.computePDF(curvePoint)
print(f"normalCurve pdf({curvePoint})={pdf:.12g}")
ott.assert_almost_equal(pdf, expectedCurvePDF, 1e-2, 0.0)
# Its deterministic moments are checked in
# t_PushForwardDistribution_curve_moments.py: the input-space quadrature is
# too expensive to share this test under coverage instrumentation.
ot.ResourceMap.SetAsUnsignedInteger(
    surfaceDiscretizationKey, previousSurfaceDiscretization
)

# A nonlinear map from R^4 to R^3 has a one dimensional fibre as well. For
# the uniform cube and Y = (X0^2 + X1^2, X2, X3), the fibre at (r^2, y1, y2)
# is a quarter circle of radius r, so the density is again pi / 4.
volumeKey = "PushForwardDistribution-NonlinearCoAreaVolumeDiscretization"
previousVolumeDiscretization = ot.ResourceMap.GetAsUnsignedInteger(volumeKey)
# A volume discretization of 8 is also enough for the normal R^4 densities
# below (validated 30x margin on their 1e-4 assertions)
ot.ResourceMap.SetAsUnsignedInteger(volumeKey, 8)
uniformCube4 = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 4)
curve4CoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(
        ["x0", "x1", "x2", "x3"],
        ["x0^2 + x1^2", "x2", "x3"],
    ),
    uniformCube4,
)
curve4Points = [[0.5, 0.5, 0.5], [0.2, 0.3, 0.4]]
for point in curve4Points:
    pdf = curve4CoArea.computePDF(point)
    print(f"curve4CoArea pdf({point})={pdf:.12g}")
    ott.assert_almost_equal(pdf, math.pi / 4.0, 1e-5, 0.0)
# The mesh/continuation comparison on a single point is enough: the second
# point is already covered in normal mode above
ot.ResourceMap.SetAsBool(continuationKey, False)
curve4MeshPDF = curve4CoArea.computePDF(curve4Points[0])
ot.ResourceMap.SetAsBool(continuationKey, True)
continuationPDF = curve4CoArea.computePDF(curve4Points[0])
print(f"curve4CoArea continuation pdf({curve4Points[0]})={continuationPDF:.12g}")
ott.assert_almost_equal(continuationPDF, math.pi / 4.0, 1e-5, 0.0)
assert abs(continuationPDF - math.pi / 4.0) < abs(curve4MeshPDF - math.pi / 4.0)

# An empty R^4 fibre has a zero density.
assert curve4CoArea.computePDF([-0.1, 0.5, 0.5]) == 0.0
assert curve4CoArea.computePDF([0.5, 1.5, 0.5]) == 0.0
assert curve4CoArea.computePDF([0.5, 0.5, 1.5]) == 0.0

# The volume discretization of 8 is kept for the normal R^4 tests below.

# With independent standard normal inputs, the R^4 -> R^3 density is
# 0.5 * exp(-y0 / 2) * phi(y1) * phi(y2), which exercises the finite search
# box and the non-constant antecedent density in dimension four.
normalCurve4 = otexp.PushForwardDistribution(
    ot.SymbolicFunction(
        ["x0", "x1", "x2", "x3"],
        ["x0^2 + x1^2", "x2", "x3"],
    ),
    ot.Normal(4),
)
curve4Point = [0.7, 0.4, 0.5]
expectedCurve4PDF = (
    0.5
    * math.exp(-0.5 * curve4Point[0])
    * ot.Normal().computePDF([curve4Point[1]])
    * ot.Normal().computePDF([curve4Point[2]])
)
pdf = normalCurve4.computePDF(curve4Point)
print(f"normalCurve4 pdf({curve4Point})={pdf:.12g}")
ott.assert_almost_equal(pdf, expectedCurve4PDF, 1e-4, 0.0)
# A fibre whose radius is smaller than the mesh step exercises the closure of
# a short loop: it must be traced once, not twice.
tinyCurve4Point = [0.02, 0.0, 0.0]
expectedTinyCurve4PDF = (
    0.5
    * math.exp(-0.5 * tinyCurve4Point[0])
    * ot.Normal().computePDF([tinyCurve4Point[1]])
    * ot.Normal().computePDF([tinyCurve4Point[2]])
)
pdf = normalCurve4.computePDF(tinyCurve4Point)
print(f"normalCurve4 pdf({tinyCurve4Point})={pdf:.12g}")
ott.assert_almost_equal(pdf, expectedTinyCurve4PDF, 1e-4, 0.0)
ot.ResourceMap.SetAsUnsignedInteger(volumeKey, previousVolumeDiscretization)

# A nonlinear R^3 -> R^2 map without an analytical gradient uses the finite
# difference fallback for the two dimensional Jacobian.
pythonCurve = otexp.PushForwardDistribution(
    ot.PythonFunction(3, 2, lambda x: [x[0] ** 2 + x[1] ** 2, x[2]]),
    uniformCube,
)
ott.assert_almost_equal(pythonCurve.computePDF([0.5, 0.4]),
                        math.pi / 4.0, 2e-3, 0.0)

# Nonlinear co-area maps with a higher dimensional fibre are still accepted
# at construction, and their PDF fails with an explicit message.
nonAffineFunctions = [
    (ot.SymbolicFunction(["x0", "x1", "x2", "x3"], ["x0 + x1^2"]),
     ot.Normal(4)),
    (ot.SymbolicFunction(["x0", "x1", "x2", "x3"], ["x0^2", "x1"]),
     ot.Normal(4)),
]
for nonAffineFunction, nonAffineAntecedent in nonAffineFunctions:
    nonAffineDistribution = otexp.PushForwardDistribution(
        nonAffineFunction, nonAffineAntecedent
    )
    outputDimension = nonAffineFunction.getOutputDimension()
    assert nonAffineDistribution.getDimension() == outputDimension
    assert nonAffineDistribution.getSample(3).getSize() == 3
    with ott.assert_raises(RuntimeError):
        nonAffineDistribution.computePDF(ot.Point(outputDimension))

# Above the configured dimension, the deterministic iterated quadrature is
# replaced by a Monte-Carlo estimate.  The map X0^2 + X1 with two standard
# normals has mean 1 and variance 3.
momentsDimensionKey = "PushForwardDistribution-MomentsMaximumDimension"
ot.ResourceMap.SetAsUnsignedInteger(momentsDimensionKey, 1)
sampledMoments = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1"], ["x0^2 + x1"]), ot.Normal(2)
)
ott.assert_almost_equal(sampledMoments.getMean(), [1.0], 0.0, 0.2)
ott.assert_almost_equal(
    sampledMoments.getCovariance()[0, 0], 3.0, 0.0, 0.4
)
ot.ResourceMap.Reset()
