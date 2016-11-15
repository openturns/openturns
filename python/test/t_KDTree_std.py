#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()


sample = ot.Normal(3).getSample(10)
tree = ot.KDTree(sample)
print('tree=', tree)

test = ot.Normal(3).getSample(20)
for i in range(len(test)):
    index = tree.getNearestNeighbourIndex(test[i])
    neighbour = tree.getNearestNeighbour(test[i])
    print('Nearest neighbour of', test[i], '=', neighbour, 'index=', index)
