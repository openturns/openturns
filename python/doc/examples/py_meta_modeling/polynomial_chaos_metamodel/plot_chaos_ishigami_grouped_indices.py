"""
Compute grouped indices for the Ishigami function
=================================================
"""
# %% 
#
# In this example, we compute grouped Sobol' indices for the Ishigami function.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
from math import pi
import openturns.viewer as otv
ot.Log.Show(ot.Log.NONE)

# %%
# Create the Ishigami test function.

# %%
ot.RandomGenerator.SetSeed(0)
formula = ['sin(X1) + 7. * sin(X2)^2 + 0.1 * X3^4 * sin(X1)']
input_names = ['X1', 'X2', 'X3']
g = ot.SymbolicFunction(input_names, formula)

# %%
# Create the probabilistic model

# %%
distributionList = [ot.Uniform(-pi, pi)] * 3
distribution = ot.ComposedDistribution(distributionList)

# %%
# Create a training sample

# %%
N = 100 
inputTrain = distribution.getSample(N)
outputTrain = g(inputTrain)

# %%
# Create the chaos.

# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory(distributionList)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(inputTrain, outputTrain, selectionAlgorithm)
totalDegree = 8
enumfunc = multivariateBasis.getEnumerateFunction()
P = enumfunc.getStrataCumulatedCardinal(totalDegree)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, P)
chaosalgo = ot.FunctionalChaosAlgorithm(inputTrain, outputTrain, distribution, adaptiveStrategy, projectionStrategy)

# %%
chaosalgo.run()
result = chaosalgo.getResult()
metamodel = result.getMetaModel()

# %%
# Print Sobol' indices

# %%
chaosSI = ot.FunctionalChaosSobolIndices(result) 
print(chaosSI.summary())

# %%
# We compute the first order indice of the group [0,1]. 

# %%
chaosSI.getSobolGroupedIndex([0,1])

# %%
# This group collects all the multi-indices containing variables only in this group, including interactions within the group (by decreasing order of significance):
#
# * [0,4,0] : 0.279938
# * [1,0,0] : 0.190322
# * [0,6,0] : 0.130033
# * [3,0,0] : 0.12058
# * [0,2,0] : 0.0250262

# %%
0.279938 + 0.190322 + 0.130033 + 0.12058 + 0.0250262

# %%
# The difference between the previous sum and the output of `getSobolGroupedIndex` is lower than 0.01, which is the threshold used by the `summary` method.

# %%
# We compute the total order indice of the group [1,2]. 

# %%
chaosSI.getSobolGroupedTotalIndex([1,2])

# %%
# This group collects all the multi-indices containing variables in this group, including interactions with variables outside the group:
#
# * [0,4,0] : 0.279938
# * [1,0,2] : 0.136823
# * [0,6,0] : 0.130033
# * [3,0,2] : 0.0837457
# * [0,2,0] : 0.0250262
# * [1,0,4] : 0.0111867

# %%
0.279938 + 0.136823 + 0.130033 + 0.0837457 + 0.0250262 + 0.0111867
