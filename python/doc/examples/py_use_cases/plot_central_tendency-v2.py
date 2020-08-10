"""
Central tendency analysis on the cantilever beam example-v2
===========================================================
"""
# %% 

# %%
# In this example we perform a central tendency analysis of a random variable Y using the various methods available. We consider the cantilever beam example (see :ref:`use-case-cantilever-beam`) and show how to use the `TaylorExpansionMoments` and `ExpectationSimulationAlgorithm` classes.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
from use_cases import use_case_cantilever_beam

cb = use_case_cantilever_beam.cantilever_beam()

# %%
# Create the random variable of interest Y=g(X).

# %%
mean = [50.0, 1.0, 10.0, 5.0]
dim = len(mean)
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
print(R)
#R = cb.R
distribution = ot.Normal(mean, sigma, R)
X = ot.RandomVector(distribution)
X.setDescription(['E', 'F', 'L', 'I'])
f = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['F*L^3/(3*E*I)'])
Y = ot.CompositeRandomVector(f, X)
Y.setDescription('Y')

# %%
# Taylor expansion
# ----------------

# %%
# Perform Taylor approximation to get the expected value of Y and the importance factors.

# %%
taylor = ot.TaylorExpansionMoments(Y)
taylor_mean_fo = taylor.getMeanFirstOrder()
taylor_mean_so = taylor.getMeanSecondOrder()
taylor_cov = taylor.getCovariance()
taylor_if = taylor.getImportanceFactors()
print('model evaluation calls number=', f.getGradientCallsNumber())
print('model gradient calls number=', f.getGradientCallsNumber())
print('model hessian calls number=', f.getHessianCallsNumber())
print('taylor mean first order=', taylor_mean_fo)
print('taylor variance=', taylor_cov)
print('taylor importance factors=', taylor_if)

# %%
graph = taylor.drawImportanceFactors()
view = viewer.View(graph)

# %%
# We see that, at first order, the variable :math:`F` explains 88.5% of the variance of the output :math:`Y`. On the other hand, the variable :math:`E` is not significant in the variance of the output: at first order, the random variable :math:`E` could be replaced by a constant with no change to the output variance.

# %%
# Monte-Carlo simulation
# ----------------------

# %%
# Perform a Monte Carlo simulation of Y to estimate its mean.

# %%
algo = ot.ExpectationSimulationAlgorithm(Y)
algo.setMaximumOuterSampling(1000)
algo.setCoefficientOfVariationCriterionType('NONE')
algo.run()
print('model evaluation calls number=', f.getEvaluationCallsNumber())
expectation_result = algo.getResult()
expectation_mean = expectation_result.getExpectationEstimate()
print('monte carlo mean=', expectation_mean, 'var=', expectation_result.getVarianceEstimate())

# %%
# Central dispersion analysis based on a sample
# ---------------------------------------------

# %%
# Directly compute statistical moments based on a sample of Y. Sometimes the probabilistic model is not available and the study needs to start from the data. 

# %%
Y_s = Y.getSample(1000)
y_mean = Y_s.computeMean()
y_stddev = Y_s.computeStandardDeviationPerComponent()
y_quantile_95p = Y_s.computeQuantilePerComponent(0.95)
print('mean=', y_mean, 'stddev=', y_stddev, 'quantile@95%', y_quantile_95p)

# %%
graph = ot.KernelSmoothing().build(Y_s).drawPDF()
graph.setTitle("Kernel smoothing approximation of the output distribution")
graph
view = viewer.View(graph)

plt.show()
