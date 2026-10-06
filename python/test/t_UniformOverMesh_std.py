#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
mesher = ot.LevelSetMesher([25] * 2)
function = ot.SymbolicFunction(["x0", "x1"], ["10*(x0^3+x1)^2+x0^2"])
level = 0.5
domain = ot.LevelSet(function, ot.LessOrEqual(), level)
lower = [-0.75, -0.5]
upper = [0.75, 0.5]
mesh = mesher.build(domain, ot.Interval(lower, upper), False)
distribution = ot.UniformOverMesh(mesh)
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
print("covariance=", oneSample.computeCovariance())

# Define a point
point = [0.1] * 2
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.5g" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.5g" % PDF)
CDF = distribution.computeCDF(point)
print(f"cdf={CDF:.3g}")

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.run()

# 1D mesh: segment sampling branch
distribution1d = ot.UniformOverMesh(ot.RegularGrid(0.0, 1.0, 5))
print("1D realization=", distribution1d.getRealization())
print("1D sample=", distribution1d.getSample(3))
print("1D pdf=%.5g" % distribution1d.computePDF([0.5]))

# 3D mesh: tetrahedron sampling branch
mesher3d = ot.IntervalMesher([2] * 3)
mesh3d = mesher3d.build(ot.Interval([-1.0] * 3, [1.0] * 3))
distribution3d = ot.UniformOverMesh(mesh3d)
print("3D realization=", distribution3d.getRealization())
print("3D sample=", distribution3d.getSample(3))
print("3D pdf=%.5g" % distribution3d.computePDF([0.0] * 3))

# Accessors
print("mesh=", distribution.getMesh().getVerticesNumber(), "vertices")
distribution.setIntegrationAlgorithm(ot.GaussLegendre([5] * 2))
print(
    "integrationAlgorithm=",
    distribution.getIntegrationAlgorithm().getClassName(),
)

# Probability edge cases
print(
    "proba(empty)=%.5g"
    % distribution.computeProbability(ot.Interval([10.0] * 2, [11.0] * 2))
)
print(
    "proba(range)=%.5g" % distribution.computeProbability(distribution.getRange())
)
with ott.assert_raises(TypeError):
    distribution.computeProbability(ot.Interval([0.0], [1.0]))

# Error cases
with ott.assert_raises(TypeError):
    distribution.setParameter([1.0])
with ott.assert_raises(TypeError):
    ot.UniformOverMesh(ot.Mesh())
vertices = ot.Sample([[0.0, 0.0], [1.0, 0.0], [2.0, 0.0]])
simplices = ot.IndicesCollection([[0, 1, 2]])
with ott.assert_raises(TypeError):
    ot.UniformOverMesh(ot.Mesh(vertices, simplices))
