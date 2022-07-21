#! /usr/bin/env python

import openturns as ot

mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
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
lambd = result.getEigenvalues()
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

# 2d input, to check that issue #1660 is solved
mesh = ot.IntervalMesher([4]*2).build(ot.Interval([-1.2]*2, [1.0]*2))
cov2D = ot.AbsoluteExponential([1.0]*2)
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov2D, 1e-3)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModesPS = result.getModesAsProcessSample()
# The output is hidden due to near-zero nonreproducible values
#print("KL modes (process sample)=", KLModesPS)
KLScaledModesPS = result.getScaledModesAsProcessSample()
# The output is hidden due to near-zero nonreproducible values
#print("KL scaled modes (process sample)=", KLScaledModesPS)
KLModes = result.getModes()
# The output is hidden due to near-zero nonreproducible values
#print("KL modes (functions)=", KLModes)
KLScaledModes = result.getScaledModes()
# The output is hidden due to near-zero nonreproducible values
#print("KL scaled modes (functions)=", KLScaledModes)
