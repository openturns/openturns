#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

distParam = ot.ArcsineMuSigma(8.4, 2.25)
print("Distribution Parameters ", repr(distParam))
print("Distribution Parameters ", distParam)

non_native = distParam.getParameters()
print('non-native=', non_native)
native = distParam(non_native)
print('native=', native)
non_native = distParam.inverse(native)
print('non-native=', non_native)

# derivative of the native parameters with regards the parameters of the
# distribution
print(distParam.gradient())

# by the finite difference technique
eps = 1e-5
nativeParamGrad = ot.SquareMatrix(ot.IdentityMatrix(2))

nativeParamGrad[0, 0] = (
    (distParam([8.4 + eps, 2.25]) - distParam([8.4 - eps, 2.25])) / (2 * eps))[0]
nativeParamGrad[1, 0] = (
    (distParam([8.4, 2.25 + eps]) - distParam([8.4, 2.25 - eps])) / (2 * eps))[0]

nativeParamGrad[0, 1] = (
    (distParam([8.4 + eps, 2.25]) - distParam([8.4 - eps, 2.25])) / (2 * eps))[1]
nativeParamGrad[1, 1] = (
    (distParam([8.4, 2.25 + eps]) - distParam([8.4, 2.25 - eps])) / (2 * eps))[1]

print(nativeParamGrad)
