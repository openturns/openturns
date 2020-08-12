"""
Parametrization of a simulation algorithm
=========================================
"""
# %%
# In this example we are going to parameterize a simulation algorithm:
#
#  - parameters linked to the number of points generated
#  - the precision of the probability estimator
#  - the sample storage strategy
#  - using callbacks to monitor progress and stopping criteria.
#  

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Create the joint distribution of the parameters.

# %%
distribution_R = ot.LogNormalMuSigma(300.0, 30.0, 0.0).getDistribution()
distribution_F = ot.Normal(75e3, 5e3)
marginals = [distribution_R, distribution_F]
distribution = ot.ComposedDistribution(marginals)

# %%
# Create the model.

# %%
model = ot.SymbolicFunction(['R', 'F'], ['R-F/(pi_*100.0)'])

# %%
# Create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Less(), 0.0)

# %%
# Create a Monte Carlo algorithm.

# %%
experiment = ot.MonteCarloExperiment()
algo = ot.ProbabilitySimulationAlgorithm(event, experiment)

# %%
# Criteria 1: Define the Maximum Coefficient of variation of the probability estimator. 

# %%
algo.setMaximumCoefficientOfVariation(0.05)

# %%
# Criteria 2: Define the number of iterations of the simulation.

# %%
algo.setMaximumOuterSampling(int(1e4))

# %%
# The block size parameter represents the number of samples evaluated per iteration, useful for parallelization. 

# %%
algo.setBlockSize(2)

# %%
# HistoryStrategy to store the values of the probability used to draw the convergence graph. 

# %%
# Null strategy
algo.setConvergenceStrategy(ot.Null())

# Full strategy
algo.setConvergenceStrategy(ot.Full())

# Compact strategy: N points
N = 1000
algo.setConvergenceStrategy(ot.Compact(N))


# %%
# Use a callback to display the progress every 10%. 

# %%
def progress(p):
    if p >= progress.t:
        progress.t += 10.0
        print('progress=', p, '%')
    return False
progress.t = 10.0
algo.setProgressCallback(progress)


# %%
# Use a callback to stop the simulation. 

# %%
def stop():
    # here we never stop, but we could
    return False
algo.setStopCallback(stop)

# %%
algo.run()

# %%
# Retrieve results. 

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)
