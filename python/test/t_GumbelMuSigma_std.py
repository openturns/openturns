#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

distParam = ot.GumbelMuSigma(1.5, 1.3)
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
eps = 0.01
nativeParamGrad = ot.SquareMatrix(ot.IdentityMatrix(2))

nativeParamGrad[0, 0] = (
    (distParam([1.5 + eps, 1.3]) - distParam([1.5 - eps, 1.3])) / (2 * eps))[0]
nativeParamGrad[1, 0] = (
    (distParam([1.5, 1.3 + eps]) - distParam([1.5, 1.3 - eps])) / (2 * eps))[0]

nativeParamGrad[0, 1] = (
    (distParam([1.5 + eps, 1.3]) - distParam([1.5 - eps, 1.3])) / (2 * eps))[1]
nativeParamGrad[1, 1] = (
    (distParam([1.5, 1.3 + eps]) - distParam([1.5, 1.3 - eps])) / (2 * eps))[1]

print(nativeParamGrad)
