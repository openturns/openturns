#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

regularGrid = ot.RegularGrid(-1.0, 0.1, 21)
regularGridNearestNeighbour = ot.RegularGridNearestNeighbour(regularGrid)
print('regularGridNearestNeighbour=', regularGridNearestNeighbour)

test = ot.Normal().getSample(20)
for i in range(len(test)):
    index = regularGridNearestNeighbour.getNearestNeighbourIndex(test[i])
    neighbour = regularGridNearestNeighbour.getNearestNeighbour(test[i])
    print('Nearest neighbour of', test[i], '=', neighbour, 'index=', index)

k = 4
for i in range(len(test)):
    indices = regularGridNearestNeighbour.getNearestNeighboursIndices(test[i], k, True)
    print(k, 'nearest neighbours of', test[i], '(indices=', indices)
