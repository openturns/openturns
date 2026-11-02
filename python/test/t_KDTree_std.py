#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import numpy as np
import os

ot.TESTPREAMBLE()


sample = ot.Normal(3).getSample(1000)
tree = ot.KDTree(sample)
print("tree=", tree)

test = ot.Normal(3).getSample(100)

sample_np = np.array(sample)
test_np = np.array(test)


def nearest_debug(x):
    a = np.sum(np.square(sample_np - x), axis=1)
    return int(np.argmin(a))


def nearest_debug_indices(x):
    a = np.sum(np.square(sample_np - x), axis=1)
    return a.argsort()


neighbourIndices = tree.query(test)

neighbourIndices_np = [nearest_debug(x) for x in test]

assert neighbourIndices == neighbourIndices_np, "Errors found in query"
for x in test:
    assert np.any(
        nearest_debug_indices(x)[:10] == tree.queryK(x, 10, True)
    ), "Errors found in queryK"

if ot.PlatformInfo.HasFeature("nanoflann"):
    radius = 0.3
    indices, distance = tree.queryRadius([0.0] * 3, radius, True)
    print(indices, distance)
    assert len(indices) == len(distance)
    for i, x in enumerate(sample):
        assert (i in indices) == (x.norm() < radius), f"{x.norm()}"
        if i in indices:
            ott.assert_almost_equal(x.norm(), distance[indices.index(i)])

# limit case
sample = [
    [0.0, -1.0 / 3.0],
    [-0.5, -1.0 / 3.0],
    [0.5, -0.77777777777],
    [-0.75, -0.111111111],
    [0.25, 0.55555555555],
    [-0.25, -0.55555555555],
]
tree = ot.KDTree(sample)
x = sample[1]
indices = tree.queryK(x, 6, True)
print(indices)
assert sorted(indices) == [0, 1, 2, 3, 4, 5], "queryK(6) must return all the indices"
if not ot.PlatformInfo.HasFeature("nanoflann"):
    # deterministic order of the built-in kd-tree, see the tie-break fix
    assert indices == [1, 5, 3, 0, 2, 4]

# space-filling ordering
order = tree.getOrdering()
assert order.getSize() == tree.getSample().getSize()
sorted_order = sorted(order)
assert sorted_order == list(range(order.getSize())), "not a permutation"
# inverse is identity
inverse = [0] * order.getSize()
for i, j in enumerate(order):
    inverse[j] = i
assert [order[inverse[j]] for j in range(order.getSize())] == list(
    range(order.getSize())
), "inverse not a valid round-trip"
# deterministic
tree2 = ot.KDTree(tree.getSample())
assert tree2.getOrdering() == order
# single point
tree3 = ot.KDTree([[1.0, 2.0]])
assert tree3.getOrdering() == [0]
print("order=", order)

# extra coverage: ctors, accessors, error branches, persistence
empty = ot.KDTree()
assert empty.getSample().getSize() == 0
assert len(repr(empty)) > 0
assert len(str(empty)) > 0
with ott.assert_raises(Exception):
    empty.query([0.0, 0.0])
assert len(repr(tree)) > 0
assert len(str(tree)) > 0
ott.assert_almost_equal(tree.getSample(), sample, 0, 0)
# setSample with the same sample is a no-op
tree.setSample(tree.getSample())
ott.assert_almost_equal(tree.getSample(), sample, 0, 0)
# single-point tree answers 0 without searching
single = ot.KDTree([[1.0, 2.0]])
assert single.query([0.0, 0.0]) == 0
assert single.queryK([0.0, 0.0], 1) == [0]
# full-size unsorted query returns all indices
all_indices = tree.queryK(x, 6)
assert sorted(all_indices) == [0, 1, 2, 3, 4, 5]
# too many neighbours
with ott.assert_raises(Exception):
    tree.queryK(x, 7)
# unsorted radius query is consistent with the sorted one (needs nanoflann)
if ot.PlatformInfo.HasFeature("nanoflann"):
    idx_sorted, dist_sorted = tree.queryRadius(x, 1.0, True)
    idx_unsorted, dist_unsorted = tree.queryRadius(x, 1.0, False)
    assert sorted(idx_unsorted) == sorted(idx_sorted)
    assert len(idx_unsorted) == len(dist_unsorted)
    pairs_sorted = sorted(zip(idx_sorted, dist_sorted))
    pairs_unsorted = sorted(zip(idx_unsorted, dist_unsorted))
    for (i1, d1), (i2, d2) in zip(pairs_sorted, pairs_unsorted):
        assert i1 == i2
        ott.assert_almost_equal(d1, d2)
# save/load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("kdtree.xml"))
study.add("tree", tree)
study.save()
loaded_study = ot.Study()
loaded_study.setStorageManager(ot.XMLStorageManager("kdtree.xml"))
loaded_study.load()
loaded_tree = ot.KDTree()
loaded_study.fillObject("tree", loaded_tree)
ott.assert_almost_equal(loaded_tree.getSample(), sample, 0, 0)
assert loaded_tree.query(x) == tree.query(x)

os.remove("kdtree.xml")
