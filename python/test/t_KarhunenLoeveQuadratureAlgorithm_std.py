#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

try:
    ot.ResourceMap.SetAsScalar(
        "LinearCombinationEvaluation-SmallCoefficient", 1.0e-10)
    domain = ot.Interval(-1.0, 1.0)
    basis = ot.OrthogonalProductPolynomialFactory([ot.LegendreFactory()])
    basisSize = 5
    functions = [basis.build(i) for i in range(basisSize)]
    experiment = ot.LHSExperiment(basis.getMeasure(), 100)
    mustScale = False
    threshold = 0.0001
    model = ot.AbsoluteExponential([1.0])
    algo = ot.KarhunenLoeveQuadratureAlgorithm(
        domain, domain, model, experiment, functions, mustScale, threshold)
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
    KLFunctions = result.getModes()
    print("KL functions=", KLFunctions)
    print("KL lift=", result.lift(coefficients[0]))
    print("KL lift as field=", result.liftAsField(coefficients[0]))
    # Now using Legendre/Gauss quadrature
    marginalDegree = 5
    algo = ot.KarhunenLoeveQuadratureAlgorithm(
        domain, domain, model, marginalDegree, threshold)
    algo.run()
    result = algo.getResult()
    lambd = result.getEigenValues()
    KLModes = result.getScaledModesAsProcessSample()
    # Due to symmetry many results can have a sign switch depending on the CPU/compiler/BLAS used
    # print("KL modes=", KLModes)
    print("KL eigenvalues=", lambd)
    coefficients = result.project(sample)
    # print("KL coefficients=", coefficients)
    KLFunctions = result.getModes()
    # print("KL functions=", KLFunctions)
    lifted = result.lift(coefficients[0])
    # print("KL lift=", lifted)
    liftedAsField = result.liftAsField(coefficients[0])
    # print("KL lift as field=", liftedAsField)

except:
    import sys
    print("t_KarhunenLoeveQuadratureAlgorithm_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
