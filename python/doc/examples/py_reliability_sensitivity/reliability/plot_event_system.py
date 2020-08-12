"""
System events: unions or intersections of events
================================================
"""
# %%
# This example illustrates system events, which are defined as unions or intersections of other events. We will show how to estimate their probability both with Monte-Carlo sampling (using the class `ProbabilitySimulationAlgorithm`) and with a first order approximation (using the class `SystemFORM`).
#
# **Intersection**
#
# The event defined as the intersection of several events is realized when all sub-events occurs:
#
# .. math::
#    E_{sys} = \bigcap_{i=1}^N E_i
#   
# **Union**
#
# The event defined as the union of several events is realized when at least one sub-event occurs:
#
# .. math::
#    E_{sys} = \bigcup_{i=1}^N E_i
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# For system events, we always have to use the same root cause. 

# %%
dim = 2
distribution = ot.Normal(dim)
X = ot.RandomVector(distribution)

# %%
# Define some basic events E1, E2 and E3. 

# %%
f1 = ot.SymbolicFunction(['x0', 'x1'], ['x0'])
f2 = ot.SymbolicFunction(['x0', 'x1'], ['x1'])
f3 = ot.SymbolicFunction(['x0', 'x1'], ['x0+x1'])

Y1 = ot.CompositeRandomVector(f1, X)
Y2 = ot.CompositeRandomVector(f2, X)
Y3 = ot.CompositeRandomVector(f3, X)

e1 = ot.ThresholdEvent(Y1, ot.Less(), 0.0)  # E1 <=> x0<0
e2 = ot.ThresholdEvent(Y2, ot.Greater(), 0.0) # E2 <=> x1>0
e3 = ot.ThresholdEvent(Y3, ot.Greater(), 0.0) # E3 <=> x0+x1>0

# %%
# Define the intersection E3=E1 AND E2. 

# %%
e4 = ot.IntersectionEvent([e1, e2])

# %%
# Approximate probability of that event: :math:`\approx 1/4`. 

# %%
e4.getSample(10000).computeMean()

# %%
# Define the union E4=E1 OR E2. 

# %%
e5 = ot.UnionEvent([e1, e2])

# %%
# Approximate probability of that event: :math:`\approx ~3/4`. 

# %%
e5.getSample(10000).computeMean()

# %%
# It supports recursion: define E7=E1 OR (E1 AND E3). 

# %%
e7 = ot.UnionEvent([e1, ot.IntersectionEvent([e2, e3])])
print(e3.isComposite())

# %%
# With Monte-Carlo algorithm
# --------------------------

# %%
# Of course, we can use simulation algorithms. 

# %%
experiment = ot.MonteCarloExperiment()
algo = ot.ProbabilitySimulationAlgorithm(e7, experiment)
algo.setMaximumOuterSampling(2500)
algo.setBlockSize(4)
algo.setMaximumCoefficientOfVariation(-1.0)
algo.run()
result = algo.getResult()
result.getProbabilityEstimate()

# %%
# SystemFORM
# ----------
#
# `SystemFORM` is an approximation method suitable for system events.
#
# The event must be in its disjunctive normal form (union of intersections, or a single intersection).

# %%
# root cause
dim = 5
mean = [200.0] * dim
mean[-1] = 60
mean[-2] = 60
sigma = [30.0] * dim
sigma[-1] = 15.0
R = ot.CorrelationMatrix(dim)
for i in range(dim):
    for j in range(i):
        R[i, j] = 0.5
dist = ot.Normal(mean, sigma, R)

# %%
# leaf events
X = ot.RandomVector(dist)
inputs = ['M1', 'M2', 'M3', 'M4', 'M5']
e0 = ot.ThresholdEvent(ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ['M1-M2+M4']), X), ot.Less(), 0.0)
e1 = ot.ThresholdEvent(ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ['M2+2*M3-M4']), X), ot.Less(), 0.0)
e2 = ot.ThresholdEvent(ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ['2*M3-2*M4-M5']), X), ot.Less(), 0.0)
e3 = ot.ThresholdEvent(ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ['-(M1+M2+M4+M5-5*10.0)']), X), ot.Less(), 0.0)
e4 = ot.ThresholdEvent(ot.CompositeRandomVector(ot.SymbolicFunction(inputs, ['-(M2+2*M3+M4-5*40.0)']), X), ot.Less(), 0.0)

# %%
# system event in DNF form (union of intersections)
event = ot.UnionEvent([ot.IntersectionEvent([e0, e3, e4]), ot.IntersectionEvent([e2, e3, e4])])

# %%
# compute probability with basic sampling
event.getSample(10000).computeMean()

# %%
# Run systemFORM.

# %%
solver = ot.AbdoRackwitz()
solver.setMaximumIterationNumber(1000)
solver.setMaximumAbsoluteError(1.0e-3)
solver.setMaximumRelativeError(1.0e-3)
solver.setMaximumResidualError(1.0e-3)
solver.setMaximumConstraintError(1.0e-3)

algo = ot.SystemFORM(solver, event, mean)
algo.run()
result = algo.getResult()
result.getEventProbability()
