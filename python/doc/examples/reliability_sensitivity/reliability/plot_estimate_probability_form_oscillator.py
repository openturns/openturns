"""
Using the FORM - SORM algorithms on a non linear function
=========================================================
"""
# %%
# In this example, we estimate a failure probability with the `FORM` and `SORM` algorithms on the :ref:`oscillator <use-case-oscillator>` example.
# This test-case is highly non linear with a significant curvature near the design point.

# %%
# Model definition
# ----------------

# %%
from openturns.usecases import oscillator
import openturns as ot
from matplotlib import pylab as plt
import numpy as np

ot.Log.Show(ot.Log.NONE)

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
# Let’s have a look on 2D cross cuts of the limit state function. For each 2D cross cut, the other variables are fixed to the input distribution mean values.
# This graph allows one to have a first idea of the variations of the function in pairs of dimensions. The colors of each contour plot are comparable.
# The number of contour levels are related to the amount of variation of the function in the corresponding coordinates.


fig = plt.figure(figsize=(12, 12))
lowerBound = [1e-5] * 8
upperBound = distribution.getRange().getUpperBound()

# Definition of number of meshes in x and y axes for the 2D cross cut plots
nX = 50
nY = 50
for i in range(distribution.getDimension()):
    for j in range(i):
        crossCutIndices = []
        crossCutReferencePoint = []
        for k in range(distribution.getDimension()):
            if k != i and k != j:
                crossCutIndices.append(k)
                # Definition of the reference point
                crossCutReferencePoint.append(distribution.getMean()[k])
        # Definition of 2D cross cut function
        crossCutFunction = ot.ParametricFunction(
            model, crossCutIndices, crossCutReferencePoint
        )
        crossCutLowerBound = [lowerBound[j], lowerBound[i]]
        crossCutUpperBound = [upperBound[j], upperBound[i]]
        # Definition of the mesh
        inputData = ot.Box([nX, nY]).generate()
        inputData *= ot.Point(crossCutUpperBound) - ot.Point(crossCutLowerBound)
        inputData += ot.Point(crossCutLowerBound)
        meshX = np.array(inputData)[:, 0].reshape(nX + 2, nY + 2)
        meshY = np.array(inputData)[:, 1].reshape(nX + 2, nY + 2)
        data = crossCutFunction(inputData)
        meshZ = np.array(data).reshape(nX + 2, nY + 2)
        levels = [(150 + 3 * i) for i in range(101)]

        # Creation of the contour
        index = 1 + i * distribution.getDimension() + j

        ax = fig.add_subplot(
            distribution.getDimension(), distribution.getDimension(), index
        )
        ax.pcolormesh(meshX, meshY, meshZ, cmap="hsv", vmin=-5, vmax=50, shading="auto")
        ax.set_xticks([])
        ax.set_yticks([])
        # Creation of axes title
        if j == 0:
            ax.set_ylabel(distribution.getDescription()[i])
        if i == 7:
            ax.set_xlabel(distribution.getDescription()[j])

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
# To get a first idea of the failure probability with reduced simulation budget, one can use the First Order Realiability Method (FORM).


# %%
# Define a solver:
optimAlgo = ot.Cobyla()
optimAlgo.setMaximumEvaluationNumber(1000)
optimAlgo.setMaximumAbsoluteError(1.0e-3)
optimAlgo.setMaximumRelativeError(1.0e-3)
optimAlgo.setMaximumResidualError(1.0e-3)
optimAlgo.setMaximumConstraintError(1.0e-3)

# %%
# Run FORM initialized at the mean of the distribution:
algo = ot.FORM(optimAlgo, event, distribution.getMean())
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

distributionStandard = ot.Normal(distribution.getDimension())
inverseIsoProbabilistic = distribution.getInverseIsoProbabilisticTransformation()
standardSpaceLimitState = ot.ComposedFunction(model, inverseIsoProbabilistic)
standardSpaceLimitStateFunction = ot.PythonFunction(8, 1, standardSpaceLimitState)

fig = plt.figure(figsize=(12, 12))
lowerBound = [-4] * 8
upperBound = [4] * 8

# sphinx_gallery_thumbnail_number = 2
# Definition of number of meshes in x and y axes for the 2D cross cut plots
nX = 50
nY = 50
for i in range(distribution.getDimension()):
    for j in range(i):

        crossCutIndices = []
        crossCutReferencePoint = []
        for k in range(distribution.getDimension()):
            if k != i and k != j:
                crossCutIndices.append(k)
                # Definition of the reference point
                crossCutReferencePoint.append(designPointStandardSpace[k])
        # Definition of 2D cross cut function
        crossCutFunction = ot.ParametricFunction(
            standardSpaceLimitStateFunction, crossCutIndices, crossCutReferencePoint
        )

        crossCutLowerBound = [
            lowerBound[j] + designPointStandardSpace[j],
            lowerBound[i] + designPointStandardSpace[i],
        ]
        crossCutUpperBound = [
            upperBound[j] + designPointStandardSpace[j],
            upperBound[i] + designPointStandardSpace[i],
        ]
        # Definition of the mesh
        inputData = ot.Box([nX, nY]).generate()
        inputData *= ot.Point(crossCutUpperBound) - ot.Point(crossCutLowerBound)
        inputData += ot.Point(crossCutLowerBound)
        meshX = np.array(inputData)[:, 0].reshape(nX + 2, nY + 2)
        meshY = np.array(inputData)[:, 1].reshape(nX + 2, nY + 2)
        data = crossCutFunction(inputData)
        meshZ = np.array(data).reshape(nX + 2, nY + 2)
        levels = [(150 + 3 * i) for i in range(101)]

        # Creation of the contour
        index = 1 + i * distribution.getDimension() + j

        ax = fig.add_subplot(
            distribution.getDimension(), distribution.getDimension(), index
        )

        graph = ot.Graph()

        ax.pcolormesh(meshX, meshY, meshZ, cmap="hsv", vmin=-5, vmax=50, shading="auto")
        ax.plot(designPointStandardSpace[j], designPointStandardSpace[i], "o")
        ax.contour(
            meshX,
            meshY,
            meshZ,
            [standardSpaceLimitStateFunction(designPointStandardSpace)[0]],
        )
        ax.set_xticks([])
        ax.set_yticks([])

        u0 = [designPointStandardSpace[j], designPointStandardSpace[i]]
        algo = ot.LinearTaylor(u0, crossCutFunction)
        algo.run()
        responseSurface = algo.getMetaModel()
        data2 = responseSurface(inputData)
        meshZ2 = np.array(data2).reshape(nX + 2, nY + 2)
        ax.contour(meshX, meshY, meshZ2, [0.0], linestyles="dotted")

        # Creation of axes title
        if j == 0:
            ax.set_ylabel(distribution.getDescription()[i])
        if i == 7:
            ax.set_xlabel(distribution.getDescription()[j])

# %%
# As it can be seen, the curvature of the limit state function near the design point is significant. In that way, FORM provides poor estimate since it linearly approximates the limit state function.
# Thus, SORM can be used in order to refine this probability estimation by approximating the limit state function with a quadratic model.

# %%
# SORM Analysis
# -------------

# %%
# Run SORM initialized at the mean of the distribution:


# %%
algoSORM = ot.SORM(ot.Cobyla(), event, distribution.getMean())
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
print("Simulation budget:", optim_res.getEvaluationNumber())

# %%
# One can see that the probability estimate has been decreased by a factor 10 compared to the FORM estimate.
# This probability is quite close to the reference probability and obtained with less than 1000 evaluations of the model.

# %%
# In order to visualize the SORM limit state approximation, we can draw cross cuts of SORM oscultating parabola using second order Taylor approximation.

fig = plt.figure(figsize=(12, 12))
lowerBound = [-4] * 8
upperBound = [4] * 8

# Definition of number of meshes in x and y axes for the 2D cross cut plots

designPointStandardSpace = resultSORM.getStandardSpaceDesignPoint()
nX = 50
nY = 50
for i in range(distribution.getDimension()):
    for j in range(i):

        crossCutIndices = []
        crossCutReferencePoint = []
        for k in range(distribution.getDimension()):
            if k != i and k != j:
                crossCutIndices.append(k)
                # Definition of the reference point
                crossCutReferencePoint.append(designPointStandardSpace[k])
        # Definition of 2D cross cut function
        crossCutFunction = ot.ParametricFunction(
            standardSpaceLimitStateFunction, crossCutIndices, crossCutReferencePoint
        )

        crossCutLowerBound = [
            lowerBound[j] + designPointStandardSpace[j],
            lowerBound[i] + designPointStandardSpace[i],
        ]
        crossCutUpperBound = [
            upperBound[j] + designPointStandardSpace[j],
            upperBound[i] + designPointStandardSpace[i],
        ]
        # Definition of the mesh
        inputData = ot.Box([nX, nY]).generate()
        inputData *= ot.Point(crossCutUpperBound) - ot.Point(crossCutLowerBound)
        inputData += ot.Point(crossCutLowerBound)
        meshX = np.array(inputData)[:, 0].reshape(nX + 2, nY + 2)
        meshY = np.array(inputData)[:, 1].reshape(nX + 2, nY + 2)
        data = crossCutFunction(inputData)
        meshZ = np.array(data).reshape(nX + 2, nY + 2)
        levels = [(150 + 3 * i) for i in range(101)]

        # Creation of the contour
        index = 1 + i * distribution.getDimension() + j

        ax = fig.add_subplot(
            distribution.getDimension(), distribution.getDimension(), index
        )

        graph = ot.Graph()
        ax.pcolormesh(meshX, meshY, meshZ, cmap="hsv", vmin=-5, vmax=50, shading="auto")
        ax.plot(designPointStandardSpace[j], designPointStandardSpace[i], "o")
        ax.contour(
            meshX,
            meshY,
            meshZ,
            [standardSpaceLimitStateFunction(designPointStandardSpace)[0]],
        )
        ax.set_xticks([])
        ax.set_yticks([])

        u0 = [designPointStandardSpace[j], designPointStandardSpace[i]]
        algo = ot.QuadraticTaylor(u0, crossCutFunction)
        algo.run()
        responseSurface = algo.getMetaModel()
        data2 = responseSurface(inputData)
        meshZ2 = np.array(data2).reshape(nX + 2, nY + 2)
        ax.contour(
            meshX,
            meshY,
            meshZ2,
            [standardSpaceLimitStateFunction(designPointStandardSpace)[0]],
            linestyles="dotted",
        )

        # Creation of axes title
        if j == 0:
            ax.set_ylabel(distribution.getDescription()[i])
        if i == 7:
            ax.set_xlabel(distribution.getDescription()[j])
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
postAnalyticalIS.setMaximumCoefficientOfVariation(0.1)
postAnalyticalIS.run()
resultPostAnalyticalIS = postAnalyticalIS.getResult()
print(
    "Probability of failure with post analytical IS = ",
    resultPostAnalyticalIS.getProbabilityEstimate(),
)

# %%
# We can see that this post-treatment of FORM result allows one to greatly improve the quality of the probability estimation.
