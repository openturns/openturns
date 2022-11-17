#! /usr/bin/env python

import openturns as ot

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

domain = ot.DomainDisjunctiveUnion([cube, sphere])
print("cube=", cube)
print("sphere=", sphere)
print("disjunctive union=", domain)
# Accessors
print("Dimension=", domain.getDimension())
# Contains
print("is point ", p0, " inside ? ", domain.contains(p0))
print("is point ", p1, " inside ? ", domain.contains(p1))
print("is point ", p2, " inside ? ", domain.contains(p2))
print("is point ", p3, " inside ? ", domain.contains(p3))
sample = [p0, p1, p2, p3]
print("is sample ", sample, " inside ? ", domain.contains(sample))

# p1 is inside cube & sphere, no need to check sphere2
sphere2 = ot.LevelSet(ot.SymbolicFunction(["x", "y"], ["x/0"]), ot.Less(), 1.0)
domain2 = ot.DomainDisjunctiveUnion([cube, sphere, sphere2])
assert not domain2.contains(p1), "prune sphere"

# computeDistance
interval1 = ot.Interval(-0.4, 0.0)
interval2 = ot.Interval(1.0, 1.3)
interval3 = ot.Interval(1.1, 1.5)
domain = ot.DomainDisjunctiveUnion([interval1, interval2, interval3])
x_dref = {-1.0: 0.6, -0.25: 0.0, 0.875: 0.125, 1.05: 0.0, 1.2: None, 2.0: 0.5}
for x, dref in x_dref.items():
    try:
        d = domain.computeDistance([x])
    except RuntimeError:
        d = None
    print(f"x={x} d={d} ref={dref}")
    if dref is not None:
        assert abs(d - dref) < 1e-8, "distance"
