"""
Regression analysis of a PCE
============================
"""

# %%
# In this example, we create a polynomial chaos expansion from regression
# and use regression analysis on the result.

# %%
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import ishigami_function


# %%
def ComputeSparseLeastSquaresFunctionalChaos(
    inputTrain,
    outputTrain,
    multivariateBasis,
    basisSize,
    distribution,
    sparse=True,
):
    """
    Create a sparse polynomial chaos based on least squares.

    * Uses the enumerate rule in multivariateBasis.
    * Uses the LeastSquaresStrategy to compute the coefficients based on
      least squares.
    * Uses LeastSquaresMetaModelSelectionFactory to use the LARS selection method.
    * Uses FixedStrategy in order to keep all the coefficients that the
      LARS method selected.

    Parameters
    ----------
    inputTrain : ot.Sample
        The input design of experiments.
    outputTrain : ot.Sample
        The output design of experiments.
    multivariateBasis : ot.Basis
        The multivariate chaos basis.
    basisSize : int
        The size of the function basis.
    distribution : ot.Distribution.
        The distribution of the input variable.
    sparse: bool
        If True, create a sparse PCE.

    Returns
    -------
    result : ot.PolynomialChaosResult
        The estimated polynomial chaos.
    """
    if sparse:
        selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
    else:
        selectionAlgorithm = ot.PenalizedLeastSquaresAlgorithmFactory()
    projectionStrategy = ot.LeastSquaresStrategy(
        inputTrain, outputTrain, selectionAlgorithm
    )
    adaptiveStrategy = ot.FixedStrategy(multivariateBasis, basisSize)
    chaosAlgorithm = ot.FunctionalChaosAlgorithm(
        inputTrain, outputTrain, distribution, adaptiveStrategy, projectionStrategy
    )
    chaosAlgorithm.run()
    chaosResult = chaosAlgorithm.getResult()
    return chaosResult


# %%
# Create a polynomial chaos expansion
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# %%
ot.Log.Show(ot.Log.NONE)
ot.RandomGenerator.SetSeed(0)
im = ishigami_function.IshigamiModel()
input_names = im.distributionX.getDescription()
sampleSize = 1000
inputSample = im.distributionX.getSample(sampleSize)
outputSample = im.model(inputSample)

# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory([im.X1, im.X2, im.X3])
totalDegree = 12
enumerateFunction = multivariateBasis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
print("Basis size = ", basisSize)

# %%
chaosResult = ComputeSparseLeastSquaresFunctionalChaos(
    inputSample,
    outputSample,
    multivariateBasis,
    basisSize,
    im.distributionX,
)
print("Selected basis size = ", chaosResult.getIndices().getSize())
chaosResult

# %%
# Validate the metamodel
metamodel = chaosResult.getMetaModel()
n_valid = 1000
inputTest = im.distributionX.getSample(n_valid)
outputTest = im.model(inputTest)
val = ot.MetaModelValidation(inputTest, outputTest, metamodel)
Q2 = val.computePredictivityFactor()[0]
graph = val.drawValidation()
graph.setTitle("Q2=%.2f%%" % (Q2 * 100))
view = otv.View(graph)

# %%
# Convert into a regression result
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# %%
# Convert the PCE into a regression result.
lmResult = chaosResult.getLinearModelResult()
lmResult

# %%
# We can then perform the analysis of the linear model, as
# is classical in regression analysis.
analysis = ot.LinearModelAnalysis(lmResult)
analysis

# %%
graph = analysis.drawModelVsFitted()
view = otv.View(graph)

# %%
graph = analysis.drawResidualsVsFitted()
view = otv.View(graph)

# %%
graph = analysis.drawScaleLocation()
view = otv.View(graph)

# %%
# sphinx_gallery_thumbnail_number = 5
graph = analysis.drawQQplot()
view = otv.View(graph)

# %%
graph = analysis.drawCookDistance()
view = otv.View(graph)

# %%
graph = analysis.drawResidualsVsLeverages()
view = otv.View(graph)

# %%
graph = analysis.drawCookVsLeverages()
view = otv.View(graph)
