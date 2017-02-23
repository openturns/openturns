#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

try:
    ot.ResourceMap.SetAsNumericalScalar("LinearCombinationEvaluation-SmallCoefficient", 1.0e-10)
    domain = ot.Interval(-1.0, 1.0)
    basis = ot.OrthogonalProductPolynomialFactory([ot.LegendreFactory()])
    basisSize = 5
    experiment = ot.LHSExperiment(basis.getMeasure(), 100)
    mustScale = False
    threshold = 0.0001
    model = ot.AbsoluteExponential([1.0])
    algo = ot.KarhunenLoeveQuadratureAlgorithm(domain, model, experiment, basis, basisSize, mustScale, threshold)
    algo.run()
    result = algo.getResult()
    lambd = result.getEigenValues()
    KLModes = result.getModesAsProcessSample()
    print("KL modes=", KLModes)
    print("KL eigenvalues=", lambd)
    process = ot.GaussianProcess(model, KLModes.getMesh())
    sample = process.getSample(10)
    coefficients = result.project(sample)
    print("KL coefficients=", coefficients)
    KLFunctions  = result.getModes()
    print("KL functions=", KLFunctions)
    print("KL lift=", result.lift(coefficients[0]))
    print("KL lift as field=", result.liftAsField(coefficients[0]))
    # Now using Legendre/Gauss quadrature
    marginalDegree = 5
    algo = ot.KarhunenLoeveQuadratureAlgorithm(domain, model, marginalDegree, threshold)
    algo.run()
    result = algo.getResult()
    lambd = result.getEigenValues()
    KLModes = result.getScaledModesAsProcessSample()
    print("KL modes=", KLModes)
    print("KL eigenvalues=", lambd)
    coefficients = result.project(sample)
    print("KL coefficients=", coefficients)
    KLFunctions = result.getModes()
    print("KL functions=", KLFunctions)
    print("KL lift=", result.lift(coefficients[0]))
    print("KL lift as field=", result.liftAsField(coefficients[0]))

except:
    import sys
    print("t_KarhunenLoeveQuadratureAlgorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
