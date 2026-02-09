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
            ott.assert_almost_equal(x.norm(), distance[indices.find(i)])
