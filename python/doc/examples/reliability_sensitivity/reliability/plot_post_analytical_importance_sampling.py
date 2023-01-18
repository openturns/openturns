"""
Use the post-analytical importance sampling algorithm
=====================================================
"""
# %%
# In this example we want to estimate the probability to exceed a threshold through the combination of approximation and simulation methods.
#
#   - perform an FORM or SORM study in order to find the design point,
#
#   - perform an importance sampling study centered around the design point:
#     the importance distribution operates in the standard space and is the
#     standard distribution of the standard space (the standard elliptical
#     distribution in the case of an elliptic copula of the input random vector,
#     the standard normal one in all the other cases).
#
# The importance sampling technique in the standard space may be of two kinds:
#
#   - the sample is generated according to the new importance distribution:
#     this technique is called post analytical importance sampling,
#
#   - the sample is generated according to the new importance distribution and
#     is controlled by the value of the linearized limit state function:
#     this technique is called post analytical controlled importance sampling.
#
# This post analytical importance sampling algorithm is created from the result structure of a FORM or SORM algorithm.
#
# It is parameterized like other simulation algorithm, through the parameters OuterSampling, BlockSize, ... and provide the same type of results.
#
# Let us note that the post FORM/SORM importance sampling method may be
# implemented thanks to the ImportanceSampling object, where the importance
# distribution is defined in the standard space: then, it requires that the
# event initially defined in the pysical space be transformed in the standard space.
#
# The controlled importance sampling technique is only accessible within the post analytical context.

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# Create a model
model = ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2"])
R = ot.CorrelationMatrix(2)
R[0, 1] = -0.6
inputDist = ot.Normal([0.0, 0.0], R)
inputDist.setDescription(["X1", "X2"])
inputVector = ot.RandomVector(inputDist)

# Create the output random vector Y=model(X)
Y = ot.CompositeRandomVector(model, inputVector)

# Create the event Y > 4
threshold = 4.0
event = ot.ThresholdEvent(Y, ot.Greater(), threshold)

# %%
# Create a FORM algorithm
solver = ot.Cobyla()
startingPoint = inputDist.getMean()
algo = ot.FORM(solver, event, startingPoint)

# Run the algorithm and retrieve the result
algo.run()
result_form = algo.getResult()

# %%
# Create the post analytical importance sampling simulation algorithm
algo = ot.PostAnalyticalImportanceSampling(result_form)
algo.run()
algo.getResult()

# %%
# Create the post analytical controlled importance sampling simulation algorithm
algo = ot.PostAnalyticalControlledImportanceSampling(result_form)
algo.run()
algo.getResult()
