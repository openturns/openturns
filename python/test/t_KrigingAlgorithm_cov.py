#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


def test_model(myModel):

    print("myModel = ",  myModel)

    dimension = myModel.getSpatialDimension()

    x1 = ot.NumericalPoint(dimension)
    x2 = ot.NumericalPoint(dimension)
    for j in range(dimension):
        x1[j] = -1.0 - j
        x2[j] = 3.0 + 2.0 * j

    print("myModel(", x1, ", ", x2, ")=",  myModel(x1, x2))

    grad = myModel.partialGradient(x1, x2)
    print("dCov =", grad)

    eps = 1e-5
    gradfd = ot.NumericalPoint(dimension)
    for j in range(dimension):
        x1_d = ot.NumericalPoint(x1)
        x1_d[j] = x1_d[j] + eps
        gradfd[j] = (myModel(x1_d, x2)[0, 0] - myModel(x1, x2)[0, 0]) / eps

    print("dCov (FD)=", gradfd)


dimension = 2


myDefautModel = ot.SquaredExponential()
print("myDefautModel = ",  myDefautModel)


myModel = ot.SquaredExponential(dimension)
test_model(myModel)


myDefautModel = ot.GeneralizedExponential()
print("myDefautModel = ",  myDefautModel)

myModel = ot.GeneralizedExponential(dimension, 10.0, 1.5)
test_model(myModel)


myDefautModel = ot.AbsoluteExponential()
print("myDefautModel = ",  myDefautModel)

myModel = ot.AbsoluteExponential(dimension)
test_model(myModel)


myDefautModel = ot.MaternModel()
print("myDefautModel = ",  myDefautModel)

myModel = ot.MaternModel(dimension, 8.0, 2.0)
test_model(myModel)


myDefautModel = ot.ProductCovarianceModel()
print("myDefautModel = ",  myDefautModel)

coll = ot.CovarianceModelCollection()
coll.add(ot.AbsoluteExponential(1, 3.0))
coll.add(ot.SquaredExponential(1, 2.0))
myModel = ot.ProductCovarianceModel(coll)
test_model(myModel)
