#! /usr/bin/env python

import openturns as ot
import os
import openturns.testing as ott

ot.TESTPREAMBLE()

# The function defining the level-set
inVars = ["x", "y"]
formula1 = ["x^4 + y^4"]
function1 = ot.SymbolicFunction(inVars, formula1)
levelSet1 = ot.LevelSet(function1, ot.LessOrEqual(), 1.0)
print("levelSet1=", levelSet1)
p0 = [-0.5, -0.5]
p1 = [0.5, 0.0]
p2 = [1.5, 0.0]
print("levelSet1 contains ", p0, "? ", p0 in levelSet1)
print("levelSet1 contains ", p1, "? ", p1 in levelSet1)
print("levelSet1 contains ", p2, "? ", p2 in levelSet1)
# Intersection
formula2 = ["(x-1)^2 + y^2"]
function2 = ot.SymbolicFunction(inVars, formula2)
levelSet2 = ot.LevelSet(function2, ot.LessOrEqual(), 1.0)
print("levelSet2=", levelSet2)
print("levelSet2 contains ", p0, "? ", levelSet2.contains(p0))
print("levelSet2 contains ", p1, "? ", levelSet2.contains(p1))
print("levelSet2 contains ", p2, "? ", levelSet2.contains(p2))
intersection = levelSet1.intersect(levelSet2)
print("intersection of ", levelSet1, " and ", levelSet2, " equals ", intersection)
print("intersection contains ", p0, "? ", intersection.contains(p0))
print("intersection contains ", p1, "? ", intersection.contains(p1))
print("intersection contains ", p2, "? ", intersection.contains(p2))
# Union
join = levelSet1.join(levelSet2)
print("join of ", levelSet1, " and ", levelSet2, " equals ", join)
print("join contains ", p0, "? ", join.contains(p0))
print("join contains ", p1, "? ", join.contains(p1))
print("join contains ", p2, "? ", join.contains(p2))

# check that f2 is not called when not needed
f1 = ot.SymbolicFunction(["x"], ["x"])
f2 = ot.MemoizeFunction(ot.SymbolicFunction(["x"], ["x/0"]))
f2.enableCache()
ls1 = ot.LevelSet(f1, ot.Less(), 0.0)
ls2 = ot.LevelSet(f2, ot.Less(), 0.0)
union = ls1.join(ls2)
intersection = ls1.intersect(ls2)
assert [-1.0] in union, "prune union"
assert f2.getCallsNumber() == 0, "union not pruned"
assert [1.0] not in intersection, "prune intersection"
assert f2.getCallsNumber() == 0, "intersection not pruned"

# save/load
if ot.PlatformInfo.HasFeature("libxml2"):
    fname = "levelset.xml"
    study = ot.Study(fname)
    study.add("intersection", intersection)
    study.save()
    study = ot.Study(fname)
    study.load()
    intersection = ot.LevelSet()
    study.fillObject("intersection", intersection)
    os.remove(fname)
    assert [1.0] not in intersection, "after save/load"

# check intersection/union at border
for op in ["Less", "Greater"]:
    f1 = ot.SymbolicFunction(["x"], ["x"])
    f2 = ot.SymbolicFunction(["x"], ["x"])
    ls1 = ot.LevelSet(f1, eval(f"ot.{op}()"), 0.0)
    ls2 = ot.LevelSet(f2, eval(f"ot.{op}()"), 0.0)
    u1 = ls1.join(ls2)
    i1 = ls1.intersect(ls2)
    assert [0.0] not in i1, "!i1"
    assert [0.0] not in u1, "!u1"
    ls1 = ot.LevelSet(f1, eval(f"ot.{op}OrEqual()"), 0.0)
    ls2 = ot.LevelSet(f2, eval(f"ot.{op}OrEqual()"), 0.0)
    u2 = ls1.join(ls2)
    i2 = ls1.intersect(ls2)
    assert [0.0] in i2, "i2"
    assert [0.0] in u2, "u2"

# check merge works
f1 = ot.SymbolicFunction(["x"], ["x"])
f2 = ot.SymbolicFunction(["x"], ["x+1"])
f3 = ot.SymbolicFunction(["x"], ["x+2"])
ls1 = ot.LevelSet(f1, ot.Less(), 0.0)
ls2 = ot.LevelSet(f2, ot.Less(), 0.0)
ls3 = ot.LevelSet(f3, ot.Less(), 0.0)
union = ls1.join(ls2).join(ls3)
intersection = ls1.intersect(ls2).intersect(ls3)
print("union x3", union)
print("intersection 3x", intersection)
x = [-3.0]
assert x in union, "union 3x"
assert x in intersection, "intersection 3x"
x = [3.0]
assert x not in union, "!union 3x"
assert x not in intersection, "!intersection 3x"

# Extra coverage (no output to preserve expout)
default_ls = ot.LevelSet()
assert default_ls == ot.LevelSet()
assert levelSet1 == levelSet1
assert not levelSet1 == levelSet2
assert not levelSet1 != levelSet1
assert len(repr(levelSet1)) > 0
assert len(str(levelSet1)) > 0
# constructor with wrong output dimension
with ott.assert_raises(Exception):
    ot.LevelSet(ot.SymbolicFunction(["x"], ["x", "x"]), ot.Less(), 0.0)
# accessors and their error paths
ls_acc = ot.LevelSet(
    ot.SymbolicFunction(["x", "y"], ["x + y"]), ot.LessOrEqual(), 1.0
)
assert ls_acc.getOperator() == ot.LessOrEqual()
ls_acc.setOperator(ot.Greater())
assert ls_acc.getOperator() == ot.Greater()
assert ls_acc.getLevel() == 1.0
ls_acc.setLevel(2.0)
assert ls_acc.getLevel() == 2.0
assert ls_acc.getFunction().getInputDimension() == 2
with ott.assert_raises(Exception):
    ls_acc.setFunction(ot.SymbolicFunction(["x", "y", "z"], ["x"]))
ls_acc.setFunction(ot.SymbolicFunction(["x", "y"], ["x - y"]))
with ott.assert_raises(Exception):
    ls_acc.setLowerBound([0.0])
with ott.assert_raises(Exception):
    ls_acc.setUpperBound([0.0, 0.0, 0.0])
ls_acc.setLowerBound([-2.0, -2.0])
ls_acc.setUpperBound([2.0, 2.0])
ott.assert_almost_equal(ls_acc.getLowerBound(), [-2.0, -2.0])
ott.assert_almost_equal(ls_acc.getUpperBound(), [2.0, 2.0])
# contains with wrong dimensions
with ott.assert_raises(Exception):
    ls_acc.contains([1.0])
with ott.assert_raises(Exception):
    ls_acc.contains(ot.Sample(3, 3))
# contains(Sample) with bounding box (pruning path)
inside = ls_acc.contains(ot.Sample([[-3.0, -3.0], [1.5, -1.0], [0.0, 0.0]]))
assert list(inside) == [False, True, False]
# contains(Sample) without bounding box
ls_nobb = ot.LevelSet(
    ot.SymbolicFunction(["x", "y"], ["x^4 + y^4"]), ot.LessOrEqual(), 1.0
)
assert list(ls_nobb.contains(ot.Sample([[-0.5, -0.5], [1.5, 0.0]]))) == [True, False]
# non-parallel function exercises the sequential Sample path
ls_py = ot.LevelSet(
    ot.PythonFunction(2, 1, lambda X: [X[0] + X[1]]), ot.LessOrEqual(), 1.0
)
assert not ls_py.isParallel()
assert list(ls_py.contains(ot.Sample([[0.0, 0.0], [2.0, 2.0]]))) == [True, False]
assert ls_py.isParallel() == ls_py.getFunction().getImplementation().isParallel()
# self-intersection returns self
assert ls_acc.intersect(ls_acc) == ls_acc
# incompatible dimensions
with ott.assert_raises(Exception):
    ls_acc.intersect(ot.LevelSet(ot.SymbolicFunction(["x"], ["x"]), ot.Less(), 0.0))
