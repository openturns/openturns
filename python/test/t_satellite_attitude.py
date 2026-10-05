#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from openturns.usecases import satellite_attitude

ot.TESTPREAMBLE()

use_case = satellite_attitude.SatelliteAttitude()
data = use_case.rotations
print(data.getSize(), data.getDimension())
assert data.getSize() == 250
assert data.getDimension() == 9

# All the rotations are proper (orthogonal, determinant +1)
for i in range(data.getSize()):
    row = data[i]
    matrix = ot.SquareMatrix([[row[3 * r + c] for c in range(3)] for r in range(3)])
    product = matrix * matrix.transpose()
    for r in range(3):
        for c in range(3):
            ott.assert_almost_equal(product[r, c], 1.0 if r == c else 0.0, 1e-5, 1e-8)
    ott.assert_almost_equal(matrix.computeDeterminant(), 1.0, 1e-10, 0.0)

# MatrixFisher factory recovers the concentrations
estimated = otexp.MatrixFisherFactory().build(data).getImplementation().getF()
reference = use_case.getReferenceF()
for r in range(3):
    ott.assert_almost_equal(estimated[r, r], reference[r, r], 0.0, 1.5)
