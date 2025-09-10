"""
Gaussian Process Regression: Normalization for optimization
===========================================================
"""

# %%
# This example aims to illustrate Gaussian Process Fitter metamodel with normalization of data.
# Like other machine learning techniques, heteregeneous data (i.e., data defined with different orders of magnitude)
# can impact the training process of Gaussian Process Regression (GPR).
# Automatic scaling process of the input data for the optimization of GPR hyperparameters can be defined using
# the :class:`~openturns.ResourceMap` key `GaussianProcessFitter-OptimizationNormalization`.
# In this example, we show the behavior of Gaussian Process Fitter with and without activating
# the normalization of hyperparameters for the optimization.


# %%
# Loading of the fire satellite use case
# -----------------------------------------------------
# This model involves 9 input variables and 3 output variables. We select only the first output variable in this example.
# We load the :ref:`Fire satellite use case<use-case-fire-satellite>`.

# %%
import openturns as ot
from openturns.usecases.fire_satellite import FireSatelliteModel
import openturns.viewer as otv
import openturns.experimental as otexp


# %%
# We define the function that evaluates the outputs depending on the inputs.
m = FireSatelliteModel()
model = m.model

# %%
# We also define the distribution of input variables to build the training and test sets.
inputDistribution = m.inputDistribution


# %%
# Generation of data
# ------------------
# We now generate the input and output training sets as 20 times the dimension of the input vector.
experiment = ot.LHSExperiment(inputDistribution, 20 * m.dim)
inputTrainingSet = experiment.generate()
outputTrainingSet = model(inputTrainingSet).getMarginal(0)

print("Lower and upper bounds of inputTrainingSet:")
print(inputTrainingSet.getMin(), inputTrainingSet.getMax())

# %%
# Creation of metamodel
# ---------------------
# We choose to use a constant trend.
basis = ot.LinearBasisFactory(m.dim).build()


# %%
# For the purpose of illustration, we consider :class:`~openturns.MaternModel`.
covarianceModel = ot.MaternModel([1.0] * m.dim, 2.5)


# %%
# Training without normalization
# ------------------------------
# First, we deactivate the normalization option for the optimization.
ot.ResourceMap.SetAsBool('GaussianProcessFitter-OptimizationNormalization', False)

# %%
# We run the algorithm and get the metamodel.
fitter_algo = otexp.GaussianProcessFitter(
    inputTrainingSet, outputTrainingSet, covarianceModel, basis
)
fitter_algo.run()
fitter_result = fitter_algo.getResult()
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()

# %%
# Inspect hyperparameters
theta = gpr_result.getCovarianceModel().getParameter()
print("theta=", theta)


# %%
# Validation of metamodel
# To validate the metamodel, we create a validation set of size equal to 50 times the input vector dimension to evaluate the functions.
gprMetamodel = gpr_result.getMetaModel()
ot.RandomGenerator.SetSeed(1)
experimentTest = ot.LHSExperiment(inputDistribution, 50 * m.dim)
inputTestSet = experimentTest.generate()
outputTestSet = model(inputTestSet).getMarginal(0)

# %%
# Then, we use the :class:`~openturns.MetaModelValidation` class to validate the metamodel.
metamodelPredictions = gprMetamodel(inputTestSet)
val = ot.MetaModelValidation(outputTestSet, metamodelPredictions)
r2Score = val.computeR2Score()
print("R2=", r2Score)

# %%
# Graphical validation
label = "Accuracy of metamodel without activating normalization for optimization"
graph = val.drawValidation().getGraph(0, 0)
graph.setLegends([""])
graph.setLegends(["R2 = %.2f%%" % (100 * r2Score[0]), ""])
graph.setLegendPosition("upper left")
graph.setXTitle("Exact function")
graph.setYTitle("Metamodel prediction")
graph.setTitle(label)
_ = otv.View(graph)

# %%
# Training with normalization
# ---------------------------
# Then, we activate the normalization option for the optimization.

# %%
ot.ResourceMap.SetAsBool('GaussianProcessFitter-OptimizationNormalization', True)

# %%
# We run the algorithm and get the metamodel.

# %%
fitter_algo = otexp.GaussianProcessFitter(
    inputTrainingSet, outputTrainingSet, covarianceModel, basis
)
fitter_algo.run()
fitter_result = fitter_algo.getResult()
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()

# %%
# Inspect hyperparameters: we can see that parameters are much different this time
theta = gpr_result.getCovarianceModel().getParameter()
print("theta=", theta)

# %%
# Validation of metamodel
gprMetamodel = gpr_result.getMetaModel()
metamodelPredictions = gprMetamodel(inputTestSet)
val = ot.MetaModelValidation(outputTestSet, metamodelPredictions)
r2Score = val.computeR2Score()
print("R2=", r2Score)

# %%
# Graphical validation
label = "Accuracy of metamodel with activating normalization for optimization"
graph2 = val.drawValidation().getGraph(0, 0)
graph2.setLegends([""])
graph2.setLegends(["R2 = %.2f%%" % (100 * r2Score[0]), ""])
graph2.setLegendPosition("upper left")
graph2.setXTitle("Exact function")
graph2.setYTitle("Metamodel prediction")
graph2.setTitle(label)
_ = otv.View(graph2)
otv.View.ShowAll()
