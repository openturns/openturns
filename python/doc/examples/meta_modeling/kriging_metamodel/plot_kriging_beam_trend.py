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
# Create the design of experiments
# --------------------------------

# %%
# We consider a simple Monte-Carlo sampling as a design of experiments. This is why we generate an input sample using the `getSample` method of the distribution. Then we evaluate the output using the `model` function.

# %%
sampleSize_train = 10
X_train = myDistribution.getSample(sampleSize_train)
Y_train = model(X_train)

# %%
# Create the metamodel
# --------------------

# %%
# In order to create the Kriging metamodel, we first select a constant trend with the `ConstantBasisFactory` class. Then we use a squared exponential covariance kernel.
# The `SquaredExponential` kernel has one amplitude coefficient and 4 scale coefficients. This is because this covariance kernel is anisotropic : each of the 4 input variables is associated with its own scale coefficient. 

# %%
basis = ot.ConstantBasisFactory(dimension).build()
covarianceModel = ot.SquaredExponential(dimension)

# %%
# Typically, the optimization algorithm is quite good at setting sensible optimization bounds.
# In this case, however, the range of the input domain is extreme.

# %%
print("Lower and upper bounds of X_train:")
print(X_train.getMin(), X_train.getMax())

# %%
# We need to manually define sensible optimization bounds.
# Note that since the amplitude parameter is computed analytically (this is possible when the output dimension is 1), we only need to set bounds on the scale parameter.

# %%
scaleOptimizationBounds = ot.Interval([1.0, 1.0, 1.0, 1.0e-10], [1.0e11, 1.0e3, 1.0e1, 1.0e-5])

# %%
# Finally, we use the `KrigingAlgorithm` class to create the Kriging metamodel.
# It requires a training sample, a covariance kernel and a trend basis as input arguments. 
# We need to set the initial scale parameter for the optimization. The upper bound of the input domain is a sensible choice here.
# We must not forget to actually set the optimization bounds defined above.

# %%
covarianceModel.setScale(X_train.getMax())
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.setOptimizationBounds(scaleOptimizationBounds)

# %%
# Run the algorithm and get the result.

# %%
algo.run()
result = algo.getResult()
krigingWithConstantTrend = result.getMetaModel()

# %%
# The `getTrendCoefficients` method returns the coefficients of the trend.

# %%
print(result.getTrendCoefficients())

# %%
# The constant trend always has only one coefficient (if there is one single output).

# %%
print(result.getCovarianceModel())

# %%
# Setting the trend
# -----------------

# %%
covarianceModel.setScale(X_train.getMax())
basis = ot.LinearBasisFactory(dimension).build()
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.setOptimizationBounds(scaleOptimizationBounds)
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
covarianceModel.setScale(X_train.getMax())
basis = ot.QuadraticBasisFactory(dimension).build()
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.setOptimizationBounds(scaleOptimizationBounds)
algo.run()
result = algo.getResult()
krigingWithQuadraticTrend = result.getMetaModel()
result.getTrendCoefficients()
print(algo.getOptimizationBounds())
print(result.getCovarianceModel())

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
# We finally want to validate the Kriging metamodel. This is why we generate a validation sample with size 100 and we evaluate the output of the model on this sample.

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
graphConstant = drawMetaModelValidation(X_test, Y_test, krigingWithConstantTrend, "Constant")
_ = View(graphConstant, figure=fig, axes=[ax1])
ax2 = fig.add_subplot(1, 3, 2)
graphLinear = drawMetaModelValidation(X_test, Y_test, krigingWithLinearTrend, "Linear")
_ = View(graphLinear, figure=fig, axes=[ax2])
ax3 = fig.add_subplot(1, 3, 3)
graphQuadratic = drawMetaModelValidation(X_test, Y_test, krigingWithQuadraticTrend, "Quadratic")
_ = View(graphQuadratic, figure=fig, axes=[ax3])
_ = fig.suptitle("Different trends")
plt.show()

# %%
# We observe that the three trends perform very well in this case. With more coefficients, the Kriging metamodel is more flexibile and can adjust better to the training sample. This does not mean, however, that the trend coefficients will provide a good fit for the validation sample. 
#
# The number of parameters in each Kriging metamodel is the following:
#
# * the constant trend Kriging has 6 coefficients to estimate: 5 coefficients for the covariance matrix and 1 coefficient for the trend,
# * the linear trend Kriging has 10 coefficients to estimate: 5 coefficients for the covariance matrix and 5 coefficients for the trend,
# * the quadratic trend Kriging has 20 coefficients to estimate: 5 coefficients for the covariance matrix and 15 coefficients for the trend.
#
# In the cantilever beam example, fitting the metamodel to a training sample with only 10 points is made much easier because the function to approximate is almost linear.
# In this case, a quadratic trend is not advisable because it can interpolate all points in the training sample.
