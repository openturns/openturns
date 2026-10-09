#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

ot.ResourceMap.SetAsUnsignedInteger(
    "OptimizationAlgorithm-DefaultMaximumIterationNumber", 1000
)
ot.ResourceMap.SetAsUnsignedInteger(
    "OptimizationAlgorithm-DefaultMaximumCallsNumber", 100000
)
ot.ResourceMap.SetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1.0e-7)
ot.ResourceMap.SetAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError", 1.0e-7)
ot.ResourceMap.SetAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError", 1.0e-7)
ot.ResourceMap.SetAsScalar(
    "OptimizationAlgorithm-DefaultMaximumConstraintError", 1.0e-7
)
ot.PlatformInfo.SetNumericalPrecision(2)

# The 1D mesher
mesher1D = ot.LevelSetMesher([7])
print("mesher1D=", mesher1D)

level = 0.5
function1D = ot.SymbolicFunction("x", "cos(x)/(1+0.1*x^2)")
levelSet1D = ot.LevelSet(function1D, ot.LessOrEqual(), level)

# Manual bounding box
mesh1D = mesher1D.build(levelSet1D, ot.Interval(-10.0, 10.0))
print("mesh1D=", mesh1D)

# The 2D mesher
mesher2D = ot.LevelSetMesher([5] * 2)
print("mesher2D=", mesher2D)

function2D = ot.SymbolicFunction(
    ["x0", "x1"], ["cos(x0 * x1)/(1 + 0.1 * (x0^2 + x1^2))"]
)
levelSet2D = ot.LevelSet(function2D, ot.LessOrEqual(), level)

# Manual bounding box, linear interpolation
mesh2D = mesher2D.build(levelSet2D, ot.Interval([-10.0] * 2, [10.0] * 2), False)
print("mesh2D=", mesh2D)

# Manual bounding box, solve the equation projection
ot.ResourceMap.SetAsBool("LevelSetMesher-SolveEquation", True)
mesh2D = mesher2D.build(levelSet2D, ot.Interval([-10.0] * 2, [10.0] * 2), True)
print("mesh2D=", mesh2D)

# Manual bounding box, optimization projection
ot.ResourceMap.SetAsBool("LevelSetMesher-SolveEquation", False)
mesh2D = mesher2D.build(levelSet2D, ot.Interval([-10.0] * 2, [10.0] * 2), True)
print("mesh2D=", mesh2D)

# The 3D mesher
mesher3D = ot.LevelSetMesher([3] * 3)
print("mesher3D=", mesher3D)

function3D = ot.SymbolicFunction(
    ["x0", "x1", "x2"], ["cos(x0 * x1 + x2)/(1 + 0.1*(x0^2 + x1^2 + x2^2))"]
)
levelSet3D = ot.LevelSet(function3D, ot.LessOrEqual(), level)

# Manual bounding box
mesh3D = mesher3D.build(levelSet3D, ot.Interval([-10.0] * 3, [10.0] * 3))
print("mesh3D=", mesh3D)

# The 4D mesher
mesher4D = ot.LevelSetMesher([3] * 4)
print("mesher4D=", mesher4D)

function4D = ot.SymbolicFunction(
    ["x0", "x1", "x2", "x3"], ["sqrt(x0^2+x1^2+x2^2+x3^2)"]
)
levelSet4D = ot.LevelSet(function4D, ot.LessOrEqual(), level)

# Manual bounding box
mesh4D = mesher4D.build(levelSet4D, ot.Interval([-0.5] * 4, [0.5] * 4), False)
print("mesh4D=", mesh4D)

# Issue #1668
f = ot.SymbolicFunction(["x", "y"], ["x^2+y^2"])
levelset = ot.LevelSet(f, ot.Less(), 1.0)
mesh = ot.LevelSetMesher([16] * 2).build(levelset, ot.Interval([-1.5] * 2, [1.5] * 2))
gLess = mesh.draw()
f = ot.SymbolicFunction(["x", "y"], ["-(x^2+y^2)"])
levelset = ot.LevelSet(f, ot.Greater(), -1.0)
mesh = ot.LevelSetMesher([16] * 2).build(levelset, ot.Interval([-1.5] * 2, [1.5] * 2))
gGreater = mesh.draw()
ott.assert_almost_equal(
    gLess.getDrawable(0).getData(), gGreater.getDrawable(0).getData(), 1e-4, 1e-4
)

# Build based on field
f = ot.SymbolicFunction(["x", "y"], ["(x-0.5)^3+y^4"])
levelSet = ot.LevelSet(f, ot.LessOrEqual(), 0.0)

# First case, no values in the field. They will be computed in LevelSetMesher
mesh1 = ot.LevelSetMesher([16] * 2).build(levelSet, ot.Field(mesh, 0))

# Second case, values are provided.
values = f(mesh.getVertices())
mesh2 = ot.LevelSetMesher([16] * 2).build(levelSet, ot.Field(mesh, values))

# Check that the two meshes are identical
ott.assert_almost_equal(mesh1.getVertices(), mesh2.getVertices(), 1e-4, 1e-4)

# QEF sharp-edge recovery: 2D box, corner inside a cell (discretization 6)
boxFunction2D = ot.PythonFunction(2, 1, lambda X: [max(abs(X[0]), abs(X[1])) - 0.5])
boxLevelSet2D = ot.LevelSet(boxFunction2D, ot.LessOrEqual(), 0.0)
boxBB2D = ot.Interval([-1.0] * 2, [1.0] * 2)
boxMesher2D = ot.LevelSetMesher([6] * 2)
assert not boxMesher2D.getUseQEF(), "QEF must be off by default"
# equation projection required: QEF aims rays at features, Brent lands on them
ot.ResourceMap.SetAsBool("LevelSetMesher-SolveEquation", True)
boxLegacy2D = boxMesher2D.build(boxLevelSet2D, boxBB2D)
boxMesher2D.setUseQEF(True)
assert boxMesher2D.getUseQEF(), "accessor round-trip failed"
boxQEF2D = boxMesher2D.build(boxLevelSet2D, boxBB2D)
boxMesher2D.setUseQEF(False)
assert not boxMesher2D.getUseQEF(), "accessor round-trip failed"
ott.assert_almost_equal(boxQEF2D.getVolume(), 1.0, 1e-9, 1e-9)
assert boxQEF2D.getVolume() >= boxLegacy2D.getVolume(), "QEF must not lose volume here"
cornerDistance = min((v - [0.5, 0.5]).norm() for v in boxQEF2D.getVertices())
assert cornerDistance < 1e-6, "QEF must recover the box corner"

# Collection of level sets = intersection: lens of two unit disks
disk1 = ot.SymbolicFunction(["x", "y"], ["(x+0.5)^2+y^2"])
disk2 = ot.SymbolicFunction(["x", "y"], ["(x-0.5)^2+y^2"])
lens = [ot.LevelSet(disk1, ot.LessOrEqual(), 1.0), ot.LevelSet(disk2, ot.LessOrEqual(), 1.0)]
lensBB = ot.Interval([-1.6] * 2, [1.6] * 2)
lensMesher = ot.LevelSetMesher([16] * 2)
lensLegacy = lensMesher.build(lens, lensBB)
lensMesher.setUseQEF(True)
lensQEF = lensMesher.build(lens, lensBB)
lensMesher.setUseQEF(False)
lensExact = 2.0 * m.pi / 3.0 - m.sqrt(3.0) / 2.0
assert abs(lensQEF.getVolume() - lensExact) <= abs(lensLegacy.getVolume() - lensExact), "QEF must be at least as accurate"
ott.assert_almost_equal(lensQEF.getVolume(), lensExact, 1e-2, 1e-2)
tipDistance = min((v - [0.0, m.sqrt(3.0) / 2.0]).norm() for v in lensQEF.getVertices())
assert tipDistance < 1e-4, "lens tip must be recovered"

# Single level set vs 1-element collection parity
singleMesh = ot.LevelSetMesher([16] * 2).build(
    ot.LevelSet(ot.SymbolicFunction(["x", "y"], ["x^2+y^2"]), ot.LessOrEqual(), 1.0),
    ot.Interval([-1.5] * 2, [1.5] * 2),
)
circleCollection = [ot.LevelSet(ot.SymbolicFunction(["x", "y"], ["x^2+y^2"]), ot.LessOrEqual(), 1.0)]
collectionMesh = ot.LevelSetMesher([16] * 2).build(circleCollection, ot.Interval([-1.5] * 2, [1.5] * 2))
assert collectionMesh == singleMesh, "1-element collection must match single build"

# Collection built from a field with one column per level set
supportMesh = ot.IntervalMesher([16] * 2).build(lensBB)
supportVertices = supportMesh.getVertices()
values1 = disk1(supportVertices).asPoint()
values2 = disk2(supportMesh.getVertices()).asPoint()
stacked = ot.Sample(supportVertices.getSize(), 2)
for i in range(supportVertices.getSize()):
    stacked[i, 0] = values1[i]
    stacked[i, 1] = values2[i]
lensMesher.setUseQEF(True)
lensFieldMesh = lensMesher.build(lens, ot.Field(supportMesh, stacked))
lensMesher.setUseQEF(False)
ott.assert_almost_equal(lensFieldMesh.getVertices(), lensQEF.getVertices(), 1e-4, 1e-4)

# default value read in ResourceMap: on enables QEF at construction
ot.ResourceMap.SetAsBool("LevelSetMesher-UseQEF", True)
defaultOnMesher = ot.LevelSetMesher([6] * 2)
assert defaultOnMesher.getUseQEF(), "default must follow the ResourceMap key"
defaultOnMesh = defaultOnMesher.build(boxLevelSet2D, boxBB2D)
ott.assert_almost_equal(defaultOnMesh.getVolume(), 1.0, 1e-9, 1e-9)
ot.ResourceMap.SetAsBool("LevelSetMesher-UseQEF", False)
assert not ot.LevelSetMesher([6] * 2).getUseQEF(), "default must follow the ResourceMap key"

# Empty collection and dimension mismatch raise
with ott.assert_raises(Exception):
    ot.LevelSetMesher([4] * 2).build([], lensBB)
with ott.assert_raises(Exception):
    badLevelSet = ot.LevelSet(ot.SymbolicFunction(["x", "y", "z"], ["x^2+y^2+z^2"]), ot.LessOrEqual(), 1.0)
    ot.LevelSetMesher([4] * 2).build([lens[0], badLevelSet], lensBB)

ot.ResourceMap.SetAsBool("LevelSetMesher-SolveEquation", True)
