# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.5.1
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# %%
"""
Axial stressed beam : comparing different methods to estimate a probability
===========================================================================
"""
# %% 

# %%
# In this example, we compare four methods to estimate the probability in the axial stressed beam example:
#
# * Monte-Carlo simulation,
# * FORM,
# * directional sampling,
# * importance sampling with FORM design point: FORM-IS.
#
#
# Introduction
# ------------
# We consider a simple beam stressed by a traction load F at both sides.
#
# ![Axial stressed beam](axial-stressed-beam.png)
#
# The geometry is supposed to be deterministic: the diameter D is equal to:
#
# .. math::
#    D=0.02 \textrm{ (m)}.
# 
#
# By definition, the yield stress is the load divided by the surface. Since the surface is :math:`\pi D^2/4`, the stress is:
#
# .. math::
#    S = \frac{F}{\pi D^2/4}.
# 
#
# Failure occurs when the beam plastifies, i.e. when the axial stress gets larger than the yield stress:
#
# .. math::
#    R - \frac{F}{\pi D^2/4} \leq 0
# 
#
# where :math:`R` is the strength.
#  
# Therefore, the limit state function :math:`G` is: 
#
# .. math::
#    G(R,F) = R - \frac{F}{\pi D^2/4},
# 
#
# for any :math:`R,F\in\mathbb{R}`.
#
# The value of the parameter :math:`D` is such that:
#
# .. math::
#    D^2/4 = 10^{-4},
# 
#
# which leads to the equation:
#
# .. math::
#    G(R,F) = R - \frac{F}{10^{-4} \pi}.
# 
#
# We consider the following distribution functions.
#
# | Variable | Distribution |
# |--|--|
# | R | LogNormal(:math:`\mu_R=3\times 10^6`, :math:`\sigma_R=3\times 10^5`) [Pa] |
# | F | Normal(:math:`\mu_F=750`, :math:`\sigma_F=50`) [N] |
#
# where :math:`\mu_R=E(R)` and :math:`\sigma_R^2=V(R)` are the mean and the variance of :math:`R`.
#
# The failure probability is: 
#
# .. math::
#    P_f = \text{Prob}(G(R,F) \leq 0).
# 
#
# The exact :math:`P_f` is 
#
# .. math::
#    P_f = 0.02920.
# 

# %%
# Define the model
# ----------------

# %%
from __future__ import print_function

import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Define the dimension of the problem and the model. 

# %%
dim = 2
limitStateFunction = ot.SymbolicFunction(['R', 'F'], ['R-F/(pi_*100.0)'])

# %%
# Test of the limit state function.

# %%
x = [300., 75000.]
print('x=', x)
print('G(x)=', limitStateFunction(x))

# %%
# Then we define the probabilistic model. Create a first marginal: `LogNormal` distribution 1D, parameterized by its mean and standard deviation.

# %%
R_dist = ot.LogNormalMuSigma(300.0, 30.0, 0.0).getDistribution()
R_dist.setName('Yield strength')
R_dist.setDescription('R')
R_dist.drawPDF()

# %%
# Create a second marginal: `Normal` distribution 1D. 

# %%
F_dist = ot.Normal(75000., 5000.)
F_dist.setName('Traction_load')
F_dist.setDescription('F')
F_dist.drawPDF()

# %%
# Create a `IndependentCopula`. 

# %%
aCopula = ot.IndependentCopula(dim)
aCopula.setName('Independent copula')

# Instanciate one distribution object
myDistribution = ot.ComposedDistribution([R_dist, F_dist], aCopula)
myDistribution.setName('myDist')

# %%
# Given that the copula is independent by default, we can as well use only the first argument of the `ComposedDistribution` class.

# %%
myDistribution = ot.ComposedDistribution([R_dist, F_dist])
myDistribution.setName('myDist')

# %%
# We create a `RandomVector` from the `Distribution`, then a composite random vector. Finally, we create a `ThresholdEvent` from this `RandomVector`.

# %%
inputRandomVector = ot.RandomVector(myDistribution)
outputRandomVector = ot.CompositeRandomVector(limitStateFunction, inputRandomVector)
myEvent = ot.ThresholdEvent(outputRandomVector, ot.Less(), 0.0)

# %%
# Using Monte Carlo simulations
# -----------------------------

# %%
cv = 0.05
NbSim = 100000

experiment = ot.MonteCarloExperiment()
algoMC = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
algoMC.setMaximumOuterSampling(NbSim)
algoMC.setBlockSize(1)
algoMC.setMaximumCoefficientOfVariation(cv)

# %%
# For statistics about the algorithm
initialNumberOfCall = limitStateFunction.getEvaluationCallsNumber()

# %%
# Perform the analysis.

# %%
algoMC.run()

# %%
result = algoMC.getResult()
probabilityMonteCarlo = result.getProbabilityEstimate()
numberOfFunctionEvaluationsMonteCarlo = limitStateFunction.getEvaluationCallsNumber() - initialNumberOfCall
print('Number of calls to the limit state =', numberOfFunctionEvaluationsMonteCarlo)
print('Pf = ', probabilityMonteCarlo)
print('CV =', result.getCoefficientOfVariation())

# %%
graph = algoMC.drawProbabilityConvergence()
graph.setLogScale(ot.GraphImplementation.LOGX)
graph

# %%
# Using FORM analysis
# -------------------

# %%
# We create a NearestPoint algorithm
myCobyla = ot.Cobyla()
# Resolution options:
eps = 1e-3
myCobyla.setMaximumEvaluationNumber(100)
myCobyla.setMaximumAbsoluteError(eps)
myCobyla.setMaximumRelativeError(eps)
myCobyla.setMaximumResidualError(eps)
myCobyla.setMaximumConstraintError(eps)

# %%
# For statistics about the algorithm
initialNumberOfCall = limitStateFunction.getEvaluationCallsNumber()

# %%
# We create a FORM algorithm. The first parameter is a NearestPointAlgorithm. The second parameter is an event. The third parameter is a starting point for the design point research.

# %%
algoFORM = ot.FORM(myCobyla, myEvent, myDistribution.getMean())

# %%
# Perform the analysis.

# %%
algoFORM.run()

# %%
resultFORM = algoFORM.getResult()
numberOfFunctionEvaluationsFORM = limitStateFunction.getEvaluationCallsNumber() - initialNumberOfCall
probabilityFORM = resultFORM.getEventProbability()
print('Number of calls to the limit state =', numberOfFunctionEvaluationsFORM)
print('Pf =', probabilityFORM)

# %%
resultFORM.drawImportanceFactors()

# %%
# Using Directional sampling
# --------------------------

# %%
# Resolution options:
cv = 0.05
NbSim = 10000

algoDS = ot.DirectionalSampling(myEvent)
algoDS.setMaximumOuterSampling(NbSim)
algoDS.setBlockSize(1)
algoDS.setMaximumCoefficientOfVariation(cv)

# %%
# For statistics about the algorithm
initialNumberOfCall = limitStateFunction.getEvaluationCallsNumber()

# %%
# Perform the analysis.

# %%
algoDS.run()

# %%
result = algoDS.getResult()
probabilityDirectionalSampling = result.getProbabilityEstimate()
numberOfFunctionEvaluationsDirectionalSampling = limitStateFunction.getEvaluationCallsNumber() - initialNumberOfCall
print('Number of calls to the limit state =', numberOfFunctionEvaluationsDirectionalSampling)
print('Pf = ', probabilityDirectionalSampling)
print('CV =', result.getCoefficientOfVariation())

# %%
graph = algoDS.drawProbabilityConvergence()
graph.setLogScale(ot.GraphImplementation.LOGX)
graph

# %%
# Using importance sampling with FORM design point: FORM-IS
# ---------------------------------------------------------

# %%
# The `getStandardSpaceDesignPoint` method returns the design point in the U-space.

# %%
standardSpaceDesignPoint = resultFORM.getStandardSpaceDesignPoint()
standardSpaceDesignPoint

# %%
# The key point is to define the importance distribution in the U-space. To define it, we use a multivariate standard Gaussian and configure it so that the center is equal to the design point in the U-space.

# %%
dimension = myDistribution.getDimension()
dimension

# %%
myImportance = ot.Normal(dimension)
myImportance.setMean(standardSpaceDesignPoint)
myImportance

# %%
# Create the design of experiment corresponding to importance sampling. This generates a `WeightedExperiment` with weights corresponding to the importance distribution.

# %%
experiment = ot.ImportanceSamplingExperiment(myImportance)

# %%
# Create the standard event corresponding to the event. This transforms the original problem into the U-space, with Gaussian independent marginals.

# %%
standardEvent = ot.StandardEvent(myEvent)

# %%
# We then create the simulation algorithm. 

# %%
algo = ot.ProbabilitySimulationAlgorithm(standardEvent, experiment)
algo.setMaximumCoefficientOfVariation(cv)
algo.setMaximumOuterSampling(40000)

# %%
# For statistics about the algorithm
initialNumberOfCall = limitStateFunction.getEvaluationCallsNumber()

# %%
algo.run()

# %%
# retrieve results
result = algo.getResult()
probabilityFORMIS = result.getProbabilityEstimate()
numberOfFunctionEvaluationsFORMIS = limitStateFunction.getEvaluationCallsNumber() - initialNumberOfCall
print('Number of calls to the limit state =', numberOfFunctionEvaluationsFORMIS)
print('Pf = ', probabilityFORMIS)
print('CV =', result.getCoefficientOfVariation())

# %%
# Conclusion
# ----------

# %%
# We now compare the different methods in terms of accuracy and speed.

# %%
import numpy as np


# %%
# The following function computes the number of correct base-10 digits in the computed result compared to the exact result.

# %%
def computeLogRelativeError(exact, computed):
    logRelativeError = -np.log10(abs(exact - computed) / abs(exact))
    return logRelativeError


# %%
# The following function prints the results.

# %%
def printMethodSummary(name, computedProbability, numberOfFunctionEvaluations):
    print("---")
    print(name,":")
    print('Number of calls to the limit state =', numberOfFunctionEvaluations)
    print('Pf = ', computedProbability)
    exactProbability = 0.02919819462483051
    logRelativeError = computeLogRelativeError(exactProbability, computedProbability)
    print("Number of correct digits=%.3f" % (logRelativeError))
    performance = logRelativeError/numberOfFunctionEvaluations
    print("Performance=%.2e (correct digits/evaluation)" % (performance))
    return


# %%
printMethodSummary("Monte-Carlo", probabilityMonteCarlo, numberOfFunctionEvaluationsMonteCarlo)
printMethodSummary("FORM", probabilityFORM, numberOfFunctionEvaluationsFORM)
printMethodSummary("DirectionalSampling", probabilityDirectionalSampling, numberOfFunctionEvaluationsDirectionalSampling)
printMethodSummary("FORM-IS", probabilityFORMIS, numberOfFunctionEvaluationsFORMIS)

# %%
# We see that all three methods produce the correct probability, but not with the same accuracy. In this case, we have found the correct order of magnitude of the probability, i.e. between one and two correct digits. There is, however, a significant difference in computational performance (measured here by the number of function evaluations).
#
# * The fastest method is the FORM method, which produces more than 1 correct digit with less than 98 function evaluations with a performance equal to :math:`1.60 \times 10^{-2}` (correct digits/evaluation). A practical limitation is that the FORM method does not produce a confidence interval: there is no guarantee that the computed probability is correct.
# * The slowest method is Monte-Carlo simulation, which produces more than 1 correct digit with 12806 function evaluations. This is associated with a very slow performance equal to :math:`1.11 \times 10^{-4}` (correct digits/evaluation). The interesting point with the Monte-Carlo simulation is that the method produces a confidence interval.
# * The DirectionalSampling method is somewhat in-between the two previous methods.
# * The FORM-IS method produces 2 correct digits and has a small number of function evaluations. It has an intermediate performance equal to :math:`2.37\times 10^{-3}` (correct digits/evaluation). It combines the best of the both worlds: it has the small number of function evaluation of FORM computation and the confidence interval of Monte-Carlo simulation.
