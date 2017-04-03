#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

try:
    mesh = ot.IntervalMesher(ot.Indices(1, 9)).build(ot.Interval(-1.0, 1.0))
    factory = ot.KarhunenLoeveP1Factory(mesh, 0.0)
    eigenValues = ot.Point()
    KLModes = factory.buildAsProcessSample(
        ot.AbsoluteExponential([1.0]), eigenValues)
    print("KL modes=", KLModes)
    print("KL eigenvalues=", eigenValues)
    cov1D = ot.AbsoluteExponential([1.0])
    KLFunctions = factory.build(cov1D, eigenValues)
    print("KL functions=", KLFunctions)
    print("KL eigenvalues=", eigenValues)
    R = ot.CorrelationMatrix(2, [1.0, 0.5, 0.5, 1.0])
    scale = [1.0]
    amplitude = [1.0, 2.0]
    cov2D = ot.ExponentialModel(scale, amplitude, R)
    KLFunctions = factory.build(cov2D, eigenValues)
    print("KL functions=", KLFunctions)
    print("KL eigenvalues=", eigenValues)

except:
    import sys
    print("t_KarhunenLoeveP1Factory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
