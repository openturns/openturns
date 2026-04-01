"""
Plot enumeration function
-------------------------
"""

# %%
# This example illustrates the enumeration functions which are ijections between :math:`\Nset` into :math:`\Nset^\inputDim`.
# Refer to :ref:`enumeration_strategy` to get the precise description of the enumerate functions. We detail here the bijections:
#
# - Linear enumeration function
# - Hyperbolic enumeration function
# - Anisotropic hyperbolic enumeration function
# - Infinity norm enumeration function
#
# These bijections are used in the in the :ref:`functional chaos expansion setting <functional_chaos>`.
# In order to build up a multivariate basis :math:`\{\psi_{\vect{\alpha}},\vect{\alpha} \in \Nset^\inputDim\}`
# by tensorization of univariate basis terms, we need to enumerate the multi-indices :math:`\vect{\alpha} \in \Nset^\inputDim`.
# In this example, we interprete the impact of the different enumeration functions within the functional chaos expansion setting.

import openturns as ot
import openturns.viewer as otv
import math as m

# %%
# The simplest way to generate the multi-indices is to enumerate the terms of increasing length.
# If the basis is polynomial, then the length corresponds to the total degree of the polynomial.
# Within a strata, the multi-indices are ordered according to the "graded reverse lexicographic
# ordering" in [sullivan2015]_.
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
    graph = ot.Graph("", "$\\alpha_1$", "$\\alpha_2$", True)
    if enum_func.__class__.__name__ == "LinearEnumerateFunction":
        graph.setTitle("Linear enumeration rule")
    elif enum_func.__class__.__name__ == "HyperbolicAnisotropicEnumerateFunction":
        graph.setTitle(f"q={enum_func.getQ()}")
    elif enum_func.__class__.__name__ == "NormInfEnumerateFunction":
        graph.setTitle("Infinity Norm enumeration rule")
    offset = 0
    for strata_index in range(maximum_strata_index):
        strata_cardinal = enum_func.getStrataCardinal(strata_index)
        sample_in_layer = [enum_func(idx + offset) for idx in range(strata_cardinal)]
        offset += strata_cardinal
        cloud = ot.Cloud(sample_in_layer)
        cloud.setLegend(str(strata_index))
        cloud.setPointStyle("circle")
        graph.add(cloud)
    graph.setIntegerXTick(True)
    graph.setIntegerYTick(True)
    graph.setLegendPosition("upper right")
    return graph


# %%
# We consider the dimension 2 and we plot the first strata.
dim = 2
enum_func = ot.LinearEnumerateFunction(dim)
graph = draw_stratas(enum_func, 7)
view = otv.View(graph, axes_kw={"aspect": "equal"})

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
for i in range(len(list_of_dimensions)):
    dimension = list_of_dimensions[i]
    number_of_coeff_array = ot.Sample(degree_maximum, 2)
    for degree in range(1, 1 + degree_maximum):
        number_of_coeff_array[degree - 1, 0] = degree
        number_of_coeff_array[degree - 1, 1] = m.comb(degree + dimension, degree)
    cloud = ot.Cloud(number_of_coeff_array)
    cloud.setPointStyle(point_styles[i])
    cloud.setLegend(f"dim.={dimension}")
    graph.add(cloud)
graph.setLegendPosition("upper left")
graph.setIntegerXTick(True)
graph.setLogScale(ot.GraphImplementation.LOGY)
view = otv.View(graph, figure_kw={"figsize": (5, 4)})

# %%
# The hyperbolic enumeration function is based on the q-norm.
# We plot the hyperbolic quasi norm for different values of :math:`q`.
# With :math:`q = 1` (with isotropy), stratas are hyperplanes:
# we recover the linear enumeration rule.


def draw_qnorm(q):
    def qnorm(x):
        norm = 0.0
        for xi in x:
            norm += xi**q
        norm = norm ** (1.0 / q)
        return [norm]

    f = ot.PythonFunction(2, 1, qnorm)
    f.setInputDescription(["x1", "x2"])
    graph = f.draw([0.0] * 2, [1.0] * 2)
    graph.setTitle(f"q = {q}")
    return graph


dln = ot.ResourceMap.GetAsUnsignedInteger("Contour-DefaultLevelsNumber")
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", 5)
grid = ot.GridLayout(2, 2)
grid.setGraph(0, 0, draw_qnorm(1.0))
grid.setGraph(0, 1, draw_qnorm(0.75))
grid.setGraph(1, 0, draw_qnorm(0.5))
grid.setGraph(1, 1, draw_qnorm(0.25))
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", dln)
grid.setTitle("Hyperbolic quasi norm")
view = otv.View(grid, axes_kw={"aspect": "equal"})

# %%
# We plot the multi-indices of the hyperbolic isotropic enumeration rule by stratas.
# The lower the value of :math:`q` the lower the number of interactions terms in stratas.
grid = ot.GridLayout(2, 2)
grid.setGraph(0, 0, draw_stratas(ot.HyperbolicAnisotropicEnumerateFunction(dim, 1.0), 7))
grid.setGraph(0, 1, draw_stratas(ot.HyperbolicAnisotropicEnumerateFunction(dim, 0.75), 7))
grid.setGraph(1, 0, draw_stratas(ot.HyperbolicAnisotropicEnumerateFunction(dim, 0.5), 7))
grid.setGraph(1, 1, draw_stratas(ot.HyperbolicAnisotropicEnumerateFunction(dim, 0.25), 7))
grid.setTitle("Hyperbolic rule")
view = otv.View(grid, axes_kw={"aspect": "equal"})

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
# in dimension :math:`d = 5` for several :math:`q` -norm values.
# We observe that the gap between high and low values of :math:`q` can lead to a gap
# in the numbers of terms of an order of magnitude.
dim = 5
graph = ot.Graph(
    "Hyperbolic enumeration. dim. = %d" % (dim),
    "Total degree",
    "Number of coefficients",
    True,
)
degree_maximum = 10
q_list = [0.2, 0.4, 0.6, 0.8, 1.0]
point_styles = ["bullet", "circle", "fdiamond", "fsquare", "triangleup"]
for i in range(len(q_list)):
    q = q_list[i]
    enum_func = ot.HyperbolicAnisotropicEnumerateFunction(dim, q)
    number_of_coeff_array = ot.Sample(degree_maximum, 2)
    for p in range(1, 1 + degree_maximum):
        number_of_coeff_array[p - 1, 0] = p
        number_of_coeff_array[p - 1, 1] = enum_func.getMaximumDegreeCardinal(p)
    cloud = ot.Cloud(number_of_coeff_array)
    cloud.setPointStyle(point_styles[i])
    cloud.setLegend(f"$q={q}$")
    graph.add(cloud)
graph.setLegendPosition("upper left")
graph.setIntegerXTick(True)
graph.setLogScale(ot.GraphImplementation.LOGY)
view = otv.View(graph, figure_kw={"figsize": (5, 4)})

# %%
# We plot the multi-indices of the hyperbolic anisotropic enumeration rule by stratas.
# This enumerate function emphasizes multi-indices whose components are larger
# when the associated weights are smaller.
grid = ot.GridLayout(2, 2)
weights = [0.4, 0.6]
maximum_strata_index = 14
grid.setGraph(0, 0, draw_stratas(ot.HyperbolicAnisotropicEnumerateFunction(weights, 1.0), maximum_strata_index))
grid.setGraph(0, 1, draw_stratas(ot.HyperbolicAnisotropicEnumerateFunction(weights, 0.7), maximum_strata_index))
grid.setGraph(1, 0, draw_stratas(ot.HyperbolicAnisotropicEnumerateFunction(weights, 0.5), maximum_strata_index))
grid.setGraph(1, 1, draw_stratas(ot.HyperbolicAnisotropicEnumerateFunction(weights, 0.25), maximum_strata_index))
grid.setTitle("Hyperbolic anisotropic rule, weights = [0.4, 0.6]")
view = otv.View(grid, axes_kw={"aspect": "equal"})

# %%
# Now we use the infinity norm enumeration function. We illustrate the enumeration in dimension 2.
# We plot the first stratas.
enum_func = ot.NormInfEnumerateFunction(2)
graph = draw_stratas(enum_func, 7)
view = otv.View(graph, axes_kw={"aspect": "equal"})

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
