#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ottest

dim = 3

factoryCollection = [ot.FourierSeriesFactory()] * dim

rank = 2
nk = [8] * dim
tensorEval = ot.CanonicalTensorEvaluation(factoryCollection, nk, rank)

for r in range(rank):
    for j in range(dim):
        coefs = ot.Normal(nk[j]).getRealization()
        tensorEval.setCoefficients(r, j, coefs)

tensorGrad = ot.CanonicalTensorGradient(tensorEval)


x = [1.0] * dim
print(x)

df = tensorGrad.gradient(x) * ot.Point([1.0])
print('df=', df)

f = ot.Function(tensorEval)
fx = f(x)[0]

eps = 1e-5

dffd = [0.0] * dim
for i in range(dim):
    xp = ot.Point(x)
    xp[i] += eps
    fp = f(xp)
    dffd[i] = (f(xp)[0] - fx) / eps
print('dffd', dffd)

# print(tensor)

ottest.assert_almost_equal(df, dffd, 1e-4)
