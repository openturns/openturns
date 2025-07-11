"""
Gaussian Process Regression: metamodel of the Branin-Hoo function
=================================================================
"""

# %%
# As a popular use case in optimization we briefly present the construction of a metamodel
# of the Branin (also referred to as Branin-Hoo) function.
#

# %%
from numpy import sqrt
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import branin_function
import openturns.experimental as otexp
ot.ResourceMap.SetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumCallsNumber", 3000)

# %%
# We load the Branin-Hoo model from the usecases module and use the model (stored in `objectiveFunction`)
bm = branin_function.BraninModel()
model = bm.objectiveFunction

# %%
# We shall represent this 2-d function with isolines. We set the number of isolines to a maximum of 10
# thanks to the following `ResourceMap` key :
ot.ResourceMap.SetAsUnsignedInteger("Contour-DefaultLevelsNumber", 10)
graphBasic = model.draw([0.0, 0.0], [1.0, 1.0], [100] * 2)
view = otv.View(graphBasic)

# %%
# We get the values of all isolines :
levels = graphBasic.getDrawables()[0].getLevels()

# # %%
# # We now build fancy isolines :

# # Build a range of colors
# ot.ResourceMap.SetAsUnsignedInteger("Drawable-DefaultPalettePhase", len(levels))
# palette = ot.Drawable.BuildDefaultPalette(len(levels))

graphFineTune = ot.Graph("The exact Branin model", r"$x_1$", r"$x_2$", True, "")
graphFineTune.setDrawables([graphBasic.getDrawable(0)])
# graphFineTune.setLegendPosition("")  # Remove the legend


# %%
# We also represent the three minima of the Branin function with orange diamonds :
sample1 = ot.Sample([bm.xexact1, bm.xexact2, bm.xexact3])
cloud1 = ot.Cloud(sample1, "orange", "diamond", "First Cloud")
graphFineTune.add(cloud1)
# Draw the graph with the palette assigned to the contour
view = otv.View(graphFineTune)

#
# The values of the exact model at these points are :
print(bm.objectiveFunction(sample1))

# %%
# The Branin function has a global minimum attained at three different points. We shall build a
# metamodel of this function that presents the same behaviour.


# %%
# Optimization of GP hyperparameters and conditioning to data
# -----------------------------------------------------------
#
# We use the :class:`~openturns.experimental.GaussianProcessFitter` class to perform the GP fitter analysis.
# We first generate a design of experiments with LHS and store the input training points in `xdata`
experiment = ot.LHSExperiment(
    ot.JointDistribution([ot.Uniform(0.0, 1.0), ot.Uniform(0.0, 1.0)]),
    28,
    False,
    True,
)
xdata = experiment.generate()

# %%
# We also get the output training values :
ydata = bm.objectiveFunction(xdata)


# %%
# This use case is defined in dimension 2 and we use a constant basis for the trend estimation :
dimension = bm.dim
basis = ot.ConstantBasisFactory(dimension).build()

# %%
# We choose a squared exponential covariance model in dimension 2 :
covarianceModel = ot.SquaredExponential([0.1] * dimension, [1.0])

# %%
# We have all the components to build a GP fitter  algorithm and run it :
fitter_algo = otexp.GaussianProcessFitter(xdata, ydata, covarianceModel, basis)
fitter_algo.setOptimizeParameters(True)
fitter_algo.run()
fitter_result = fitter_algo.getResult()
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()
print(gpr_result)

# %%
# Metamodel visualization
# -----------------------
#
# We get the metamodel (mean of the conditioned Gaussian Process) and the variance estimation.
# We draw the GP metamodel of the Branin function.
gprMetamodel = gpr_result.getMetaModel()
gccc = otexp.GaussianProcessConditionalCovariance(gpr_result)


graphBasic = gprMetamodel.draw([0.0, 0.0], [1.0, 1.0], [100] * 2)
# Take the first drawable as the only contour with multiple levels
contours = graphBasic.getDrawable(0).getImplementation()
contours.setColorBarPosition("")  # Hide the color bar
contours.setDrawLabels(True)  # Draw the labels
levels = contours.getLevels()

# Build a range of colors
ot.ResourceMap.SetAsUnsignedInteger("Drawable-DefaultPalettePhase", len(levels))
palette = ot.Drawable.BuildDefaultPalette(len(levels))

graphFineTune = ot.Graph("Branin metamodel (mean)", r"$x_1$", r"$x_2$", True, "")
graphFineTune.setDrawables([contours])
graphFineTune.setLegendPosition("")

# %%
# We also represent the location of the minima of the Branin function :
sample1 = ot.Sample([bm.xexact1, bm.xexact2, bm.xexact3])
cloud1 = ot.Cloud(sample1, "orange", "diamond", "First Cloud")
graphFineTune.add(cloud1)
view = otv.View(graphFineTune)

# %%
# We evaluate the metamodel at the minima locations :
print(gprMetamodel(sample1))

# %%
# Graphically, both the metamodel and the exact function look the same. The metamodel also has three
# basins around the minima and the value of the metamodel at each minimum location is comparable to
# the exact value of -0.979476. We have roughly two correct digits for each isoline.


# %%
# Standard deviation
# ------------------
#
# We finally take a look at the standard deviation in the :math:`[0,1] \times [0,1]` domain. It may be
# seen as a measure of the error of the metamodel.

# %%
# We discretize the domain with 22 points (`N` inside points and 2 endpoints) :
N = 20
inputData = ot.Box([N, N]).generate()

# %%
# We compute the conditional variance of the model and take the square root to get the deviation :
condCov = gccc.getConditionalMarginalVariance(inputData, 0)
condCovSd = sqrt(condCov)

# %%
# As we have previously done we build contours with the following levels ans labels :
levels = [0.01, 0.025, 0.050, 0.075, 0.1, 0.125, 0.150, 0.175]
contour = ot.Contour(N + 2, N + 2, condCovSd)
contour.setLevels(levels)
graphFineTune = ot.Graph("Standard deviation", r"$x_1$", r"$x_2$", True, "")
graphFineTune.setDrawables([contour])
graphFineTune.setLegendPosition("")

# %%
# We superimpose the training sample :
cloud = ot.Cloud(xdata)
cloud.setPointStyle("fcircle")
cloud.setColor("red")
graphFineTune.add(cloud)
view = otv.View(graphFineTune)

# %%
# We observe that the standard deviation is small in the center of the domain where we have enough
# data to learn the model.
# We can print the value of the variance at the first 5 training points (they all behave similarly) :
print(gccc.getConditionalMarginalVariance(xdata, 0)[0:5])

# %%
# These values are nearly zero which is expected as the metamodel interpolates data. The value being
# known it is not random anymore and the variance ought to be zero.

# %%
# Display all figures
otv.View.ShowAll()
