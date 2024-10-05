"""
Conditional expectation of a polynomial chaos expansion
=======================================================
"""
# %%
#
# In this example, we compute the conditional expectation of a polynomial
# chaos expansion of the :ref:`Ishigami function <use-case-ishigami>` using
# the :meth:`~openturns.FunctionalChaosResult.getConditionalExpectation`
# method.

# %%
# Introduction
# ~~~~~~~~~~~~
# Let :math:`\inputDim \in \Nset`
# be the dimension of the input random vector.
# Let :math:`\Expect{\inputRV} \in \Rset^\inputDim`
# be the mean of the input random vector :math:`\inputRV`.
# Let :math:`\model` be the physical model:
#
# .. math::
#     \model : \Rset^\inputDim \rightarrow \Rset.
#
# Given :math:`\vect{u} \subseteq \{1, ..., \inputDim\}` a group
# of input variables, we want to create a new function :math:`\widehat{\model}`:
#
# .. math::
#     \widehat{\model}: \Rset^{|\vect{u}|} \rightarrow \Rset
#
# where :math:`|\vect{u}| = \operatorname{card}(\vect{u})` is the
# number of variables in the group.
#
# In this example, we experiment two different ways to reduce the
# input dimension of a polynomial chaos expansion:
#
# - the parametric function,
# - the conditional expectation.
#
# The goal of this page is to see how we can create these
# functions and the difference between them.

# %%
# Parametric function
# ~~~~~~~~~~~~~~~~~~~
#
# The simplest method to reduce the dimension of the input
# is to set some input variables to constant values.
# In this example, all marginal inputs, except those in
# the conditioning indices are set to the mean of the input random vector.
#
# Let :math:`\overline{\vect{u}}` be the complementary set of input
# marginal indices such that :math:`\vect{u}` and :math:`\overline{\vect{u}}`
# form a disjoint partition of the full set of variable indices:
#
# .. math::
#
#     \vect{u} \; \dot{\cup} \; \overline{\vect{u}} = \{1, ..., \inputDim\}.
#
# The parametric function with reduced dimension is:
#
# .. math::
#
#   \widehat{\model}(\inputReal_{\vect{u}})
#   = \model\left(\inputReal_{\vect{u}},
#            \inputReal_{\overline{\vect{u}}}
#            = \Expect{\inputRV_{\overline{\vect{u}}}}\right)
#
# for any :math:`\inputReal_{\vect{u}} \in \Rset^{|\vect{u}|}`.
# The previous function is a parametric function based on the function :math:`\model`
# where the parameter is :math:`\Expect{\inputRV_{\overline{\vect{u}}}}`.
# Assuming that the input random vector has an independent copula,
# computing :math:`\Expect{\inputRV_{\overline{\vect{u}}}}`
# can be done by selecting the corresponding indices in :math:`\Expect{\inputRV}`.
# This function can be created using the :class:`~openturns.ParametricFunction`
# class.

# %%
# Parametric PCE
# ~~~~~~~~~~~~~~
#
# If the physical model is a PCE, then the associated parametric model is also a
# PCE.
# Its coefficients and the associated functional basis can be computed from
# the original PCE.
# A significant fact, however, is that the coefficients of the parametric
# PCE are *not* the ones of the original PCE: the coefficients of the parametric
# PCE have to be multiplied by factors which depend on the
# value of the discarded basis functions on the parameter vector.
# This feature is not currently available in the library.
# However, we present it below as this derivation is interesting
# to understand why the conditional expectation may behave
# differently from the corresponding parametric PCE.

# %%
# Let :math:`\cJ^P \subseteq \Nset^{\inputDim}` be the set of
# multi-indices corresponding to the truncated polynomial chaos expansion
# up to the :math:`P`-th coefficient.
# Let :math:`h` be the PCE in the standard space:
#
# .. math::
#
#     h(\standardReal) = \sum_{\vect{\alpha} \in \cJ^P}
#         a_{\vect{\alpha}} \psi_{\vect{\alpha}}(\standardReal).
#
# Let :math:`\vect{u} \subseteq \{1, ..., \inputDim\}` be a group of variables,
# let :math:`\overline{\vect{u}}` be its complementary set such that
#
# .. math::
#
#     \vect{u} \; \dot{\cup} \; \overline{\vect{u}} = \{1, ..., \inputDim\}
#
# i.e. the groups :math:`\vect{u}` and :math:`\overline{\vect{u}}` create a disjoint partition
# of the set :math:`\{1, ..., \inputDim\}`.
# Let :math:`|\vect{u}| \in \Nset` be the number of elements
# in the group :math:`\vect{u}`.
# Hence, we have :math:`|\vect{u}| + |\overline{\vect{u}}| = \inputDim`.

# %%
# Let :math:`\standardReal_{\vect{u}}^{(0)} \in \Rset^{|\vect{u}|}`
# be a given point.
# We are interested in the function :
#
# .. math::
#
#     \widehat{h}(\standardReal_{\overline{\vect{u}}})
#     = h\left(\standardReal_{\overline{\vect{u}}},
#     \standardReal_{\vect{u}}^{(0)}\right)
#
# for any :math:`\standardReal_{\overline{\vect{u}}} \in \Rset^{|\overline{\vect{u}}|}`.
# We assume that the polynomial basis are defined by the tensor product:
#
# .. math::
#
#     \psi_{\vect{\alpha}}\left(\standardReal\right)
#     = \prod_{i = 1}^{\inputDim}
#     \pi_{\alpha_i}^{(i)}\left(\standardReal\right)
#
# for any :math:`\standardReal \in \standardInputSpace`
# where :math:`\pi_{\alpha_i}^{(i)}` is the polynomial of degree
# :math:`\alpha_i` of the :math:`i`-th input standard variable.
#
# Let :math:`\vect{u} = (u_i)_{i = 1, ..., |\vect{u}|}` denote the components of the
# group :math:`\vect{u}` where :math:`|\vect{u}|` is the number of elements in the group.
# Similarly, let :math:`\overline{\vect{u}} = (\overline{u}_i)_{i = 1, ..., |\overline{\vect{u}}|}` denote the
# components of the complementary group :math:`\overline{\vect{u}}`.
# The components of :math:`\standardReal \in \Rset^{\inputDim}`
# which are in the group :math:`\vect{u}` are :math:`\left(z_{u_i}^{(0)}\right)_{i = 1, ..., |\vect{u}|}`
# and the complementary components are
# :math:`\left(z_{\overline{u}_i}\right)_{i = 1, ..., |\overline{\vect{u}}|}`.

# %%
# Let :math:`\overline{\psi}_{\overline{\vect{\alpha}}}` be the reduced polynomial:
#
# .. math::
#    :label: PCE_CE_1
#
#     \overline{\psi}_{\overline{\vect{\alpha}}}(z_{\overline{\vect{u}}})
#     = \left(\prod_{i = 1}^{|\overline{\vect{u}}|}
#        \pi_{\alpha_{\overline{u}_i}}^{(\overline{u}_i)}
#        \left(\standardReal_{\overline{u}_i}\right) \right)
#
# where :math:`\overline{\vect{\alpha}} \in \Nset^{|\vect{u}|}` is the reduced multi-index
# defined from the multi-index :math:`\vect{\alpha}\in \Nset^{\inputDim}`
# by the equation:
#
# .. math::
#
#     \overline{\alpha}_i = \alpha_{\overline{u}_i}
#
# for :math:`i = 1, ..., |\overline{\vect{u}}|`.
# The components of the reduced multi-index :math:`\overline{\vect{\alpha}}` which corresponds
# to the components of the multi-index given by the complementary group :math:`|\vect{u}|`.

# %%
# We must then gather the reduced multi-indices.
# Let :math:`\overline{\cJ}^P` be the set of unique reduced multi-indices:
#
# .. math::
#    :label: PCE_CE_2
#
#     \overline{\cJ}^P = \left\{\overline{\vect{\alpha}} \in \Nset^{|\vect{u}|}
#     \; | \; \vect{\alpha} \in \cJ^P\right\}.
#
# For any reduced multi-index :math:`\overline{\vect{\alpha}} \in \overline{\cJ}^P`
# of dimension :math:`|\overline{\vect{u}}|`,
# we note :math:`\cJ_{\overline{\vect{\alpha}}}^P`
# the set of corresponding (un-reduced) multi-indices of
# dimension :math:`\inputDim`:
#
# .. math::
#    :label: PCE_CE_3
#
#     \cJ_{\overline{\vect{\alpha}}}^P
#     = \left\{\vect{\alpha} \in \cJ^P \; |\; \overline{\alpha}_i
#     = \alpha_{\overline{u}_i}, \; i = 1, ..., |\overline{\vect{u}}|\right\}.
#
# Each aggregated coefficient :math:`\overline{a}_{\overline{\vect{\alpha}}} \in \Rset`
# is defined by the equation:
#
# .. math::
#    :label: PCE_CE_5
#
#     \overline{a}_{\overline{\vect{\alpha}}}
#     = \sum_{\vect{\alpha} \in \cJ^P_{\overline{\vect{\alpha}}}}
#     a_{\vect{\alpha}} \left(\prod_{i = 1}^{|\vect{u}|}
#     \pi_{\alpha_{u_i}}^{(u_i)}\left(\standardReal_{u_i}^{(0)}\right) \right).
#
# Finally:
#
# .. math::
#    :label: PCE_CE_4
#
#     \widehat{h}(\standardReal_{\overline{\vect{u}}})
#     = \sum_{\overline{\vect{\alpha}} \in \overline{\cJ}^P}
#     \overline{a}_{\overline{\vect{\alpha}}}
#     \overline{\psi}(z_{\overline{\vect{u}}})
#
# for any :math:`\standardReal_{\overline{\vect{u}}} \in \Rset^{|\overline{\vect{u}}|}`.

# %%
# The method is the following.
#
# - Create the reduced polynomial basis from equation :eq:`PCE_CE_1`.
# - Create the list of reduced multi-indices from the equation :eq:`PCE_CE_2`, and, for each
#   reduced multi-index, the list of corresponding multi-indices from the equation :eq:`PCE_CE_3`.
# - Aggregate the coefficients from the equation :eq:`PCE_CE_5`.
# - The parametric PCE is defined by the equation :eq:`PCE_CE_4`.

# %%
# Conditional expectation
# ~~~~~~~~~~~~~~~~~~~~~~~
#
# One method to reduce the input dimension of a function is to
# consider its conditional expectation.
# The conditional expectation function is:
#
# .. math::
#
#   \widehat{\model}(\inputReal_{\vect{u}})
#   = \Expect{\model(\inputReal)
#            \; | \; \inputRV_{\vect{u}}
#            = \inputReal_{\vect{u}}}
#
# for any :math:`\inputReal_{\vect{u}} \in \Rset^{|\vect{u}|}`.
# In general, there is no dedicated method to create such a conditional expectation
# in the library.
# We can, however, efficiently compute the conditional expectation of a polynomial
# chaos expansion.
# In turn, this conditional chaos expansion (PCE) is a polynomial chaos expansion
# which can be computed using the :meth:`~openturns.FunctionalChaosResult.getConditionalExpectation`
# method from the :class:`~openturns.FunctionalChaosResult` class.

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
    inputDistribution = chaosResult.getDistribution()
    if not inputDistribution.hasIndependentCopula():
        raise ValueError(
            "The input distribution has a copula" "which is not independent"
        )
    # Create the parametric function
    pceFunction = chaosResult.getMetaModel()
    xMean = inputDistribution.getMean()
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
# We choose a sample size equal to 1000.

# %%
ot.Log.Show(ot.Log.NONE)
ot.RandomGenerator.SetSeed(0)
im = ishigami_function.IshigamiModel()
input_names = im.inputDistribution.getDescription()
sampleSize = 1000
inputSample = im.inputDistribution.getSample(sampleSize)
outputSample = im.model(inputSample)


# %%
# We then create a sparce PCE of the Ishigami function using
# a candidate basis up to the total degree equal to 12.
# This leads to 455 candidate coefficients.
# The coefficients are computed from least squares.

# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory([im.X1, im.X2, im.X3])
totalDegree = 12
enumerateFunction = multivariateBasis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
print("Basis size = ", basisSize)

# %%
# Finally, we create the PCE.
# Only 61 coefficients are selected by the :class:`~openturns.LARS`
# algorithm.

# %%
chaosResult = computeSparseLeastSquaresFunctionalChaos(
    inputSample,
    outputSample,
    multivariateBasis,
    basisSize,
    im.inputDistribution,
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
# For each marginal index `i`, we we plot the output :math:`Y`
# against the input marginal :math:`X_i` of the sample.
# Then we plot the parametric function depending on :math:`X_i`.

# %%
inputDimension = im.inputDistribution.getDimension()
npPoints = 100
inputRange = im.inputDistribution.getRange()
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

grid.setLegendPosition("topright")
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
inputDimension = im.inputDistribution.getDimension()
npPoints = 100
inputRange = im.inputDistribution.getRange()
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

grid.setLegendPosition("topright")
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
