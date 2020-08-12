"""
Test samples independence using regression
==========================================
"""
# %%
# In this example we are going to estimate whether samples are independent using the regression test.
#
# It consists in detecting a linear relation between two scalar samples.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Generate a sample of dimension 3 with component 0 correlated to component 2
marginals = [ot.Normal()] * 3
S = ot.CorrelationMatrix(3)
S[0, 2] = 0.9
copula = ot.NormalCopula(S)
distribution = ot.ComposedDistribution(marginals, copula)
sample = distribution.getSample(30)

# %%
# Split it in two samples: firstSample of dimension=2, secondSample of dimension=1
firstSample = sample[:, :2]
secondSample = sample[:, 2]

# %%
# Test independance of each components of firstSample against secondSample
test_results = ot.LinearModelTest.FullRegression(firstSample, secondSample)
for i in range(len(test_results)):
    print('Component', i, 'is independent?', test_results[i].getBinaryQualityMeasure(),
          'p-value=%.6g' % test_results[i].getPValue(),
          'threshold=%.6g' % test_results[i].getThreshold())
