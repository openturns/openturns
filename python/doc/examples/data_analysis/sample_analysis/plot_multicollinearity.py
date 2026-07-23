"""
Compute indices in a multicollinear context
===========================================
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
import matplotlib
import os
from pathlib import Path

# %%
# First, we define a function that computes the desired quantity:


def compute_quantity(X, Y, type):
    """
    Compute a quantity for the given dataset

    Parameters
    - X: the input sample
    - Y: the output sample
    - type: the quantity to compute ("LMG", "PMVD", "Johnson" or "VIF")
    """
    analysis = ot.MulticollinearityAnalysis(X, Y)
    if type == "LMG":
        lmg, _ = analysis.computeLmgPmvd()
        return lmg
    elif type == "PMVD":
        _, pmvd = analysis.computeLmgPmvd()
        return pmvd
    elif type == "Johnson":
        return analysis.computeJohnson()
    elif type == "VIF":
        return analysis.computeVif()
    else:
        raise Exception(f"Invalid type: {type}")

# %%
# Then we add a function that builds a graph:


def create_graph(title, names, mean, interval):
    dimension = mean.getDimension()
    palette = ot.Drawable.BuildDefaultPalette(2)
    graph = ot.Graph(title, "", "")
    matplotlib.rc('xtick', labelsize=8.0)

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
# And a function that computes the quantity, performs a bootstrap and creates a graph:


def compute(X, Y, type, bootstrap_size=100):
    """
    Compute a quantity for the given X and Y samples and print it
    Then perform a bootstrap and display a graph with the computed means and confidence intervals

    Parameters
    - X: the input sample
    - Y: the output sample
    - type: the quantity to compute ("LMG", "PMVD", "Johnson" or "VIF")
    - bootstrap_size: the number of iterations used by the bootstrap
    """
    quantity = compute_quantity(X, Y, type)
    print(quantity)

    # Perform a bootstrap
    sample_size = sample.getSize()
    boot = ot.Sample(bootstrap_size, X.getDimension())
    for i in range(bootstrap_size):
        selection = ot.BootstrapExperiment.GenerateSelection(sample_size, sample_size)
        X_boot = X[selection]
        Y_boot = Y[selection]
        boot[i, :] = compute_quantity(X_boot, Y_boot, type)

    # Create a graph with the mean values and confidence intervals computed from the bootstrap
    mean = boot.computeMean()
    alpha = 0.05
    lb = boot.computeQuantilePerComponent(alpha / 2.0)
    ub = boot.computeQuantilePerComponent(1.0 - alpha / 2.0)
    interval = ot.Interval(lb, ub)
    create_graph(type, X.getDescription(), mean, interval)

# %%
# We can now load our data from a CSV file in order to get the X and Y samples:


try:
    csv = str(Path(os.path.dirname(__file__), "plot_multicollinearity.csv"))
except NameError:
    csv = "plot_multicollinearity.csv"
sample = ot.Sample.ImportFromCSVFile(csv, ";")
Y = sample.getMarginal(0)
X = sample.getMarginal(range(1, sample.getDimension()))

# %%
# Compute the LMG index:

compute(X, Y, "LMG")

# %%
# Compute the PMVD index:

compute(X, Y, "PMVD")

# %%
# Compute the Johnson index:

compute(X, Y, "Johnson")

# %%
# Compute the VIF metric:

compute(X, Y, "VIF")

# %%
# Display all graphs:

otv.View.ShowAll()
