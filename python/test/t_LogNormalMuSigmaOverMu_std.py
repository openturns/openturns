#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

distParam = ot.LogNormalMuSigmaOverMu(0.63, 5.24, -0.5)
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
nativeParamGrad = ot.SquareMatrix(ot.IdentityMatrix(3))

nativeParamGrad[0, 0] = (
    (distParam([0.63 + eps, 5.24, -0.5]) - distParam([0.63 - eps, 5.24, -0.5])) / (2 * eps))[0]
nativeParamGrad[1, 0] = (
    (distParam([0.63, 5.24 + eps, -0.5]) - distParam([0.63, 5.24 - eps, -0.5])) / (2 * eps))[0]
nativeParamGrad[2, 0] = (
    (distParam([0.63, 5.24, -0.5 + eps]) - distParam([0.63, 5.24, -0.5 - eps])) / (2 * eps))[0]

nativeParamGrad[0, 1] = (
    (distParam([0.63 + eps, 5.24, -0.5]) - distParam([0.63 - eps, 5.24, -0.5])) / (2 * eps))[1]
nativeParamGrad[1, 1] = (
    (distParam([0.63, 5.24 + eps, -0.5]) - distParam([0.63, 5.24 - eps, -0.5])) / (2 * eps))[1]
nativeParamGrad[2, 1] = (
    (distParam([0.63, 5.24, -0.5 + eps]) - distParam([0.63, 5.24, -0.5 - eps])) / (2 * eps))[1]

print(nativeParamGrad)
