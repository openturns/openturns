#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

try:
    mesh = ot.IntervalMesher(ot.Indices(1, 9)).build(ot.Interval(-1.0, 1.0))
    factory = ot.KarhunenLoeveP1Factory(mesh, 0.0);
    eigenValues = ot.NumericalPoint()
    KLModes = factory.buildAsProcessSample(ot.AbsoluteExponential(1, 1.0), eigenValues)
    print("KL modes=", KLModes)
    print("KL eigenvalues=", eigenValues)

except:
    import sys
    print("t_KarhunenLoeveP1Factory_std.py", sys.exc_info()[0], sys.exc_info()[1])
