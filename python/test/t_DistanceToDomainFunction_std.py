#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# distance function from an Interval of dimension 0
singleton = ot.Interval()
assert singleton.computeDistance([]) == 0.0
assert ot.DistanceToDomainFunction(singleton)([]) == ot.Point([0.0])

# distance function from an Interval
box = ot.Interval([1.2, 0.7], [2.0, 1.0])
distance = ot.DistanceToDomainFunction(box)

point1 = ot.Point([1.5, 0.8])
assert box.computeDistance(point1) == 0.0
distance_to_point1 = distance(point1)
assert distance_to_point1 == ot.Point([0.0])

point2 = ot.Point([1.1, 0.8])
ott.assert_almost_equal(box.computeDistance(point2), 0.1, 1e-12, 0.0)
distance_to_point2 = distance(point2)
ott.assert_almost_equal(distance_to_point2, ot.Point([0.1]), 1e-12, 0.0)

sample = ot.Sample([point1, point2])
ott.assert_almost_equal(
    box.computeDistance(sample), ot.Sample([[0.0], [0.1]]), 1e-12, 0.0
)
ott.assert_almost_equal(distance(sample), ot.Sample([[0.0], [0.1]]), 1e-12, 0.0)

# distance function from a DomainUnion of Intervals
interval1 = ot.Interval(-0.5, 0.0)  # [-0.5, 0]
interval2 = ot.Interval(0.7, 1.3)  # [0.7, 1.3]
interval3 = ot.Interval(0.9, 1.0)
interval3.setFiniteUpperBound([False])  # [0.9, +infinity)

small_union = ot.DomainUnion([interval1, interval2])  # [-0.5, 0] U [0.7, 1.3]
# [-0.5, 0] U [0.7, +infinity)
big_union = ot.DomainUnion([small_union, interval3])

sample_dim1 = ot.Sample([[0.1], [1.4]])
ott.assert_almost_equal(
    small_union.computeDistance(sample_dim1), [[0.1]] * 2, 1e-12, 0.0
)
ott.assert_almost_equal(
    big_union.computeDistance(sample_dim1), [[0.1], [0.0]], 1e-12, 0.0
)

distance_to_small_union = ot.DistanceToDomainFunction(small_union)
ott.assert_almost_equal(distance_to_small_union(sample_dim1), [[0.1]] * 2, 1e-12, 0.0)
distance_to_big_union = ot.DistanceToDomainFunction(big_union)
ott.assert_almost_equal(distance_to_big_union(sample_dim1), [[0.1], [0.0]], 1e-12, 0.0)

# extra coverage: default ctor, comparison, string converters
default = ot.DistanceToDomainFunction()
assert default == default
assert distance == distance
assert not (distance != distance)
assert distance == ot.DistanceToDomainFunction(box)
assert distance != distance_to_small_union
assert len(repr(distance)) > 0
assert len(str(distance)) > 0
assert len(repr(default)) > 0
# gradient/hessian are not implemented
with ott.assert_raises(Exception):
    distance.gradient(point1)
with ott.assert_raises(Exception):
    distance.hessian(point1)
# save/load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("distance_to_domain.xml"))
study.add("f", distance)
study.save()
loaded_study = ot.Study()
loaded_study.setStorageManager(ot.XMLStorageManager("distance_to_domain.xml"))
loaded_study.load()
loaded = ot.Function()
loaded_study.fillObject("f", loaded)
assert loaded == distance

os.remove("distance_to_domain.xml")
