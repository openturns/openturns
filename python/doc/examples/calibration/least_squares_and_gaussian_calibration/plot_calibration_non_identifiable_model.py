"""
Calibration of a non-identifiable model: influence of the optimizer
===================================================================

In this example we present a case that is not identifiable (there is an infinite number of optimal
parameters). It is similar to the :doc:`calibration of the flooding model <plot_calibration_flooding>`
example but has 2 parameters instead of 3.

We will perform a non-linear least squares calibration with 2 different optimizers, and see that the
choice of the optimizer can greatly influence the results.
"""

# %%
# Parameters to calibrate and observations
# ----------------------------------------
#
# The variables of the model are:
#
# - :math:`Q` : Input. Observed.
# - :math:`K_s`, :math:`\alpha` : Input. Calibrated.
# - :math:`H`: Output. Observed.
#
# The height of the river is:
#
# .. math::
#    H = \left(\frac{Q}{K_s B \sqrt{\alpha \times 10^{-3}}}\right)^{0.6},
#
# where
#
# - :math:`Q` is the flowrate of the river,
# - :math:`B` = 300 (m) is the width of the river,
# - :math:`K_s` is the Strickler coefficient and :math:`\alpha` is the slope.

# %%
# Define the observations
# -----------------------
# We import the dataset from the :class:`~openturns.usecases.flood_model.FloodModel` class.

# %%
import openturns as ot
from openturns.usecases import flood_model
import openturns.viewer as otv
import matplotlib.pyplot as plt
import math

fm = flood_model.FloodModel()
sampleQH = fm.data100
sampleQ = sampleQH[:, 0]
sampleH = sampleQH[:, 1]
print(sampleQH[:5, :])


# %%
# Create the physical model
# -------------------------
# We define the model :math:`g` which has 3 inputs and one output H.

# %%
def functionFlooding2Parameters(X):
    B = 300.0
    Q, K_s, alpha = X
    alpha *= 1.0e-3  # Downscale the slope
    if alpha <= 0.0 or K_s <= 0.0:
        H = math.inf
    else:
        H = (Q / (K_s * B * math.sqrt(alpha))) ** (3.0 / 5.0)
    return [H]


g = ot.PythonFunction(3, 1, functionFlooding2Parameters)
g.setOutputDescription(["H (m)"])
g.setInputDescription(["Q (m3/s)", "Ks", "alpha"])

# %%
# Create the parametric functions
# -------------------------------
# We create the parametric functions that we need.

# %%
referenceKs = 20.0
referenceAlpha = 0.4
referencePoint = [referenceKs, referenceAlpha]
model = ot.ParametricFunction(g, [1, 2], referencePoint)
calibrationModel = ot.ParametricFunction(g, [0], [0.0])


# %%
# Calibration with non-linear least squares
# -----------------------------------------
# We use :class:`~openturns.NonLinearLeastSquaresCalibration` to perform a calibration with bootstrap resampling.

# %%
def plot_parameter_distribution(calibrationResult, title):
    grid = calibrationResult.drawParameterDistributions()
    otv.View(
        grid,
        figure_kw={"figsize": (7.0, 2.5)},
        legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"},
    )
    plt.subplots_adjust(wspace=0.4)
    plt.suptitle(title)


# %%
# First we use the :class:`~openturns.Ceres` optimizer (it's the default one but we set it explicitly here).

# %%
algo = ot.NonLinearLeastSquaresCalibration(calibrationModel, sampleQ, sampleH, referencePoint)
algo.setOptimizationAlgorithm(ot.Ceres())
algo.run()
resultCeres = algo.getResult()
thetaCeres = resultCeres.getParameter()
print(f"Ks = {thetaCeres[0]:.1f}, alpha = {thetaCeres[1]:.2f}")

# %%
plot_parameter_distribution(resultCeres, "Non-linear least squares calibration with Ceres")

# %%
# Now we use the :class:`~openturns.CMinpack` optimizer.

# %%
algo = ot.NonLinearLeastSquaresCalibration(calibrationModel, sampleQ, sampleH, referencePoint)
algo.setOptimizationAlgorithm(ot.CMinpack())
algo.run()
resultCMinpack = algo.getResult()
thetaCMinpack = resultCMinpack.getParameter()
print(f"Ks = {thetaCMinpack[0]:.1f}, alpha = {thetaCMinpack[1]:.2f}")

# %%
plot_parameter_distribution(resultCMinpack, "Non-linear least squares calibration with CMinpack")

# %%
# We see that the solvers converge to different solutions. The shape of the distribution is also wider with CMinpack than Ceres.


# %%
# Plot the cost function
# ----------------------
# We plot the cost function and display the solutions found by Ceres and CMinpack.

# %%
class LeastSquaresCalibrationCostFunction(ot.OpenTURNSPythonFunction):

    def __init__(self, model, inputObservations, outputObservations):
        """
        Create a least squares calibration cost function.

        Parameters
        ----------
        model : ot.Function()
            The parametric model.
        inputObservations : 2-d sequence of float
            The input observations.
        outputObservations : 2-d sequence of float
            The output observations.
        """
        self.model = model
        self.inputObservations = inputObservations
        self.outputObservations = outputObservations
        super().__init__(model.getParameterDimension(), 1)
        self.setInputDescription(model.getParameterDescription())
        self.setOutputDescription(["Cost"])

    def _exec(self, theta):
        """
        Compute the least squares cost function

        Parameters
        ----------
        theta : ot.Point(dimension)
            The parameters.

        Returns
        -------
        cost : list(float)
            The cost function.

        """
        self.model.setParameter(theta)
        modelPredictions = self.model(self.inputObservations)
        residuals = self.outputObservations - modelPredictions
        residualsPoint = residuals.asPoint()
        sumOfSquares = residualsPoint.normSquare()
        return [sumOfSquares]


def PlotFilledContourWithEdges(contour):
    graph = ot.Graph()
    # Filled
    contourFilled = ot.Contour(contour)
    contourFilled.setIsFilled(True)
    contourFilled.setColorMap("PuBu")
    graph.add(contourFilled)
    # Edges
    contourEdges = ot.Contour(contour)
    contourEdges.setColor("black")
    contourEdges.setIsFilled(False)
    contourEdges.setColorBarPosition("")
    graph.add(contourEdges)
    return graph


ssFunction = LeastSquaresCalibrationCostFunction(model, sampleQ, sampleH)
ssFunction = ot.Function(ssFunction)

graph = ot.Graph("Least squares cost function", r"$K_s$", r"$\alpha$", True)
graphSS = ssFunction.draw([10.0, 0.1], [50.0, 3.0], [50] * 2)
contour = graphSS.getDrawable(0).getImplementation()
contour.buildDefaultLevels(7)
contour.setColorMapNorm("rank")
contour = PlotFilledContourWithEdges(contour)
graph.add(contour)
for result in [resultCeres, resultCMinpack]:
    theta = result.getParameter()
    cloud = ot.Cloud([theta[0]], [theta[1]])
    cloud.setPointStyle("circle")
    graph.add(cloud)
otv.View(
    graph,
    figure_kw={"figsize": (4.5, 3.5)}
)
plt.tight_layout()

otv.View.ShowAll()

# %%
# We see that the solutions found by Ceres (green) and CMinpack (red) are different but associated with the same cost.
