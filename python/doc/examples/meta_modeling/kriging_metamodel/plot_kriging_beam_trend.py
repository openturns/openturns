"""
Configuring the trend in Kriging
================================
"""
# %%
# The goal of this example is to show how to configure the trend in a Kriging metamodel. This is why this example focuses on the three available trends:
#
# * `ConstantBasisFactory`,
# * `LinearBasisFactory`,
# * `QuadraticBasisFactory`.
#
# For this purpose, we use the :ref:`cantilever beam <use-case-cantilever-beam>` example.

# %%
# Definition of the model
# -----------------------

# %%
import openturns as ot
ot.RandomGenerator.SetSeed(0)
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We load the use case :
from openturns.usecases import cantilever_beam as cantilever_beam
cb = cantilever_beam.CantileverBeam()

# %%
# We define the function which evaluates the output depending on the inputs.
model = cb.model

# %%
# Then we define the distribution of the input random vector. 
dimension = cb.dim # number of inputs
myDistribution = cb.distribution

# %%
# We use a transformation because data contain very large values.
transformation = myDistribution.getIsoProbabilisticTransformation()

# %%
# Create the design of experiments
# --------------------------------

# %%
# We consider a simple Monte-Carlo sampling as a design of experiments. This is why we generate an input sample using the `getSample` method of the distribution. Then we evaluate the output using the `model` function.

# %%
sampleSize_train = 20
X_train = myDistribution.getSample(sampleSize_train)
Y_train = model(X_train)

# %%
# Create the metamodel
# --------------------

# %%
# In order to create the kriging metamodel, we first select a constant trend with the `ConstantBasisFactory` class. Then we use a squared exponential covariance model. Finally, we use the `KrigingAlgorithm` class to create the kriging metamodel, taking the training sample, the covariance model and the trend basis as input arguments. 

# %%
covarianceModel = ot.SquaredExponential([1.]*dimension, [1.0])

# %%
basis = ot.ConstantBasisFactory(dimension).build()
algo = ot.KrigingAlgorithm(transformation(X_train), Y_train, covarianceModel, basis)
algo.run()
result = algo.getResult()
krigingWithConstantTrend = result.getMetaModel()

# %%
# The `getTrendCoefficients` method returns the coefficients of the trend.

# %%
result.getTrendCoefficients()

# %%
# The constant trend always has only one coefficient (if there is one single output).

# %%
result.getCovarianceModel()

# %%
# The `SquaredExponential` model has one amplitude coefficient and 4 scale coefficients. This is because this covariance model is anisotropic : each of the 4 input variables is associated with its own scale coefficient. 

# %%
# Setting the trend
# -----------------

# %%
basis = ot.LinearBasisFactory(dimension).build()
algo = ot.KrigingAlgorithm(transformation(X_train), Y_train, covarianceModel, basis)
algo.run()
result = algo.getResult()
krigingWithLinearTrend = result.getMetaModel()
result.getTrendCoefficients()

# %%
# The number of coefficients in the linear and quadratic trends depends on the number of inputs, which is 
# equal to 
#
# .. math::
#    dim = 4 
# 
#
# in the cantilever beam case.
#
# We observe that the number of coefficients in the trend is 5, which corresponds to:
#
# * 1 coefficient for the constant part,
# * dim=4 coefficients for the linear part.

# %%
basis = ot.QuadraticBasisFactory(dimension).build()
algo = ot.KrigingAlgorithm(transformation(X_train), Y_train, covarianceModel, basis)
algo.run()
result = algo.getResult()
krigingWithQuadraticTrend = result.getMetaModel()
result.getTrendCoefficients()

# %%
# This time, the trend has 15 coefficients:
#
# * 1 coefficient for the constant part,
# * 4 coefficients for the linear part,
# * 10 coefficients for the quadratic part.
#
# This is because the number of coefficients in the quadratic part has 
#
# .. math::
#    \frac{dim (dim+1)}{2}=\frac{4\times 5}{2}=10
# 
#
# coefficients, associated with the symmetric matrix of the quadratic function. 

# %%
# Validate the metamodel
# ----------------------

# %%
# We finally want to validate the kriging metamodel. This is why we generate a validation sample which size is equal to 100 and we evaluate the output of the model on this sample.

# %%
sampleSize_test = 100
X_test = myDistribution.getSample(sampleSize_test)
Y_test = model(X_test)


# %%
def drawMetaModelValidation(X_test, Y_test, krigingMetamodel, title):
    val = ot.MetaModelValidation(X_test, Y_test, krigingMetamodel)
    Q2 = val.computePredictivityFactor()[0]
    graph = val.drawValidation().getGraph(0, 0)
    graph.setLegends([""])
    graph.setLegends(["%s, Q2 = %.2f%%" % (title, 100*Q2),""])
    graph.setLegendPosition("topleft")
    return graph


# %%
import pylab as pl
from openturns.viewer import View

# %%
fig = pl.figure(figsize=(12, 4))
ax1 = fig.add_subplot(1, 3, 1)
graphConstant = drawMetaModelValidation(transformation(X_test), Y_test, krigingWithConstantTrend, "Constant")
_ = View(graphConstant, figure=fig, axes=[ax1])
ax2 = fig.add_subplot(1, 3, 2)
graphLinear = drawMetaModelValidation(transformation(X_test), Y_test, krigingWithLinearTrend, "Linear")
_ = View(graphLinear, figure=fig, axes=[ax2])
ax3 = fig.add_subplot(1, 3, 3)
graphQuadratic = drawMetaModelValidation(transformation(X_test), Y_test, krigingWithQuadraticTrend, "Quadratic")
_ = View(graphQuadratic, figure=fig, axes=[ax3])
_ = fig.suptitle("Different trends")
plt.show()

# %%
# We observe that the three trends perform very well in this case. The more the trend has coefficients, the more the kriging metamodel has flexibility to adjust to the training sample. This does not mean, however, that the trend coefficients will provide a good fit for the validation sample. 
#
# The number of parameters in each kriging metamodel is the following:
#
# * the constant trend kriging has 6 coefficients to estimate: 5 coefficients for the covariance matrix and 1 coefficient for the trend,
# * the linear trend kriging has 10 coefficients to estimate: 5 coefficients for the covariance matrix and 5 coefficients for the trend,
# * the quadratic trend kriging has 20 coefficients to estimate: 5 coefficients for the covariance matrix and 15 coefficients for the trend.
#
# In the cantilever beam example, fitting the metamodel with a training sample which size is only equal to 20 is made much easier because the function to approximate is almost linear.
