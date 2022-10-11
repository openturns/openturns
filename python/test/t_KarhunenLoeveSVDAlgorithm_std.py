#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()


mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
# 1D mesh, 1D covariance, uniform weight, automatic centering, more samples
# than vertices
cov1D = ot.AbsoluteExponential([1.0])
sample = ot.GaussianProcess(cov1D, mesh).getSample(16)
algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModes = result.getModesAsProcessSample()
print("KL modes=", KLModes)
print("KL eigenvalues=", lambd)
coefficients = result.project(sample)
print("KL coefficients=", coefficients)
KLFunctions = result.getModes()
print("KL functions=", KLFunctions)
print("KL lift=", result.lift(coefficients[0]))
print("KL lift as field=", result.liftAsField(coefficients[0]))

# 1D mesh, 1D covariance, uniform weight, automatic centering
sample = ot.GaussianProcess(cov1D, mesh).getSample(6)
algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModes = result.getModesAsProcessSample()
print("KL modes=", KLModes)
print("KL eigenvalues=", lambd)
coefficients = result.project(sample)
print("KL coefficients=", coefficients)
KLFunctions = result.getModes()
print("KL functions=", KLFunctions)
print("KL lift=", result.lift(coefficients[0]))
print("KL lift as field=", result.liftAsField(coefficients[0]))

# 1D mesh, 1D covariance, uniform weight, declared centered
algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0, True)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModes = result.getModesAsProcessSample()
print("KL modes=", KLModes)
print("KL eigenvalues=", lambd)
coefficients = result.project(sample)
print("KL coefficients=", coefficients)
KLFunctions = result.getModes()
print("KL functions=", KLFunctions)
print("KL lift=", result.lift(coefficients[0]))
print("KL lift as field=", result.liftAsField(coefficients[0]))

# 1D mesh, 1D covariance, nonuniform weight, automatic centering
weights = mesh.computeWeights()
algo = ot.KarhunenLoeveSVDAlgorithm(sample, weights, 0.0, True)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModes = result.getModesAsProcessSample()
print("KL modes=", KLModes)
print("KL eigenvalues=", lambd)
coefficients = result.project(sample)
print("KL coefficients=", coefficients)
KLFunctions = result.getModes()
print("KL functions=", KLFunctions)
print("KL lift=", result.lift(coefficients[0]))
print("KL lift as field=", result.liftAsField(coefficients[0]))

# 1D mesh, 1D covariance, uniform weight, automatic centering
R = ot.CorrelationMatrix(2, [1.0, 0.5, 0.5, 1.0])
scale = [1.0]
amplitude = [1.0, 2.0]
cov2D = ot.ExponentialModel(scale, amplitude, R)
sample = ot.GaussianProcess(cov2D, mesh).getSample(6)
algo = ot.KarhunenLoeveSVDAlgorithm(sample, 0.0)
algo.run()
result = algo.getResult()
lambd = result.getEigenvalues()
KLModes = result.getModesAsProcessSample()
print("KL modes=", KLModes)
print("KL eigenvalues=", lambd)
coefficients = result.project(sample)
print("KL coefficients=", coefficients)
KLFunctions = result.getModes()
print("KL functions=", KLFunctions)
print("KL lift=", result.lift(coefficients[0]))
print("KL lift as field=", result.liftAsField(coefficients[0]))

# truncation test
def func(tau, theta):
    return m.sin(2 * m.pi * (tau - theta)) + 1.0


mesh_size = 10
mesh = ot.RegularGrid(0.0, 1.0 / (mesh_size - 1.0), mesh_size)
samples = ot.ProcessSample(mesh, 100, 1)
for i in range(samples.getSize()):
    samples[i] = [[func(tau[0], i)] for tau in mesh.getVertices()]
algo = ot.KarhunenLoeveSVDAlgorithm(samples, 0.0, True)
algo.run()
result = algo.getResult()
evs = result.getEigenvalues()
assert len(evs) == 10, "expected 10 values"
