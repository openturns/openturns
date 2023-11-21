"""
Control algorithm termination
=============================
"""
# %%
# In this examples we are going to expose ways to control the termination of optimization and simulation algorithms using callbacks.
#

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# Define an event to compute a probability
myFunction = ot.SymbolicFunction(["E", "F", "L", "I"], ["-F*L^3/(3.0*E*I)"])
dim = myFunction.getInputDimension()
mean = [50.0, 1.0, 10.0, 5.0]
sigma = [1.0] * dim
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)
vect = ot.RandomVector(myDistribution)
output = ot.CompositeRandomVector(myFunction, vect)
myEvent = ot.ThresholdEvent(output, ot.Less(), -3.0)

# %%
# **Stop a FORM algorithm using a calls number limit**
#
# A FORM algorithm termination can be controlled by the maximum number of iterations
#
# of its underlying optimization solver, but not directly by a maximum number of evaluations.

# %%
# Create the optimization algorithm
myCobyla = ot.Cobyla()
myCobyla.setMaximumCallsNumber(400)
myCobyla.setMaximumAbsoluteError(1.0e-10)
myCobyla.setMaximumRelativeError(1.0e-10)
myCobyla.setMaximumResidualError(1.0e-10)
myCobyla.setMaximumConstraintError(1.0e-10)


# %%
# Define the stopping criterion
def stop():
    return myFunction.getCallsNumber() > 100


myCobyla.setStopCallback(stop)

# %%
# Run FORM
myAlgo = ot.FORM(myCobyla, myEvent, mean)
myAlgo.run()
result = myAlgo.getResult()
print("event probability:", result.getEventProbability())
print("calls number:", myFunction.getCallsNumber())

# %%
# **Stop a simulation algorithm using a time limit**
#
# Here we will create a callback to not exceed a specified simulation time.

# %%
# Create simulation
experiment = ot.MonteCarloExperiment()
myAlgo = ot.ProbabilitySimulationAlgorithm(myEvent, experiment)
myAlgo.setMaximumOuterSampling(1000000)
myAlgo.setMaximumCoefficientOfVariation(-1.0)

# %%
# Define the stopping criterion
timer = ot.TimerCallback(0.01)
myAlgo.setStopCallback(timer)

# %%
# Run algorithm
myAlgo.run()
result = myAlgo.getResult()
print("event probability:", result.getProbabilityEstimate())
print("calls number:", myFunction.getCallsNumber())
