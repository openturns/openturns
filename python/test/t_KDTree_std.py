#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import numpy as np

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
