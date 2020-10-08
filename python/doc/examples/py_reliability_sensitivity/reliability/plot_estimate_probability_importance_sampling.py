"""
Probability estimation with importance sampling simulation on cantilever beam example
=====================================================================================
"""
# %%
# In this example we estimate a failure probability with the importance sampling simulation algorithm provided by the `ImportanceSamplingExperiment` class. 
#
# The main steps of this method are:
#
# * run a FORM analysis,
# * create an importance distribution based on the results of the FORM results,
# * run a sampling-based probability estimate algorithm. 
#
#
# Description of the problem
# --------------------------
#
# Let us consider the analytical example of a cantilever beam, with Young's modulus E, length L and section modulus I.
#
# One end of the cantilever beam is built in a wall and we apply a concentrated bending load F at the other end of the beam, resulting in a deviation:
#
# .. math::
#    d = \frac{FL^3}{3EI}
# 
#
# Failure occurs when the beam deviation is too large:
#
# .. math::d \ge 30 (cm)
#   
# Four independent random variables are considered:
#
#  - E: Young's modulus [Pa]
#  - F: load [N]
#  - L: length [m]
#  - I: section [m^4]
#
# Stochastic model (simplified model, no units):
#
#  - E ~ Beta(0.93, 2.27, 2.8e7, 4.8e7)
#  - F ~ LogNormal(30000, 9000, 15000)
#  - L ~ Uniform(250, 260)
#  - I ~ Beta(2.5, 1.5, 3.1e2, 4.5e2)
#

# %%
# Define the cantilever beam model
# --------------------------------

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Create the marginal distributions of the parameters
dist_E = ot.Beta(0.93, 2.27, 2.8e7, 4.8e7)
dist_F = ot.LogNormalMuSigma(30000, 9000, 15000).getDistribution()
dist_L = ot.Uniform(250, 260)
dist_I = ot.Beta(2.5, 1.5, 3.1e2, 4.5e2)
marginals = [dist_E, dist_F, dist_L, dist_I]

# %%
# Create the Copula
RS = ot.CorrelationMatrix(4)
RS[2, 3] = -0.2
# Evaluate the correlation matrix of the Normal copula from RS
R = ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(RS)
# Create the Normal copula parametrized by R
copula = ot.NormalCopula(R) 

# %%
# Create the joint probability distribution
distribution = ot.ComposedDistribution(marginals, copula)

# %%
# create the model
model = ot.SymbolicFunction(['E', 'F', 'L', 'I'], ['F*L^3/(3*E*I)'])

# %%
# Define the event
# ----------------

# %%
# We create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Greater(), 30.0)

# %%
# Run a FORM analysis
# -------------------

# %%
# Define a solver
optimAlgo = ot.Cobyla()
optimAlgo.setMaximumEvaluationNumber(1000)
optimAlgo.setMaximumAbsoluteError(1.0e-10)
optimAlgo.setMaximumRelativeError(1.0e-10)
optimAlgo.setMaximumResidualError(1.0e-10)
optimAlgo.setMaximumConstraintError(1.0e-10)

# %%
# Run FORM
algo = ot.FORM(optimAlgo, event, distribution.getMean())
algo.run()
result = algo.getResult()

# %%
# Configure an importance sampling algorithm
# ------------------------------------------

# %%
# The `getStandardSpaceDesignPoint` method returns the design point in the U-space.

# %%
standardSpaceDesignPoint = result.getStandardSpaceDesignPoint()
standardSpaceDesignPoint

# %%
# The key point is to define the importance distribution in the U-space. To define it, we use a multivariate standard Gaussian centered around the design point in the U-space.

# %%
dimension = distribution.getDimension()
dimension

# %%
myImportance = ot.Normal(dimension)
myImportance.setMean(standardSpaceDesignPoint)
myImportance

# %%
# Create the design of experiment corresponding to importance sampling. This generates a `WeightedExperiment` with weights fitting to the importance distribution.

# %%
experiment = ot.ImportanceSamplingExperiment(myImportance)
type(experiment)

# %%
# Create the standard event corresponding to the event. This pushes the original problem to the U-space, with Gaussian independent marginals. 

# %%
standardEvent = ot.StandardEvent(event)

# %%
# Run the importance sampling simulation
# --------------------------------------

# %%
# We then create the simulation algorithm. 

# %%
algo = ot.ProbabilitySimulationAlgorithm(standardEvent, experiment)
algo.setMaximumCoefficientOfVariation(0.1)
algo.setMaximumOuterSampling(40000)
algo.run()

# %%
# retrieve results
result = algo.getResult()
probability = result.getProbabilityEstimate()
probability

# %%
# In order to compute the confidence interval, we use the `getConfidenceLength` method, which returns the length of the interval. In order to compute the bounds of the interval, we divide this length by 2.

# %%
alpha = 0.05

# %%
pflen = result.getConfidenceLength(1-alpha)
print("%.2f%% confidence interval = [%f,%f]" % ((1-alpha)*100,probability-pflen/2,probability+pflen/2))
