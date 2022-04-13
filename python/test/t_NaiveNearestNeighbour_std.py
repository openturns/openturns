#! /usr/bin/env python

import openturns as ot
import numpy as np
import os

ot.TESTPREAMBLE()


sample = ot.Normal(3).getSample(1000)
tree = ot.NaiveNearestNeighbour(sample)
print('tree=', tree)

test = ot.Normal(3).getSample(100)

sample_np = np.array(sample)
test_np = np.array(test)


def nearest_debug(x):
    global sample_np
    a = np.sum(np.square(sample_np - x), axis=1)
    return int(np.argmin(a))


def nearest_debug_indices(x):
    global sample_np
    a = np.sum(np.square(sample_np - x), axis=1)
    return a.argsort()


neighbourIndices = tree.query(test)

neighbourIndices_np = [nearest_debug(x) for x in test]

if neighbourIndices != neighbourIndices_np:
    print("Errors found in query")
    os.exit(1)

for x in test:
    if np.any(nearest_debug_indices(x)[:10] != tree.queryK(x, 10, True)):
        print("Errors found in queryK")
        os.exit(1)
