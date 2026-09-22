#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
from openturns.usecases import brain_diffusion

ot.TESTPREAMBLE()

use_case = brain_diffusion.BrainDiffusion()
data = use_case.tensors
print(data.getSize(), data.getDimension())
assert data.getSize() == 400
assert data.getDimension() == 6

# All the tensors are symmetric positive definite
for i in range(data.getSize()):
    row = data[i]
    matrix = ot.SymmetricMatrix([[row[0], row[1], row[2]], [row[1], row[3], row[4]], [row[2], row[4], row[5]]])
    copy = ot.SymmetricMatrix(matrix)
    eigen = copy.computeEV()[0]
    for value in eigen:
        assert value > 0.0

# RiemannianGaussian factory recovers the Frechet mean and the unit covariance
estimated = otexp.RiemannianGaussianFactory().build(data).getImplementation()
ott.assert_almost_equal(estimated.getMean(), use_case.getReferenceMean(), 0.0, 0.1)
sigma = estimated.getSigma()
for i in range(6):
    ott.assert_almost_equal(sigma[i, i], 1.0, 0.0, 0.2)
