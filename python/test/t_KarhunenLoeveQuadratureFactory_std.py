#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

try:
    dim = 1
    domain = ot.Interval([-1.0] * dim, [1.0] * dim)
    basis = ot.OrthogonalProductPolynomialFactory([ot.LegendreFactory()] * dim)
    basisSize = 10
    experiment = ot.LHSExperiment(basis.getMeasure(), 1000)
    mustScale = False
    threshold = 0.01
    factory = ot.KarhunenLoeveQuadratureFactory(
        domain, experiment, basis, basisSize, mustScale, threshold)
    model = ot.AbsoluteExponential([1.0] * dim)
    lambd = ot.Point()
    KLModes = factory.build(model, lambd)
    # print("KL modes=", KLModes)
    print("KL eigenvalues=", lambd)

except:
    import sys
    print("t_KarhunenLoeveQuadratureFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
