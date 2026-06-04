"""
Create a general linear model metamodel
=======================================
"""

# %%
# In this example we create a global approximation of a model response using a general linear model.
# We show how to use the :class:`~openturns.GaussianProcessFitter` class, which estimates the parameters of the model.

# %%
import openturns as ot
import openturns.viewer as otv


# %%
# We create a model and a sample from this model.

# %%
dimension = 2
input_names = ["x1", "x2"]
formulas = ["cos(0.1 * x1 + 0.2 * x2)"]
model = ot.SymbolicFunction(input_names, formulas)
distribution = ot.Normal(dimension)
x = distribution.getSample(100)
y = model(x)

# %%
# We create a :class:`~openturns.GaussianProcessFitter` based on a quadratic basis.
# The `run` method estimates the coefficients of the trend and the hyperparameters of the covariance model.

# %%
basis = ot.QuadraticBasisFactory(dimension).build()
covarianceModel = ot.SquaredExponential([1] * dimension, [1.0])
algo = ot.GaussianProcessFitter(x, y, covarianceModel, basis)
algo.run()
result = algo.getResult()

# %%
# We see that the trend coefficients have been estimated.

# %%
result.getTrendCoefficients()

# %%
# The parameters of the covariance models also have been estimated.

# %%
result.getCovarianceModel()

# %%
# The `getMetaModel` method returns the metamodel where the parameters have been estimated.

# %%
responseSurface = result.getMetaModel()

# %%
# Plot the output of our model depending on :math:`x_2` with :math:`x_1=0.5`.

# %%
xmin = -5.0
xmax = 5.0
x1value = 0.5
parametricModelGraph = ot.ParametricFunction(model, [0], [x1value]).draw(xmin, xmax)
graphMetamodel = ot.ParametricFunction(responseSurface, [0], [x1value]).draw(xmin, xmax)
parametricModelGraph.add(graphMetamodel)
parametricModelGraph.setLegends(["Model", "Meta-Model"])
parametricModelGraph.setLegendPosition("upper right")
view = otv.View(parametricModelGraph)

# %%
view.ShowAll()
