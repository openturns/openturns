#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

regularGrid = ot.RegularGrid(-1.0, 0.1, 21)
regularGridNearestNeighbourAlgorithm = ot.RegularGridNearestNeighbour(
    regularGrid)
print('regularGridNearestNeighbourAlgorithm=',
      regularGridNearestNeighbourAlgorithm)

test = ot.Normal().getSample(20)
for i in range(len(test)):
    index = regularGridNearestNeighbourAlgorithm.query(test[i])
    neighbour = ot.Point(1, regularGrid.getValue(
        regularGridNearestNeighbourAlgorithm.query(test[i])))
    print('Nearest neighbour of', test[i], '=', neighbour, 'index=', index)

k = 4
for i in range(len(test)):
    indices = regularGridNearestNeighbourAlgorithm.queryK(test[i], k, True)
    print(k, 'nearest neighbours of', test[i], '(indices=', indices)
