"""
Compute sensitivity indices in a multicollinear context
=======================================================
"""

# %%
# Introduction
# ~~~~~~~~~~~~
#
# In this example, we are interested in the computation of several quantities that are relevant when predictors are correlated:
#
# - LMG index
# - PMVD index
# - Johnson index
# - VIF metric
#
# We will use the :class:`~openturns.MulticollinearityAnalysis` class.

import openturns as ot
import openturns.viewer as otv
from openturns.usecases import ames_housing
import matplotlib

# %%
# We will also perform bootstraps and plot the results, so we start by defining some helper functions.
# First, we define one that computes the desired quantity:


def compute_quantity(X, Y, kind):
    """
    Compute a quantity for the given dataset

    Parameters
    - X: the input sample
    - Y: the output sample
    - kind: the type of quantity to compute ("LMG", "PMVD", "Johnson" or "VIF")
    """
    analysis = ot.MulticollinearityAnalysis(X, Y)
    if kind == "LMG":
        lmg, _ = analysis.computeLmgPmvd()
        return lmg
    elif kind == "PMVD":
        _, pmvd = analysis.computeLmgPmvd()
        return pmvd
    elif kind == "Johnson":
        return analysis.computeJohnson()
    elif kind == "VIF":
        return analysis.computeVIF()
    else:
        raise Exception(f"Invalid kind: {kind}")

# %%
# Then we add a function that builds a graph:


def create_graph(title, names, mean, interval):
    dimension = mean.getDimension()
    palette = ot.Drawable.BuildDefaultPalette(2)
    graph = ot.Graph(title, "", "")
    matplotlib.rc('xtick', labelsize=8.0)  # reduce the font size of the X labels

    # Confidence bounds
    lb = interval.getLowerBound()
    ub = interval.getUpperBound()
    for i in range(dimension):
        curve = ot.Curve([1 + i, 1 + i], [lb[i], ub[i]])
        curve.setLineWidth(2.0)
        curve.setColor(palette[1])
        graph.add(curve)

    # Means
    data = ot.Sample(0, 2)
    for i in range(dimension):
        data.add([i + 1, mean[i]])
    cloud = ot.Cloud(data, palette[0], "circle")
    graph.add(cloud)

    box = graph.getBoundingBox()
    box.setLowerBound([0.8, 0.0])
    graph.setBoundingBox(box)
    axes_kw = {"xticks": range(1, dimension + 1), "xticklabels": names}
    otv.View(graph, axes_kw=axes_kw, figure_kw={"figsize": (10.0, 4.8)})

# %%
# And finally, a function that performs a bootstrap and creates a graph with the results:


def bootstrap(X, Y, kind, alpha=0.95, bootstrap_size=100):
    """
    Perform a bootstrap on the provided sample and create a graph with the means and confidence intervals

    Parameters
    - X: the input sample
    - Y: the output sample
    - kind: the type of quantity to compute ("LMG", "PMVD", "Johnson" or "VIF")
    - alpha: the confidence level
    - bootstrap_size: the number of points in the experiment
    """

    # Perform a bootstrap
    sample_size = X.getSize()
    boot = ot.Sample(bootstrap_size, X.getDimension())
    for i in range(bootstrap_size):
        selection = ot.BootstrapExperiment.GenerateSelection(sample_size, sample_size)
        X_boot = X[selection]
        Y_boot = Y[selection]
        boot[i, :] = compute_quantity(X_boot, Y_boot, kind)

    # Create a graph with the mean values and confidence intervals computed from the bootstrap
    mean = boot.computeMean()
    lb = boot.computeQuantilePerComponent((1.0 - alpha) / 2.0)
    ub = boot.computeQuantilePerComponent(1.0 - (1.0 - alpha) / 2.0)
    interval = ot.Interval(lb, ub)
    create_graph(kind, X.getDescription(), mean, interval)

# %%
# We can now start using our functions. We will take the X and Y samples from the :ref:`Ames Housing<use-case-ames-housing>` dataset
# (which contains data about houses in the city of Ames):


data = ames_housing.AmesHousing().data
Y = data[:, 0]  # Extract the sale price
X = data[:, 1:data.getDimension()]  # Extract the other features

# %%
# LMG and PMVD indices:

analysis = ot.MulticollinearityAnalysis(X, Y)
lmg, pmvd = analysis.computeLmgPmvd()
print(lmg)

# %%
bootstrap(X, Y, "LMG")

# %%
print(pmvd)

# %%
bootstrap(X, Y, "PMVD")

# %%
# Johnson index:

analysis = ot.MulticollinearityAnalysis(X, Y)
johnson = analysis.computeJohnson()
print(johnson)

# %%
bootstrap(X, Y, "Johnson")

# %%
# We notice that the Johnson indices are close to the LMG ones, which was expected (it's a known theoretical result).
#
# VIF metric:

analysis = ot.MulticollinearityAnalysis(X)
vif = analysis.computeVIF()
print(vif)

# %%
bootstrap(X, Y, "VIF")

# %%
# Display all graphs:

otv.View.ShowAll()
