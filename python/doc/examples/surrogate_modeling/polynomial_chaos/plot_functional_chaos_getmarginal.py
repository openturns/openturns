"""
Get the (output) marginal of a PCE
==================================
"""

# %%
from openturns.usecases import flood_model
import openturns as ot

# %%
ot.RandomGenerator.SetSeed(0)

# %%
# Create a PCE of a multivariate output function
fm = flood_model.FloodModel()
sampleSize = 100
inputTrain = fm.distribution.getSample(sampleSize)
print(f"Output dimension = {fm.model.getOutputDimension()}")
outputTrain = fm.model(inputTrain)
marginalList = [fm.distribution.getMarginal(i) for i in range(fm.distribution.getDimension())]
multivariateBasis = ot.OrthogonalProductPolynomialFactory(marginalList)
selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
projectionStrategy = ot.LeastSquaresStrategy(selectionAlgorithm)
totalDegree = 4
enumerateFunction = multivariateBasis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
adaptiveStrategy = ot.FixedStrategy(multivariateBasis, basisSize)
chaosAlgo = ot.FunctionalChaosAlgorithm(
    inputTrain, outputTrain, fm.distribution, adaptiveStrategy, projectionStrategy
)
chaosAlgo.run()
chaosResult = chaosAlgo.getResult()
chaosResult

# %%
# Get the marginal PCE corresponding to the third output.
marginal_pce = chaosResult.getMarginal(2)
marginal_pce


# %%
# Get the marginal PCE corresponding to the second and third outputs.
marginal_pce = chaosResult.getMarginal([1, 2])
marginal_pce

# %%
