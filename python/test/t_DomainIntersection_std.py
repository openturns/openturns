#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.PlatformInfo.SetNumericalPrecision(6)

dim = 2

# First domain: [0,2]x[0,2]
cube = ot.Interval([0.0] * dim, [2.0] * dim)

# Second domain: sphere center=(0,0) r=1
function = ot.SymbolicFunction(["x", "y"], ["x^2 + y^2"])
sphere = ot.LevelSet(function, ot.LessOrEqual(), 1.0)

# Inside sphere but not cube
p0 = [-0.25, 0.25]
# Inside cube and sphere
p1 = [0.25, 0.25]
# Inside cube but not sphere
p2 = [1.8, 1.8]
# Outside
p3 = [4.0, 4.0]

domain = ot.DomainIntersection([cube, sphere])
print("cube=", cube)
print("sphere=", sphere)
print("intersection=", domain)
# Accessors
print("Dimension=", domain.getDimension())
# Contains
print("is point ", p0, " inside ? ", domain.contains(p0))
print("is point ", p1, " inside ? ", domain.contains(p1))
print("is point ", p2, " inside ? ", domain.contains(p2))
print("is point ", p3, " inside ? ", domain.contains(p3))
sample = [p0, p1, p2, p3]
print("is sample ", sample, " inside ? ", domain.contains(sample))

# p0 is outside cube, no need to check sphere2
sphere2 = ot.LevelSet(ot.SymbolicFunction(["x", "y"], ["x/0"]), ot.LessOrEqual(), 1.0)
domain2 = ot.DomainIntersection([cube, sphere2])
assert not domain2.contains(p0), "prune sphere"

# check thread-safety
f1 = ot.PythonFunction(2, 1, lambda X: [X[0]])
f2 = ot.PythonFunction(2, 1, lambda X: [X[1]])
l2 = ot.LevelSet(f2, ot.Greater(), 0.0)
l1 = ot.LevelSet(f1, ot.Greater(), 0.0)
u = ot.DomainIntersection([l1, l2])
x = ot.Normal(2).getSample(1000)
y = u.contains(x)
assert len(y) == 1000

# Extra coverage (no output to preserve expout)
assert ot.DomainIntersection() == ot.DomainIntersection()
assert domain == domain
assert domain == ot.DomainIntersection([cube, sphere])
assert not domain == ot.DomainIntersection([cube])
assert domain != ot.DomainIntersection([cube])
assert not domain != domain
assert len(repr(domain)) > 0
assert len(str(domain)) > 0
# empty intersection contains everything
assert ot.DomainIntersection().contains(p1)
assert list(ot.DomainIntersection().contains([p0, p1])) == [1, 1]
# heterogeneous dimensions are rejected
with ott.assert_raises(Exception):
    ot.DomainIntersection([cube, ot.Interval([0.0], [1.0])])
# isParallel is false when a member is not parallel
assert not u.isParallel()
assert domain.isParallel()
# single-domain intersection behaves like the domain itself
assert ot.DomainIntersection([cube]).contains(p1) == cube.contains(p1)
# save / load roundtrip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("di_test.xml"))
study.add("domain", domain)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager("di_test.xml"))
study2.load()
loaded = ot.DomainIntersection()
study2.fillObject("domain", loaded)
assert loaded == domain

os.remove("di_test.xml")
