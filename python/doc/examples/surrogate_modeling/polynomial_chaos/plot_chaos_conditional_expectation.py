"""
Reduce the dimension of a model
===============================
"""

# %%
#
# In this example, we show different ways to reduce the input dimension of a model by
# fixing some of its components to some given values:
#
# - simply fix some components to given values, using the :class:`~openturns.ParametricFunction`
#   class,
# - from a Functional Chaos Expansion, compute the mean of the meta model with respect to the free values,
#   using the :meth:`~openturns.FunctionalChaosResult.getConditionalExpectation` method
#   of the :class:`~openturns.FunctionalChaosResult` class. This case is valid only of the input random
#   vector has independent marginals.

# %%
# Introduction
# ~~~~~~~~~~~~
# Let :math:`\inputRV \in \Rset^{\inputDim}` be a random vector with finite mean and
# :math:`\model` be a model:
#
# .. math::
#     \model : \Rset^\inputDim \rightarrow \Rset.
#
# Given :math:`\vect{u} \subseteq \{1, ..., \inputDim\}` a set
# of indices, we want to create a new function :math:`\widehat{\model}`:
#
# .. math::
#     \widehat{\model}: \Rset^{|\vect{u}|} \rightarrow \Rset
#
# where :math:`|\vect{u}| = \operatorname{card}(\vect{u})` is the
# number of the remaining variables.
#
# Let :math:`\overline{\vect{u}}` be the complementary set of input
# marginal indices:
#
# .. math::
#
#     \vect{u} \; \dot{\cup} \; \overline{\vect{u}} = \{1, ..., \inputDim\}.

# %%
# Parametric function
# ~~~~~~~~~~~~~~~~~~~
#
# The first method to reduce the dimension of the input
# is to set some input variables to fixed values :math:`\vect{x}_{\overline{\vect{u}}}^0`.
# The parametric function with reduced dimension is defined by:
#
# .. math::
#
#   \widehat{\model}(\inputReal_{\vect{u}})
#   = \model\left(\inputReal_{\vect{u}},
#            \inputReal_{\overline{\vect{u}}}
#            =  \inputReal_{\overline{\vect{u}}}^0\right)
#
# for any :math:`\inputReal_{\vect{u}} \in \Rset^{|\vect{u}|}`. The fixed values can be the mean
# of the random vector :math:`\vect{X}_{\overline{\vect{u}}}` if it exists.
#
# The :math:`\widehat{\model}` function is a parametric function based on the function :math:`\model`
# where the parameter is :math:`\inputReal_{\overline{\vect{u}}}^0`.
# This function can be created using the :class:`~openturns.ParametricFunction`
# class.

# %%
# Parametric FCE
# ~~~~~~~~~~~~~~
#
# Assume here that the model has a finite variance and that it is expressed as a FunctionalChaos
# Expansion (FCE). As done previously,
# we can fix some of
# its components to some given values so as  to get a model which depends on the free components only.
#
# Refer to :any:`functional_chaos` to get details on the mathematical aspects and the notations.
# The model can be written as:
#
# .. math::
#    :label: ModelFCE_ex
#
#    g(\inputReal)
#    = \sum_{\vect{\alpha} \in \set{J}^P}
#    a_{\vect{\alpha}} \psi_{\vect{\alpha}}(\inputReal)
#
# and the orthonormal basis (which is a tensorized basis) can be written as:
#
# .. math::
#
#    \psi_{\vect{\alpha}}(\inputReal)
#    = \prod_{i = 1}^\inputDim \pi_{\alpha_i}^{(i)}(x_i)
#
# so that the model conditionned to :math:`\vect{X}_{\bar{\vect{u}}} = \vect{x}_{\bar{\vect{u}}}^0`
# can be written as:
#
# .. math::
#
#    \widehat{\model}(\vect{x}_{\vect{u}})
#    = \sum_{\vect{\alpha} \in \set{J}^P}
#    \left(a_{\vect{\alpha}} \prod_{i \in \bar{\vect{u}}} \pi_{\alpha_i}^{(i)}(x_i^0)\right)
#    \prod_{j \in \vect{u}} \pi_{\alpha_j}^{(j)}(x_j).
#

# %%
# Conditional expectation of a FCE
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# The previous parametric FCE fixes some components to some given values. It is also possible to
# compute the mean of the model :eq:`ModelFCE_ex` with respect to these components. In that case, we
# get a FCE wich is expressed in the free components. The resulting FCE is:
#
# .. math::
#
#   \widehat{\model}(\vect{u})\left(\inputReal_{\vect{u}}\right)  =
#   \mathbb{E}_{\vect{X}_{\overline{\vect{u}}}}
#    \left[\model(\inputRV) | \inputRV_{\vect{u}} = \inputReal_{\vect{u}}\right]
#
# for any :math:`\inputReal_{\vect{u}} \in \Rset^{|\vect{u}|}`.
#
# This reduced model is the conditional FCE, computed by the
# :meth:`~openturns.FunctionalChaosResult.getConditionalExpectation`
# method from the :class:`~openturns.FunctionalChaosResult` class. This method assumes that the input
# distribution :math:`\mu_{\inputRV}` has
# independent marginals and that the basis :math:`\left(\psi_k\right)_{k \in I_n}`  is orthonormal
# with respect to  :math:`\mu_{\inputRV}`. This basis is built as the tensorization of univariate basis
# orthonormal with respect to  the marginal distributions.

# %%
# Create the PCE
# ~~~~~~~~~~~~~~

# %%
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import ishigami_function
import matplotlib.pyplot as plt

# %%
# The next function creates a parametric PCE based on a
# given PCE and a set of indices.


# %%
def meanParametricPCE(chaosResult, indices):
    """
    Return the parametric PCE of Y with given input marginals set to the mean.

    All marginal inputs, except those in the conditioning indices
    are set to the mean of the input random vector.

    The resulting function is :

    g(xu) = PCE(xu, xnotu = E[Xnotu])

    where xu is the input vector of conditioning indices,
    xnotu is the input vector fixed indices and
    E[Xnotu] is the expectation of the random vector of the components
    not in u.

    Parameters
    ----------
    chaosResult: ot.FunctionalChaosResult(inputDimension)
        The polynomial chaos expansion.
    indices: ot.Indices()
        The indices of the input variables which are set to constant values.

    Returns
    -------
    parametricPCEFunction : ot.ParametricFunction(reducedInputDimension, outputDimension)
        The parametric PCE.
        The reducedInputDimension is equal to inputDimension - indices.getSize().
    """
    distribution = chaosResult.getDistribution()
    # Create the parametric function
    pceFunction = chaosResult.getMetaModel()
    xMean = distribution.getMean()
    referencePoint = xMean[indices]
    parametricPCEFunction = ot.ParametricFunction(pceFunction, indices, referencePoint)
    return parametricPCEFunction


# %%
# The next function creates a sparse PCE using least squares.


# %%
def computeSparseLeastSquaresFunctionalChaos(
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
# In the next cell, we create a training sample from the
# Ishigami test function.
# We choose a sample size equal to 500.

# %%
ot.RandomGenerator.SetSeed(0)
im = ishigami_function.IshigamiModel()
input_names = im.distribution.getDescription()
sampleSize = 500
inputSample = im.distribution.getSample(sampleSize)
outputSample = im.model(inputSample)


# %%
# We then create a sparce PCE of the Ishigami function using
# a candidate basis up to the total degree equal to 10.
# This leads to 285 candidate coefficients.
# The coefficients are computed from least squares.

# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory([im.X1, im.X2, im.X3])
totalDegree = 10
enumerateFunction = multivariateBasis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
print("Basis size = ", basisSize)

# %%
# Finally, we create the PCE.
# Only 28 coefficients are selected by the :class:`~openturns.LARS`
# algorithm.

# %%
chaosResult = computeSparseLeastSquaresFunctionalChaos(
    inputSample,
    outputSample,
    multivariateBasis,
    basisSize,
    im.distribution,
)
print("Selected basis size = ", chaosResult.getIndices().getSize())
chaosResult


# %%
# In order to see the structure of the data, we create a grid of
# plots which shows all projections of :math:`Y` versus :math:`X_i`
# for :math:`i = 1, 2, 3`.
# We see that the Ishigami function is particularly non linear.

# %%
grid = ot.VisualTest.DrawPairsXY(inputSample, outputSample)
grid.setTitle(f"n = {sampleSize}")
view = otv.View(grid, figure_kw={"figsize": (8.0, 3.0)})
plt.subplots_adjust(wspace=0.4, bottom=0.25)

# %%
# Parametric function
# ~~~~~~~~~~~~~~~~~~~
#
# We now create the parametric function where :math:`X_i` is free
# and the other variables are set to their mean values.
# We can show that a parametric PCE is, again, a PCE.
# The library does not currently implement this feature.
# In the next cell, we create it from the `meanParametricPCE` we defined
# previously.

# %%
# Create different parametric functions for the PCE.
# In the next cell, we create the parametric PCE function
# where :math:`X_1` is active while :math:`X_2` and :math:`X_3` are
# set to their mean values.
indices = [1, 2]
parametricPCEFunction = meanParametricPCE(chaosResult, indices)
print(parametricPCEFunction.getInputDimension())


# %%
# Now that we know how the `meanParametricPCE` works, we loop over
# the input marginal indices and consider the three functions
# :math:`\widehat{\model}_1(\inputReal_1)`,
# :math:`\widehat{\model}_2(\inputReal_2)` and
# :math:`\widehat{\model}_3(\inputReal_3)`.
# For each marginal index `i`, we  plot the output :math:`Y`
# against the input marginal :math:`X_i` of the sample.
# Then we plot the parametric function depending on :math:`X_i`.

# %%
inputDimension = im.distribution.getDimension()
npPoints = 100
inputRange = im.distribution.getRange()
inputLowerBound = inputRange.getLowerBound()
inputUpperBound = inputRange.getUpperBound()
# Create the palette with transparency
palette = ot.Drawable().BuildDefaultPalette(2)
firstColor = palette[0]
r, g, b, a = ot.Drawable.ConvertToRGBA(firstColor)
newAlpha = 64
newColor = ot.Drawable.ConvertFromRGBA(r, g, b, newAlpha)
palette[0] = newColor
grid = ot.VisualTest.DrawPairsXY(inputSample, outputSample)
reducedBasisSize = chaosResult.getCoefficients().getSize()
grid.setTitle(
    f"n = {sampleSize}, total degree = {totalDegree}, "
    f"basis = {basisSize}, selected = {reducedBasisSize}"
)
for i in range(inputDimension):
    graph = grid.getGraph(0, i)
    graph.setLegends(["Data"])
    graph.setXTitle(f"$x_{1 + i}$")
    # Set all indices except i
    indices = list(range(inputDimension))
    indices.pop(i)
    parametricPCEFunction = meanParametricPCE(chaosResult, indices)
    xiMin = inputLowerBound[i]
    xiMax = inputUpperBound[i]
    curve = parametricPCEFunction.draw(xiMin, xiMax, npPoints).getDrawable(0)
    curve.setLineWidth(2.0)
    curve.setLegend(r"$PCE(x_i, x_{-i} = \mathbb{E}[X_{-i}])$")
    graph.add(curve)
    if i < inputDimension - 1:
        graph.setLegends([""])
    graph.setColors(palette)
    grid.setGraph(0, i, graph)

grid.setLegendPosition("upper right")
view = otv.View(
    grid,
    figure_kw={"figsize": (8.0, 3.0)},
    legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"},
)
plt.subplots_adjust(wspace=0.4, right=0.7, bottom=0.25)

# %%
# We see that the parametric function is located within each cloud, but
# sometimes seems a little vertically on the edges of the data.
# More precisely, the function represents well how :math:`Y` depends
# on :math:`X_2`, but does not seem to represent well how :math:`Y`
# depends on :math:`X_1` or :math:`X_3`.

# %%
# Conditional expectation
# ~~~~~~~~~~~~~~~~~~~~~~~

# %%
# In the next cell, we create the conditional expectation function
# :math:`\Expect{\model(\inputReal) \; | \; \inputRV_1 = \inputReal_1}`.
# The input random vector satifies the required conditions to use the method.

# %%
conditionalPCE = chaosResult.getConditionalExpectation([0])
conditionalPCE

# %%
# On output, we see that the result is, again, a PCE.
# Moreover, a subset of the previous coefficients are presented in this
# conditional expectation: only multi-indices which involve
# :math:`X_1` are presented (and the other marginal components are removed).
# We observe that the value of the coefficients are unchanged with respect to the
# previous PCE.

# %%
# In the next cell, we create the conditional expectation function
# :math:`\Expect{\model(\inputReal) \; | \; \inputRV_2 = \inputReal_2, \inputRV_3 = \inputReal_3}`.

# %%
conditionalPCE = chaosResult.getConditionalExpectation([1, 2])
conditionalPCE

# %%
# We see that the conditional PCE has input dimension 2.


# %%
# In the next cell, we compare the parametric PCE and the conditional
# expectation of the PCE.

# sphinx_gallery_thumbnail_number = 3
inputDimension = im.distribution.getDimension()
npPoints = 100
inputRange = im.distribution.getRange()
inputLowerBound = inputRange.getLowerBound()
inputUpperBound = inputRange.getUpperBound()
# Create the palette with transparency
palette = ot.Drawable().BuildDefaultPalette(3)
firstColor = palette[0]
r, g, b, a = ot.Drawable.ConvertToRGBA(firstColor)
newAlpha = 64
newColor = ot.Drawable.ConvertFromRGBA(r, g, b, newAlpha)
palette[0] = newColor
grid = ot.VisualTest.DrawPairsXY(inputSample, outputSample)
grid.setTitle(f"n = {sampleSize}, total degree = {totalDegree}")
for i in range(inputDimension):
    graph = grid.getGraph(0, i)
    graph.setLegends(["Data"])
    graph.setXTitle(f"$x_{1 + i}$")
    xiMin = inputLowerBound[i]
    xiMax = inputUpperBound[i]
    # Set all indices except i to the mean
    indices = list(range(inputDimension))
    indices.pop(i)
    parametricPCEFunction = meanParametricPCE(chaosResult, indices)
    # Draw the parametric function
    curve = parametricPCEFunction.draw(xiMin, xiMax, npPoints).getDrawable(0)
    curve.setLineWidth(2.0)
    curve.setLineStyle("dashed")
    curve.setLegend(r"$PCE\left(x_i, x_{-i} = \mathbb{E}[X_{-i}]\right)$")
    graph.add(curve)
    # Compute conditional expectation given Xi
    conditionalPCE = chaosResult.getConditionalExpectation([i])
    print(f"i = {i}")
    print(conditionalPCE)
    conditionalPCEFunction = conditionalPCE.getMetaModel()
    curve = conditionalPCEFunction.draw(xiMin, xiMax, npPoints).getDrawable(0)
    curve.setLineWidth(2.0)
    curve.setLegend(r"$\mathbb{E}\left[PCE | X_i = x_i\right]$")
    graph.add(curve)
    if i < inputDimension - 1:
        graph.setLegends([""])
    graph.setColors(palette)
    # Set the graph into the grid
    grid.setGraph(0, i, graph)

grid.setLegendPosition("upper right")
view = otv.View(
    grid,
    figure_kw={"figsize": (8.0, 3.0)},
    legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"},
)
plt.subplots_adjust(wspace=0.4, right=0.7, bottom=0.25)

# %%
# We see that the conditional expectation of the PCE is a better
# approximation of the data set than the parametric PCE.

# %%
# Conclusion
# ~~~~~~~~~~
#
# In this example, we have seen how to compute the conditional
# expectation of a PCE.
# We have seen that this function is a good approximation of the Ishigami
# function when we reduce the input dimension.
# We have also seen that the parametric PCE might be a poor
# approximation of the Ishigami function.
# This is because the parametric PCE depends on the particular value
# that we have chosen to create the parametric function.
#
# The fact that the conditional expectation of the PCE is a
# good approximation of the function when we reduce the input dimension
# is a consequence of a theorem which states that the
# conditional expectation is the best approximation of the
# function in the least squares sense (see [girardin2018]_ page 79).

# %%
otv.View.ShowAll()
