#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# The 1D interface
dim = 2
a = ot.Point(dim, -1.0)
b = ot.Point(dim, 2.0)
domain = ot.Domain(ot.Interval(a, b))
p1 = ot.Point(dim, 0.5)
p2 = ot.Point(dim, 2.5)
print("Domain=", domain)
# Accessors
print("Dimension=", domain.getDimension())
# Contains
print("is point ", p1, " inside ", domain, "? ", p1 in domain)
print("is point ", p2, " inside ", domain, "? ", p2 in domain)

# Equality tests
assert ot.Interval(a, b) == ot.Interval(a, b), "Interval==Interval"
assert ot.Interval(a, b) != ot.Interval([-2.0, -1.0], b), "Interval!=Interval"
assert ot.Interval(a, b) != ot.DomainComplement(ot.Interval(a, b)), "Interval!=Complement"

# LevelSet
f = ot.SymbolicFunction(["x"], ["x"])
assert ot.LevelSet(f) == ot.LevelSet(f), "LevelSet==LevelSet"
assert ot.LevelSet(f) != ot.LevelSet(f, ot.GreaterOrEqual(), 0.0), "LevelSet!=LevelSet op"
assert ot.LevelSet(f, ot.LessOrEqual(), 1.0) != ot.LevelSet(f, ot.LessOrEqual(), 2.0), "LevelSet!=LevelSet level"

# DomainUnion
d1 = ot.Interval([0.0], [1.0])
d2 = ot.Interval([1.0], [2.0])
assert ot.DomainUnion([d1, d2]) == ot.DomainUnion([d1, d2]), "Union==Union"
assert ot.DomainUnion([d1, d2]) != ot.DomainUnion([d1, d1]), "Union!=Union"

# DomainIntersection
assert ot.DomainIntersection([d1, d2]) == ot.DomainIntersection([d1, d2]), "Intersection==Intersection"
assert ot.DomainIntersection([d1, d2]) != ot.DomainIntersection([d1, d1]), "Intersection!=Intersection"

# DomainDifference
assert ot.DomainDifference(d1, d2) == ot.DomainDifference(d1, d2), "Difference==Difference"
assert ot.DomainDifference(d1, d2) != ot.DomainDifference(d2, d1), "Difference!=Difference"

# DomainComplement
assert ot.DomainComplement(d1) == ot.DomainComplement(d1), "Complement==Complement"
assert ot.DomainComplement(d1) != ot.DomainComplement(d2), "Complement!=Complement"

# DomainDisjunctiveUnion
assert ot.DomainDisjunctiveUnion([d1, d2]) == ot.DomainDisjunctiveUnion([d1, d2]), "DisjunctiveUnion==DisjunctiveUnion"
assert ot.DomainDisjunctiveUnion([d1, d2]) != ot.DomainDisjunctiveUnion([d1, d1]), "DisjunctiveUnion!=DisjunctiveUnion"

# Handle equality
assert ot.Domain(d1) == ot.Domain(d1), "Domain handle =="
assert ot.Domain(d1) != ot.Domain(d2), "Domain handle !="

# Self equality
assert d1 == d1, "self =="
assert not d1 != d1, "self !="

# MeshDomain equality
mesh1 = ot.Mesh(ot.IntervalMesher([5]).build(ot.Interval([0.0], [1.0])))
mesh2 = ot.Mesh(ot.IntervalMesher([5]).build(ot.Interval([0.0], [1.0])))
md1 = ot.MeshDomain(mesh1)
md2 = ot.MeshDomain(mesh2)
# Mesh uses unique indexing for vertices, so these may not be equal
# Just check that they are equal to themselves
assert md1 == md1, "MeshDomain self =="
assert md1 != ot.MeshDomain(), "MeshDomain !="

# Extra MeshDomain coverage: accessors, contains, bounds, save/load (no prints to keep expout stable)

defaultMD = ot.MeshDomain()
assert isinstance(repr(defaultMD), str)
assert isinstance(str(defaultMD), str)
assert md1.getMesh().getVerticesNumber() == mesh1.getVerticesNumber()
assert isinstance(repr(md1), str)
assert isinstance(str(md1), str)
# EnclosingSimplexAlgorithm accessor
algo = md1.getEnclosingSimplexAlgorithm()
md1.setEnclosingSimplexAlgorithm(
    ot.EnclosingSimplexAlgorithm(mesh1.getVertices(), mesh1.getSimplices())
)
assert md1.getEnclosingSimplexAlgorithm().getVertices().getSize() == mesh1.getVerticesNumber()
# contains / bounds / parallel
assert md1.contains([0.5]), "MeshDomain should contain 0.5"
assert not md1.contains([5.0]), "MeshDomain should not contain 5.0"
domainHandle = ot.Domain(md1)
assert ot.Point([0.5]) in domainHandle
assert ot.Point([5.0]) not in domainHandle
ott.assert_almost_equal(md1.getLowerBound(), [0.0], 1e-12, 1e-12)
ott.assert_almost_equal(md1.getUpperBound(), [1.0], 1e-12, 1e-12)
assert md1.isParallel()
# equals with another domain type
assert not (md1 == ot.Interval([0.0], [1.0])), "MeshDomain != Interval"
# save/load round-trip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("meshdomain.xml"))
study.add("md", md1)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("meshdomain.xml"))
loadedStudy.load()
assert loadedStudy.hasObject("md")
loadedMD = ot.MeshDomain()
loadedStudy.fillObject("md", loadedMD)
assert loadedMD == md1

os.remove("meshdomain.xml")
