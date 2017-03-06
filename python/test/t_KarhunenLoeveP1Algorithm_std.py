#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

try:
    mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
    cov1D = ot.AbsoluteExponential([1.0])
    algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
    algo.run()
    result = algo.getResult()
    lambd = result.getEigenValues()
    KLModes = result.getModesAsProcessSample()
    print("KL modes=", KLModes)
    print("KL eigenvalues=", lambd)
    process = ot.GaussianProcess(cov1D, KLModes.getMesh())
    coefficients = result.project(process.getSample(10))
    print("KL coefficients=", coefficients)
    KLFunctions = result.getModes()
    print("KL functions=", KLFunctions)
    print("KL lift=", result.lift(coefficients[0]))
    print("KL lift as field=", result.liftAsField(coefficients[0]))
    R = ot.CorrelationMatrix(2)
    R[0, 1] = 0.5
    scale = [1.0]
    amplitude = [1.0, 2.0]
    cov2D = ot.ExponentialModel(scale, amplitude, R)
    algo = ot.KarhunenLoeveP1Algorithm(mesh, cov2D, 0.0)
    algo.run()
    result = algo.getResult()
    lambd = result.getEigenValues()
    KLModes = result.getModesAsProcessSample()
    print("KL modes=", KLModes)
    print("KL eigenvalues=", lambd)
    process = ot.GaussianProcess(cov2D, KLModes.getMesh())
    coefficients = result.project(process.getSample(10))
    print("KL coefficients=", coefficients)
    KLFunctions = result.getModes()
    print("KL functions=", KLFunctions)
    print("KL lift=", result.lift(coefficients[0]))
    print("KL lift as field=", result.liftAsField(coefficients[0]))
except:
    import sys
    print("t_KarhunenLoeveP1Algorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
