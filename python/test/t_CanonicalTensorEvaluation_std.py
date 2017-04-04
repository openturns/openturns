#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ottest

dim = 3

factoryCollection = [ot.FourierSeriesFactory()] * dim

rank = 2
nk = [10] * dim
tensorEval = ot.CanonicalTensorEvaluation(factoryCollection, nk, rank)

for r in range(rank):
    for j in range(dim):
        tensorEval.setCoefficients(r, j, [1.0] * nk[j])

f = ot.Function(tensorEval)
print(tensorEval)

x = [1.0] * dim
print(x)

y = f(x)
print(y)

ottest.assert_almost_equal(y, [-1.45674])

tensorEval.setRank(1)
f = ot.Function(tensorEval)
print(tensorEval)

y = f(x)
print(y)

ottest.assert_almost_equal(y, [-0.728368])
