"""
Plot enumeration function
-------------------------
"""

# %%
# This example illustrates the enumeration functions which are bijections between :math:`\Nset` into :math:`\Nset^\inputDim`.
# Refer to :ref:`enumeration_strategy` to get the precise description of the enumerate functions. We detail here the bijections:
#
# - linear enumeration function;
# - hyperbolic enumeration function;
# - anisotropic hyperbolic enumeration function;
# - infinity norm enumeration function.
#
# These bijections are used in the :ref:`functional chaos expansion setting <functional_chaos>`.
# In order to build up a multivariate basis :math:`\{\psi_{\vect{\alpha}},\vect{\alpha} \in \Nset^\inputDim\}`
# by tensorization of univariate basis terms, we need to enumerate the multi-indices :math:`\vect{\alpha} \in \Nset^\inputDim`.
# In this example, we interprete the impact of the different enumeration functions within the functional chaos expansion setting.

import openturns as ot
import openturns.viewer as otv
import math as m
import matplotlib.colors as mcolors
import matplotlib.pyplot as plt

# %%
# The simplest way to generate multi-indices is to list them by increasing length.
# If the basis is polynomial, then the length corresponds to the total degree of the polynomial.
# Within a strata, the multi-indices are ordered according to the "graded reverse lexicographic
# ordering" (see [sullivan2015]_).
# This is named the linear enumeration rule in the library.
#
# We print the ordered elements in dimension 4 to illustrate the ordering of the multi-indices.
# We only consider the 4 first strata and we print their associated length
# (which are equal to 0 up to 3).
dim = 4
enum_func = ot.LinearEnumerateFunction(dim)
td_prev = 0
print("#  | multi-index     | length")
print("---+-----------------+-------------")
i_max = enum_func.getStrataCumulatedCardinal(3)
for i in range(i_max):
    multi_index = enum_func(i)
    td = sum(multi_index)
    if td > td_prev:
        td_prev = td
        print("---+-----------------+-------------")
    print(f"{i:2} |       {multi_index} |           {td}")

# %%
# We define a function that plots the successive stratas with different colors.


def draw_stratas(enum_func, maximum_strata_index):
    """
    Plot enumeration rule by stratas.

    Parameters
    ----------
    enum_func : openturns.EnumerateFunction
        Enumerate function

    Returns
    -------
    graph : openturns.Graph
        Plot of the multi-indices colored by stratas
    """
    if maximum_strata_index < 1:
        raise ValueError(
            f"maximum_strata_index must be >= 1, got {maximum_strata_index}"
        )
    cmap = plt.get_cmap("viridis")
    if maximum_strata_index == 1:
        viridis_palette = [mcolors.to_hex(cmap(0.0))]
    else:
        viridis_palette = [
            mcolors.to_hex(cmap(i / (maximum_strata_index - 1)))
            for i in range(maximum_strata_index)
        ]
    graph = ot.Graph("", "$\\alpha_1$", "$\\alpha_2$", True)
    if isinstance(enum_func, ot.LinearEnumerateFunction):
        graph.setTitle("Linear enumeration rule")
    elif isinstance(enum_func, ot.HyperbolicAnisotropicEnumerateFunction):
        graph.setTitle(f"q={enum_func.getQ()}")
    elif isinstance(enum_func, ot.NormInfEnumerateFunction):
        graph.setTitle("Infinity Norm enumeration rule")
    offset = 0
    for strata_index in range(maximum_strata_index):
        strata_cardinal = enum_func.getStrataCardinal(strata_index)
        sample_in_layer = [enum_func(idx + offset) for idx in range(strata_cardinal)]
        offset += strata_cardinal
        cloud = ot.Cloud(sample_in_layer)
        cloud.setLegend(str(strata_index))
        cloud.setPointStyle("circle")
        cloud.setColor(viridis_palette[strata_index])
        graph.add(cloud)
    graph.setIntegerXTick(True)
    graph.setIntegerYTick(True)
    graph.setLegendPosition("upper left")
    graph.setLegendCorner((1.0, 1.0))
    return graph


# %%
# We consider the dimension 2 and we plot the first strata.
dim = 2
enum_func = ot.LinearEnumerateFunction(dim)
graph = draw_stratas(enum_func, 7)
_ = otv.View(graph, axes_kw={"aspect": "equal"}, figure_kw={"figsize": (5, 4)})

# %%
# When the number of input dimensions of a functional chaos expansion (FCE) increases,
# each multi-index corresponds to a coefficient in the expansion.
# Hence, the number of multi-indices represents the number of coefficients in the FCE.
# We plot the number of terms in the basis depending on the maximum total length
# for several dimension values.
# We observe the exponential increase of the number of terms with the dimension
# :math:`\inputDim` (curse of dimensionality).
graph = ot.Graph("Linear enumeration", "Total degree", "Number of coefficients")
degree_maximum = 10
list_of_dimensions = [1, 5, 10, 15, 20]
point_styles = ["bullet", "circle", "fdiamond", "fsquare", "triangleup"]
for i, dimension in enumerate(list_of_dimensions):
    data = [
        [degree, m.comb(degree + dimension, degree)]
        for degree in range(1, 1 + degree_maximum)
    ]
    number_of_coeff_array = ot.Sample(data)
    cloud = ot.Cloud(number_of_coeff_array)
    cloud.setPointStyle(point_styles[i])
    cloud.setLegend(f"dim.={dimension}")
    graph.add(cloud)
graph.setLegendPosition("upper left")
graph.setIntegerXTick(True)
graph.setLogScale(ot.GraphImplementation.LOGY)
_ = otv.View(graph, figure_kw={"figsize": (5, 4)})

# %%


def build_q_quasinorm_function(weights, q):
    """Returns a parametric function representing the $q$-quasi-norm of a
    two-dimensional vector weighted by the provided coefficients."""
    if any(w <= 0 for w in weights):
        raise ValueError("All weights must be strictly positive.")
    if len(weights) != 2:
        raise ValueError(f"The number of weights should be 2, but is {len(weights)}")
    if q <= 0.0:
        raise ValueError(f"The parameter q should be greater than 0, but is {q}")
    q_norm_function = ot.SymbolicFunction(
        ["x1", "x2", "w1", "w2", "q"], ["((w1 * x1)^q + (w2 * x2)^q)^(1 / q)"]
    )
    q_norm_parametric = ot.ParametricFunction(
        q_norm_function, [2, 3, 4], [weights[0], weights[1], q]
    )
    return q_norm_parametric


# %%
# The hyperbolic enumeration function is based on the q-quasi-norm.
# This function is a quasi-norm for :math:`q \in (0,1)`
# and is a norm for :math:`q \geq 1`.
# We plot the hyperbolic quasi norm for different values of :math:`q`.
# With :math:`q = 1` (with isotropy), stratas are hyperplanes:
# we recover the linear enumeration rule.


def draw_q_quasinorm(q):
    """Generates a contour plot of the Lq-quasi-norm for a given exponent q in a
    two-dimensional space."""
    weights = [1.0, 1.0]
    f = build_q_quasinorm_function(weights, q)
    graph = f.draw([0.0] * 2, [1.0] * 2)
    graph.setTitle(f"q = {q}")
    graph.setXTitle("$x_1$")
    graph.setYTitle("$x_2$")
    return graph


dln = ot.ResourceMap.GetAsUnsignedInteger("Contour-DefaultLevelsNumber")
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", 5)
grid = ot.GridLayout(2, 2)
grid.setGraph(0, 0, draw_q_quasinorm(1.0))
grid.setGraph(0, 1, draw_q_quasinorm(0.75))
grid.setGraph(1, 0, draw_q_quasinorm(0.5))
grid.setGraph(1, 1, draw_q_quasinorm(2.0))
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", dln)
grid.setTitle("Hyperbolic quasi norm")
_ = otv.View(grid, axes_kw={"aspect": "equal"}, figure_kw={"figsize": (5, 4)})
plt.subplots_adjust(wspace=0.7, hspace=0.7)


# %%
def drawHyperbolicStratas(dim, q, maximum_strata_index=7):
    """Generates a plot of the **isotropic** hyperbolic enumeration function
    strata up to a specified maximum index."""
    enumerateFunction = ot.HyperbolicAnisotropicEnumerateFunction(dim, q)
    return draw_stratas(enumerateFunction, maximum_strata_index)


# %%
# We plot the multi-indices of the hyperbolic isotropic enumeration rule by stratas.
# The lower the value of :math:`q` the lower the number of interactions terms in stratas.
grid = ot.GridLayout(2, 2)
grid.setGraph(0, 0, drawHyperbolicStratas(dim, 1.0))
grid.setGraph(0, 1, drawHyperbolicStratas(dim, 0.75))
grid.setGraph(1, 0, drawHyperbolicStratas(dim, 0.5))
grid.setGraph(1, 1, drawHyperbolicStratas(dim, 2.0))
grid.setTitle("Hyperbolic rule")
_ = otv.View(grid, axes_kw={"aspect": "equal"}, figure_kw={"figsize": (6, 5)})
plt.subplots_adjust(wspace=0.7, hspace=0.7)

# %%
# Interaction multi-indices are presented in the center of the :math:`(\alpha_1, \alpha_2)` space.
# We see that when the quasi-norm parameter is close to zero, the enumeration rule
# shows less interaction multi-indices.
# Instead, multi-indices close to the axes represent multivariate polynomials
# with high marginal degrees.
# When :math:`q` is close to zero, these polynomials with high marginal degrees appear
# sooner with the hyperbolic enumeration rule.

# %%
# We plot the number of terms in the basis depending on the maximum total degree
# in dimension :math:`d = 5` for several :math:`q`-quasi-norm values.
# We observe that the gap between high and low values of :math:`q` can lead to a gap
# in the numbers of terms of an order of magnitude.
dim = 5
graph = ot.Graph(
    f"Hyperbolic enumeration. dim. = {dim}",
    "Total degree",
    "Number of coefficients",
    True,
)
degree_maximum = 10
q_list = [0.2, 0.4, 0.6, 0.8, 1.0]
point_styles = ["bullet", "circle", "fdiamond", "fsquare", "triangleup"]
for i, q in enumerate(q_list):
    enum_func = ot.HyperbolicAnisotropicEnumerateFunction(dim, q)
    data = [
        [p, enum_func.getMaximumDegreeCardinal(p)] for p in range(1, 1 + degree_maximum)
    ]
    cloud = ot.Cloud(data)
    cloud.setPointStyle(point_styles[i])
    cloud.setLegend(f"$q={q}$")
    graph.add(cloud)
graph.setLegendPosition("upper left")
graph.setIntegerXTick(True)
graph.setLogScale(ot.GraphImplementation.LOGY)
_ = otv.View(graph, figure_kw={"figsize": (5, 4)})


# %%
def drawAnisotropicHyperbolicStratas(weights, q, maximum_strata_index=14):
    """Generates a plot of the anisotropic hyperbolic enumeration function
    strata up to a specified maximum index."""
    enumerateFunction = ot.HyperbolicAnisotropicEnumerateFunction(weights, q)
    return draw_stratas(enumerateFunction, maximum_strata_index)


# %%
# We plot the multi-indices of the hyperbolic anisotropic enumeration rule by stratas.
# This enumerate function emphasizes multi-indices whose components are larger
# when the associated weights are smaller.
grid = ot.GridLayout(2, 2)
weights = [0.4, 0.6]
grid.setGraph(0, 0, drawAnisotropicHyperbolicStratas(weights, 1.0))
grid.setGraph(0, 1, drawAnisotropicHyperbolicStratas(weights, 0.7))
grid.setGraph(1, 0, drawAnisotropicHyperbolicStratas(weights, 0.5))
grid.setGraph(1, 1, drawAnisotropicHyperbolicStratas(weights, 0.25))
grid.setTitle("Hyperbolic anisotropic rule, weights = [0.4, 0.6]")
_ = otv.View(grid, axes_kw={"aspect": "equal"}, figure_kw={"figsize": (7, 7)})
plt.subplots_adjust(wspace=0.7, hspace=0.7)

# %%
# Now we use the infinity norm enumeration function. We illustrate the enumeration in dimension 2.
# We plot the first stratas.
enum_func = ot.NormInfEnumerateFunction(2)
graph = draw_stratas(enum_func, 7)
_ = otv.View(graph, axes_kw={"aspect": "equal"}, figure_kw={"figsize": (5, 4)})

# %%
# We print the 3 first stratas in dimension 3.
print("#  | multi-index ")
print("---+-------------")
enum_func = ot.NormInfEnumerateFunction(3)
i_max = enum_func.getStrataCumulatedCardinal(3)
for i in range(i_max):
    multi_index = enum_func(i)
    print(f"{i:2} |       {multi_index}")

# %%
# Display the graphs.
otv.View.ShowAll()
