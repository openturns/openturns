"""
Create a polynomial chaos for the Ishigami function: a quick start guide to polynomial chaos
============================================================================================
"""

from openturns.usecases import ishigami_function
import openturns as ot

# %%
# We load the Ishigami model :
im = ishigami_function.IshigamiModel()

# %%
N = 100
inputTrain = im.distributionX.getSample(N)
outputTrain = im.model(inputTrain)

# %%
chaosalgo = ot.FunctionalChaosAlgorithm(inputTrain, outputTrain)

# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory([im.X1, im.X2, im.X3])
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
totalDegree = 8
enumfunc = multivariateBasis.getEnumerateFunction()
P = enumfunc.getStrataCumulatedCardinal(totalDegree)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, P)
chaosalgo = ot.FunctionalChaosAlgorithm(
    inputTrain, outputTrain, im.distributionX, adaptiveStrategy, projectionStrategy
)

# %%
chaosalgo.run()
chaosResult = chaosalgo.getResult()

# %%
chaosResult

# %%
print(chaosResult)

# %%
print(chaosResult._repr_html_())

chaosSI = ot.FunctionalChaosSobolIndices(chaosResult)

# %%
chaosSI

# %%
print(chaosSI)

# %%
print(chaosSI._repr_html_())
