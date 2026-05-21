"""
Using the FORM - SORM algorithms on a nonlinear function
=========================================================
"""

# %%
# In this example, we estimate a failure probability with the `FORM` and `SORM` algorithms on the :ref:`oscillator <use-case-oscillator>` example.
# This test-case is highly non linear with a significant curvature near the design point.

# %%
# Model definition
# -----------------

# %%
from openturns.usecases import oscillator
import openturns as ot
import openturns.viewer as otv
import numpy as np

ot.RandomGenerator.SetSeed(1)

# %%
# We load the model from the usecases module:
osc = oscillator.Oscillator()

# %%
# We use the input parameters distribution from the data class:
distribution = osc.distribution


# %%
# We define the model:
model = osc.model

# %%
# We create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Less(), 0.0)
event.setName("failure")

# %%
# Cross cuts in the physical space
# ---------------------------------

# %%
# Let's have a look on 2D cross cuts of the limit state function. For each 2D cross cut, the other variables are fixed to the input distribution mean values.
# This graph allows one to have a first idea of the variations of the function in pairs of dimensions. The colors of each contour plot are comparable.
# The number of contour levels are related to the amount of variation of the function in the corresponding coordinates.


lowerBound = [1e-5] * 8
upperBound = distribution.getRange().getUpperBound()
nX = 50
description = distribution.getDescription()
description.add("")
model.setDescription(description)
grid = model.drawCrossCuts(
    distribution.getMean(),
    lowerBound,
    upperBound,
    [nX] * distribution.getDimension(),
    False,
    True,
    -5.0,
    50.0,
)
view = otv.View(grid, contour_kw={"cmap": "hsv", "levels": 55})
axes = view.getAxes()
fig = view.getFigure()
fig.set_size_inches(12, 12)
fig.colorbar(
    view.getSubviews()[1][0].getContourSets()[0], ax=axes[:-2, -1], fraction=0.3
)
for i in range(len(axes)):
    for j in range(i + 1):
        if i < len(axes) - 1:
            axes[i][j].xaxis.set_ticklabels([])
        if j > 0:
            axes[i][j].yaxis.set_ticklabels([])
        axes[i][j].set_xticks([])
        axes[i][j].set_yticks([])
fig.subplots_adjust(top=0.99, bottom=0.05, left=0.06, right=0.99)

# %%
# Computation of reference probability with Monte-Carlo simulation
# ----------------------------------------------------------------

# %%
# The target probability is supposed to be extremely low (:math:`3.78\times 10^{-7}`).
# Indeed, when performing Monte-Carlo simulation with a simulation budget of 100000 points, no sample are in the failure state, that induces a probability estimate of zero.

experiment = ot.MonteCarloExperiment()
algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
algo.setMaximumOuterSampling(int(1e5))
algo.run()

result = algo.getResult()
probability = result.getProbabilityEstimate()
print("Failure probability = ", probability)

# %%
# To get an accurate estimate of this probability, a billion of simulations are necessary.

# %%
# FORM Analysis
# -------------

# %%
# To get a first idea of the failure probability with reduced simulation budget, one can use the First Order Reliability Method (FORM).


# %%
# Define a solver:
optimAlgo = ot.Cobyla()
optimAlgo.setStartingPoint(distribution.getMean())
optimAlgo.setMaximumCallsNumber(1000)
optimAlgo.setMaximumAbsoluteError(1.0e-3)
optimAlgo.setMaximumRelativeError(1.0e-3)
optimAlgo.setMaximumResidualError(1.0e-3)
optimAlgo.setMaximumConstraintError(1.0e-3)

# %%
# Run FORM initialized at the mean of the distribution:
algo = ot.FORM(optimAlgo, event)
algo.run()
result = algo.getResult()

# %%
# Probability of failure:
print("FORM estimate of the probability of failure: ", result.getEventProbability())

# %%
# Design point in the standard space:

# %%
designPointStandardSpace = result.getStandardSpaceDesignPoint()
print("Design point in standard space : ", designPointStandardSpace)

# %%
# Design point in the physical space:

# %%
print("Design point in physical space : ", result.getPhysicalSpaceDesignPoint())


# %%
# For this test case, in order to validate the results obtained by FORM, we can plot the cross cuts in the standard space near the design point found by FORM.


# %%

inverseIsoProbabilistic = distribution.getInverseIsoProbabilisticTransformation()
standardSpaceLimitState = ot.ComposedFunction(model, inverseIsoProbabilistic)

# sphinx_gallery_thumbnail_number = 2

nX = 50
xMin = designPointStandardSpace - ot.Point(distribution.getDimension(), 4.0)
xMax = designPointStandardSpace + ot.Point(distribution.getDimension(), 4.0)
description = distribution.getDescription()
description.add("")
standardSpaceLimitState.setDescription(description)
grid = standardSpaceLimitState.drawCrossCuts(
    designPointStandardSpace, xMin, xMax, [nX] * distribution.getDimension(),
    False, True, -5.0, 50.0,
)
view = otv.View(grid, contour_kw={"cmap": "hsv", "levels": 55})
axes = view.getAxes()
axFigure = view.getFigure()
axFigure.set_size_inches(12, 12)

my_labels = {
    "MPP": "Design Point",
    "O": "Origin in Standard Space",
    "TLSF": "True Limit State Function",
    "ALSF": "Approximated Limit State Function",
}

for i in range(distribution.getDimension()):
    for j in range(i):
        ax = axes[i - 1][j]

        crossCutIndices = []
        crossCutReferencePoint = []
        for k in range(distribution.getDimension()):
            if k != i and k != j:
                crossCutIndices.append(k)
                crossCutReferencePoint.append(designPointStandardSpace[k])
        crossCutFunction = ot.ParametricFunction(
            standardSpaceLimitState, crossCutIndices, crossCutReferencePoint
        )

        crossCutLowerBound = [xMin[j], xMin[i]]
        crossCutUpperBound = [xMax[j], xMax[i]]
        inputData = ot.Box([nX, nX]).generate()
        inputData *= ot.Point(crossCutUpperBound) - ot.Point(crossCutLowerBound)
        inputData += ot.Point(crossCutLowerBound)
        meshX = np.array(inputData)[:, 0].reshape(nX + 2, nX + 2)
        meshY = np.array(inputData)[:, 1].reshape(nX + 2, nX + 2)
        data = crossCutFunction(inputData)
        meshZ = np.array(data).reshape(nX + 2, nX + 2)

        ax.plot(
            designPointStandardSpace[j],
            designPointStandardSpace[i],
            "o",
            label=my_labels["MPP"],
        )
        ax.plot(0.0, 0.0, "rs", label=my_labels["O"])
        cs2 = ax.contour(meshX, meshY, meshZ, [0.0])

        u0 = [designPointStandardSpace[j], designPointStandardSpace[i]]
        algo = ot.LinearTaylor(u0, crossCutFunction)
        algo.run()
        responseSurface = algo.getMetaModel()
        data2 = responseSurface(inputData)
        meshZ2 = np.array(data2).reshape(nX + 2, nX + 2)
        cs3 = ax.contour(meshX, meshY, meshZ2, [0.0], linestyles="dotted")

        if i == 1 and j == 0:
            h2, l2 = cs2.legend_elements()
            h3, l3 = cs3.legend_elements()
            lg = ax.legend(
                [h2[0], h3[0]],
                [my_labels["TLSF"], my_labels["ALSF"]],
                frameon=False,
                loc="upper center",
                bbox_to_anchor=(8, -1.5),
            )
        elif i == 2 and j == 0:
            lg1 = ax.legend(
                frameon=False, loc="upper center", bbox_to_anchor=(7.65, -0.8)
            )

# %%
# As it can be seen, the curvature of the limit state function near the design point is significant.
# In that way, FORM provides poor estimate since it linearly approximates the limit state function.
# Thus, SORM can be used in order to refine this probability estimation by approximating the limit state function with a quadratic model.

# %%
# SORM Analysis
# -------------

# %%
# Run SORM initialized at the mean of the distribution:


# %%
solver = ot.Cobyla()
solver.setStartingPoint(distribution.getMean())
algoSORM = ot.SORM(solver, event)
algoSORM.run()

# %%
# Analysis of SORM results:

# %%
resultSORM = algoSORM.getResult()
print(
    "Probability estimate with SORM (Breitung correction) =",
    resultSORM.getEventProbabilityBreitung(),
)

print(
    "Probability estimate with SORM (Hohenbichler correction) =",
    resultSORM.getEventProbabilityHohenbichler(),
)

optim_res = resultSORM.getOptimizationResult()
print("Simulation budget:", optim_res.getCallsNumber())

# %%
# One can see that the probability estimate has been decreased by a factor 10 compared to the FORM estimate.
# This probability is quite close to the reference probability and obtained with less than 1000 evaluations of the model.

# %%
# In order to visualize the SORM limit state approximation, we can draw cross cuts of SORM oscultating parabola using second order Taylor approximation.

nX = 50
xMin = designPointStandardSpace - ot.Point(distribution.getDimension(), 4.0)
xMax = designPointStandardSpace + ot.Point(distribution.getDimension(), 4.0)
description = distribution.getDescription()
description.add("")
standardSpaceLimitState.setDescription(description)
grid = standardSpaceLimitState.drawCrossCuts(
    designPointStandardSpace, xMin, xMax, [nX] * distribution.getDimension(),
    False, True, -5.0, 50.0,
)
view = otv.View(grid, contour_kw={"cmap": "hsv", "levels": 55})
axes = view.getAxes()
axFigure = view.getFigure()
axFigure.set_size_inches(12, 12)

my_labels = {
    "MPP": "Design Point",
    "O": "Origin in Standard Space",
    "TLSF": "True Limit State Function",
    "ALSF": "Approximated Limit State Function",
}

for i in range(distribution.getDimension()):
    for j in range(i):
        ax = axes[i - 1][j]

        crossCutIndices = []
        crossCutReferencePoint = []
        for k in range(distribution.getDimension()):
            if k != i and k != j:
                crossCutIndices.append(k)
                crossCutReferencePoint.append(designPointStandardSpace[k])
        crossCutFunction = ot.ParametricFunction(
            standardSpaceLimitState, crossCutIndices, crossCutReferencePoint
        )

        crossCutLowerBound = [xMin[j], xMin[i]]
        crossCutUpperBound = [xMax[j], xMax[i]]
        inputData = ot.Box([nX, nX]).generate()
        inputData *= ot.Point(crossCutUpperBound) - ot.Point(crossCutLowerBound)
        inputData += ot.Point(crossCutLowerBound)
        meshX = np.array(inputData)[:, 0].reshape(nX + 2, nX + 2)
        meshY = np.array(inputData)[:, 1].reshape(nX + 2, nX + 2)
        data = crossCutFunction(inputData)
        meshZ = np.array(data).reshape(nX + 2, nX + 2)

        ax.plot(
            designPointStandardSpace[j],
            designPointStandardSpace[i],
            "o",
            label=my_labels["MPP"],
        )
        ax.plot(0.0, 0.0, "rs", label=my_labels["O"])
        cs2 = ax.contour(meshX, meshY, meshZ, [0.0])

        u0 = [designPointStandardSpace[j], designPointStandardSpace[i]]
        algo = ot.QuadraticTaylor(u0, crossCutFunction)
        algo.run()
        responseSurface = algo.getMetaModel()
        data2 = responseSurface(inputData)
        meshZ2 = np.array(data2).reshape(nX + 2, nX + 2)
        cs3 = ax.contour(meshX, meshY, meshZ2, [0.0], linestyles="dotted")

        if i == 1 and j == 0:
            h2, l2 = cs2.legend_elements()
            h3, l3 = cs3.legend_elements()
            lg = ax.legend(
                [h2[0], h3[0]],
                [my_labels["TLSF"], my_labels["ALSF"]],
                frameon=False,
                loc="upper center",
                bbox_to_anchor=(8, -1.5),
            )
        elif i == 2 and j == 0:
            lg1 = ax.legend(
                frameon=False, loc="upper center", bbox_to_anchor=(7.65, -0.8)
            )
# %%
# We can see that this approximation is very appropriate, explaining the accuracy of the obtained results.

# %%
# Estimation with post analytical Importance Sampling
# ---------------------------------------------------

# %%
# Different algorithms exist for the reliability analysis by Importance Sampling.
# One way is to perform post analytical Importance Sampling by defining the auxiliary density centered at the design point found by FORM.


# %%
postAnalyticalIS = ot.PostAnalyticalImportanceSampling(result)
postAnalyticalIS.setMaximumCoefficientOfVariation(0.05)
postAnalyticalIS.run()
resultPostAnalyticalIS = postAnalyticalIS.getResult()
print(
    "Probability of failure with post analytical IS = ",
    resultPostAnalyticalIS.getProbabilityEstimate(),
)

# %%
# We can see that this post-treatment of FORM result allows one to greatly improve the quality of the probability estimation.
otv.View.ShowAll()
