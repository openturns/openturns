"""
Calibration of a model with 2D output
=====================================

In this example we present the calibration of a model with 2 output variables.
It is similar to the :any:`calibration of the logistic model <plot_calibration_logistic>` example,
but here we want to predict the growth of 2 populations (US and FR) instead of just one.
"""

# %%
# First, we define a number of functions that we will need later.
import openturns as ot
import openturns.viewer as otv
import matplotlib.pyplot as pl
from math import exp, sqrt


def RMSE(population_observations, population_predicted):
    residual = population_observations - population_predicted
    dimension = population_observations.getDimension()
    SS = 0.0
    for i in range(dimension):
        marginal_residual = residual[:, i].asPoint()
        SS += marginal_residual.normSquare()
    size = population_observations.getSize()
    mse = SS / (dimension * size)
    rmse = sqrt(mse)
    return rmse


def plot_calibrated_prediction(
    sample_US,
    sample_FR,
    time_observations,
    population_predicted_before,
    population_predicted_after,
    method
):
    palette = ot.DrawableImplementation.BuildDefaultPalette(2)

    graph2 = ot.Graph("", "Time (years)", "Population (millions)", True, "topleft")
    # Observations
    cloud = ot.Cloud(sample_US)
    cloud.setColor(palette[0])
    cloud.setLegend("Observations (US)")
    graph2.add(cloud)

    cloud = ot.Cloud(sample_FR)
    cloud.setColor(palette[1])
    cloud.setLegend("Observations (FR)")
    graph2.add(cloud)

    # Predictions before calibration
    cloud = ot.Curve(time_observations, population_predicted_before[:, 0])
    cloud.setLegend("Before (US)")
    cloud.setColor(palette[0])
    cloud.setLineStyle("dashed")
    graph2.add(cloud)
    cloud = ot.Curve(time_observations, population_predicted_before[:, 1])
    cloud.setLegend("Before (FR)")
    cloud.setColor(palette[1])
    cloud.setLineStyle("dashed")
    graph2.add(cloud)

    # Predictions after calibration
    cloud = ot.Curve(time_observations, population_predicted_after[:, 0])
    cloud.setLegend("After (US)")
    cloud.setColor(palette[0])
    graph2.add(cloud)
    cloud = ot.Curve(time_observations, population_predicted_after[:, 1])
    cloud.setLegend("After (FR)")
    cloud.setColor(palette[1])
    graph2.add(cloud)

    rmse = RMSE(population_observations, population_predicted_after)
    graph2.setTitle(f"{method} (RMSE = {rmse:.1f})")
    return graph2


def plot_model_vs_data(sample_US, sample_FR, theta, model, time_observations):

    palette = ot.DrawableImplementation.BuildDefaultPalette(2)

    graph2 = ot.Graph("", "Time (years)", "Population (millions)", True, "topleft")
    # Observations
    cloud = ot.Cloud(sample_US)
    cloud.setLegend("Observations (US)")
    cloud.setColor(palette[0])
    graph2.add(cloud)

    cloud = ot.Cloud(sample_FR)
    cloud.setLegend("Observations (FR)")
    cloud.setColor(palette[1])
    graph2.add(cloud)

    # Predictions
    model.setParameter(theta)
    population_predicted = model(time_observations)
    cloud = ot.Curve(time_observations, population_predicted[:, 0])
    cloud.setLegend("Predictions (US)")
    cloud.setColor(palette[0])
    graph2.add(cloud)

    cloud = ot.Curve(time_observations, population_predicted[:, 1])
    cloud.setLegend("Predictions (FR)")
    cloud.setColor(palette[1])
    graph2.add(cloud)
    return graph2


def decorateParameterDistribution(grid):
    nbcols = grid.getNbColumns()
    for i in range(nbcols):
        graph = grid.getGraph(0, i)
        if i == nbcols - 1:
            graph.setLegends(["PDF", "Posterior", "Candidate"])
        else:
            graph.setLegends([""])
        if i > 0:
            graph.setYTitle("")
        grid.setGraph(0, i, graph)
    return grid


def decorateResiduals(grid):
    graph = grid.getGraph(0, 0)
    graph.setLegends([""])
    graph.setXTitle(r"$\epsilon_{US}$")
    graph.setTitle("")
    grid.setGraph(0, 0, graph)
    graph = grid.getGraph(0, 1)
    graph.setXTitle(r"$\epsilon_{FR}$")
    graph.setYTitle("")
    graph.setTitle("")
    graph.setLegends(["Initial", "Calibrated", "Obs. err."])
    grid.setGraph(0, 1, graph)
    return grid


def printRMSEValue(calibrationResult, theta_prior):
    residualFunction = calibrationResult.getResidualFunction()
    residualsBefore = residualFunction(theta_prior)
    dimension = residualFunction.getOutputDimension()
    rmseBefore = sqrt(residualsBefore.normSquare() / dimension)
    print(f"RMSE before = {rmseBefore:.2f}")
    thetaMAP = calibrationResult.getParameterMAP()
    residualsAfter = residualFunction(thetaMAP)
    rmseAfter = sqrt(residualsAfter.normSquare() / dimension)
    print(f"RMSE after = {rmseAfter:.2f}")


# %%
# Define the observations
# -----------------------
# We create a :class:`~openturns.Sample` with time, US and FR populations.

data = ot.Sample(
    [
        [1790, 3.9, 28.1],
        [1800, 5.3, 29.4],
        [1810, 7.2, 30.0],
        [1820, 9.6, 31.6],
        [1830, 13, 33.6],
        [1840, 17, 34.9],
        [1850, 23, 36.5],
        [1860, 31, 37.4],
        [1870, 39, 37.7],
        [1880, 50, 39.2],
        [1890, 62, 39.9],
        [1900, 76, 40.7],
        [1910, 92, 41.4],
        [1920, 106, 38.9],
        [1930, 123, 41.3],
        [1940, 132, 40.7],
        [1950, 151, 41.6],
        [1960, 179, 45.5],
        [1970, 203, 50.5],
        [1980, 221, 53.7],
        [1990, 250, 56.6],
        [2000, 281, 58.9],
    ]
)
data.setDescription(["Time", "US Population", "FR Population"])

# Extract the years
time_observations = data.getMarginal(0)

# Extract the US and FR populations
population_observations = data.getMarginal([1, 2])
data_US = data.getMarginal([0, 1])
data_FR = data.getMarginal([0, 2])

# %%
# Plot the populations.

graph = ot.Graph("", "Time (years)", "Population (millions)", True, "topleft")
cloud = ot.Cloud(data_US)
cloud.setLegend("Observations (US)")
graph.add(cloud)
cloud = ot.Cloud(data_FR)
cloud.setLegend("Observations (FR)")
graph.add(cloud)
graph.setColors(ot.DrawableImplementation.BuildDefaultPalette(2))
view = otv.View(graph)


# %%
# Create the model
# ----------------
# We use a logistic model to predict the populations.

def logistic_population(t, t0, y0, a, b, m):
    y = m + a * y0 / (b * y0 + (a - b * y0) * exp(-a * (t - t0)))
    return y


def logistic_model(X):
    t = X[0]
    a_US = X[1]
    c_US = X[2]
    a_FR = X[3]
    c_FR = X[4]
    m = X[5]

    t0 = 1790.0
    y0_US = 3.9e6
    y0_FR = 28.1e6
    b_US = exp(c_US)
    b_FR = exp(c_FR)

    scaled_m = m * 1.0e6

    # Return a 2D point with US and FR populations
    y = ot.Point(2)
    y[0] = logistic_population(t, t0, y0_US, a_US, b_US, scaled_m)
    y[1] = logistic_population(t, t0, y0_FR, a_FR, b_FR, scaled_m)
    z = y / 1.0e6  # Convert to millions

    return z


# Prior parameters
a_US = 0.03134
c_US = -22.58
a_FR = 0.005
c_FR = -40.0
m = 1.0
theta_prior = [a_US, c_US, a_FR, c_FR, m]

# Define the physical model (takes 6 input variables and outputs 2 variables)
g = ot.PythonFunction(6, 2, logistic_model)
g.setInputDescription(["$t$", "$a_{US}$", "$c_{US}$", "$a_{FR}$", "$c_{FR}$", "$m$"])

# Define parametric functions from the physical model
model = ot.ParametricFunction(g, [1, 2, 3, 4, 5], theta_prior)
calibration_model = ot.ParametricFunction(g, [0], [0.0])

# Predictions before calibration
population_predicted_before = model(time_observations)

# %%
# Plot the predictions before calibration.

graph = plot_model_vs_data(
    data_US,
    data_FR,
    theta_prior,
    model,
    time_observations,
)
graph.setTitle("Before calibration")
view = otv.View(graph)

# %%
# Calibration with linear least squares
# -------------------------------------
# We are now able to perform the calibration, using linear least squares
# using the :class:`~openturns.LinearLeastSquaresCalibration` class.

algo = ot.LinearLeastSquaresCalibration(
    calibration_model,
    time_observations,
    population_observations,
    theta_prior,
)
algo.run()
calibrationResult = algo.getResult()

thetaMAP = calibrationResult.getParameterMAP()
print(thetaMAP)

# %%
# Plot the predictions before and after calibration.

model.setParameter(thetaMAP)
population_predicted_after = model(time_observations)

graph = plot_calibrated_prediction(
    data_US,
    data_FR,
    time_observations,
    population_predicted_before,
    population_predicted_after,
    "LLS"
)

view = otv.View(graph)

# %%
# Plot the parameter distributions.

grid = calibrationResult.drawParameterDistributions()
grid = decorateParameterDistribution(grid)
view = otv.View(grid, figure_kw={"figsize": (12, 3)},
                legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"})
pl.subplots_adjust(wspace=0.4)
test = pl.suptitle("LLS")

# %%
# Plot the observation errors.

observation_error = calibrationResult.getObservationsError()
observation_error.setDescription([r"$\epsilon_{US}$", r"$\epsilon_{FR}$"])
graph = observation_error.drawPDF()
graph.setTitle("LLS: " + graph.getTitle())
view = otv.View(graph, legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"})

# %%
# Plot the residual analysis.

grid = calibrationResult.drawResiduals()
grid = decorateResiduals(grid)
grid.setTitle("LLS: Residual analysis")
view = otv.View(grid, legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"})
pl.subplots_adjust(wspace=0.4, top=0.9)

# %%

printRMSEValue(calibrationResult, theta_prior)

# %%
# Gaussian linear calibration
# ---------------------------
# We will now perform a Gaussian linear calibration using the
# :class:`~openturns.GaussianLinearCalibration` class.

# Prior distribution
sigmaA_US = 0.1 * a_US
sigmaC_US = 10.0
sigmaA_FR = 0.1 * a_FR
sigmaC_FR = 10.0
sigmaM = 1.0

parameterCovariance = ot.CovarianceMatrix(5)
parameterCovariance[0, 0] = sigmaA_US ** 2
parameterCovariance[1, 1] = sigmaC_US ** 2
parameterCovariance[2, 2] = sigmaA_FR ** 2
parameterCovariance[3, 3] = sigmaC_FR ** 2
parameterCovariance[4, 4] = sigmaM ** 2

# Standard deviation of the population observation error
# We assume independent covariance errors with standard deviation
# error equal to 10.0 millions.
sigma_pop = 10.0
errorCovariance = ot.CovarianceMatrix(2)
errorCovariance[0, 0] = sigma_pop ** 2
errorCovariance[1, 1] = sigma_pop ** 2

algo = ot.GaussianLinearCalibration(
    calibration_model,
    time_observations,
    population_observations,
    theta_prior,
    parameterCovariance,
    errorCovariance,
)

algo.run()
calibrationResult = algo.getResult()

thetaMAP = calibrationResult.getParameterMAP()
print(thetaMAP)

# %%
# Plot the predictions before and after calibration.

model.setParameter(thetaMAP)
population_predicted_after = model(time_observations)

graph = plot_calibrated_prediction(
    data_US,
    data_FR,
    time_observations,
    population_predicted_before,
    population_predicted_after,
    "Gaussian Linear"
)
view = otv.View(graph)

# %%
# Plot the parameter distributions.

grid = calibrationResult.drawParameterDistributions()
grid = decorateParameterDistribution(grid)
view = otv.View(grid, figure_kw={"figsize": (12, 3)},
                legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"})
pl.subplots_adjust(wspace=0.4)
text = pl.suptitle("GaussianLinearCalibration")

# %%
# Plot the observation errors.

observation_error = calibrationResult.getObservationsError()
observation_error.setDescription([r"$\epsilon_{US}$", r"$\epsilon_{FR}$"])
graph = observation_error.drawPDF()
graph.setTitle("Gaussian Lin.: " + graph.getTitle())
view = otv.View(graph, legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"})

# %%
# Plot the residual analysis.

grid = calibrationResult.drawResiduals()
grid = decorateResiduals(grid)
grid.setTitle("Gaussian Lin.: Residual analysis")
view = otv.View(grid, legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"})
pl.subplots_adjust(wspace=0.4, top=0.9)

# %%

printRMSEValue(calibrationResult, theta_prior)

# %%

otv.View.ShowAll()
