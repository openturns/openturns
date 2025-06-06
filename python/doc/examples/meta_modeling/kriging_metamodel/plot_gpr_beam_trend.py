"""
Gaussian Process Regression: choose a polynomial trend on the beam model
========================================================================
"""

# %%
# In this example, we consider the :ref:`cantilever beam <use-case-cantilever-beam>` and we build a metamodel using a
# Gaussian process regression whose trend is estimated on the
# given data set. We illustrate the impact of the choice of the trend function basis on the metamodel.
# This example focuses on three polynomial trends:
#
# * :class:`~openturns.ConstantBasisFactory`;
# * :class:`~openturns.LinearBasisFactory`;
# * :class:`~openturns.QuadraticBasisFactory`.
#
# In the :doc:`/auto_meta_modeling/kriging_metamodel/plot_gpr_choose_trend` example,
# we give another example of this procedure.
from openturns.usecases import cantilever_beam
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

ot.Log.Show(ot.Log.NONE)

# %%
# Definition of the model
# -----------------------
#
# We load the use case.
cb = cantilever_beam.CantileverBeam()

# %%
# We define the function which evaluates the output depending on the inputs.
model = cb.model

# %%
# Then we define the distribution of the input random vector.
dimension = cb.dim  # number of inputs
input_dist = cb.distribution

# %%
# Create the design of experiments
# --------------------------------
#
# We consider a simple Monte-Carlo sampling as a design of experiments.
# This is why we generate an input sample using the :meth:`~openturns.Distribution.getSample` method of the distribution.
# Then we evaluate the output using the `model` function.

# %%
sampleSize_train = 10
X_train = input_dist.getSample(sampleSize_train)
Y_train = model(X_train)

# %%
# Constant basis
# --------------
#
# In this paragraph we choose a basis constant for the estimation of the trend.
# The basis is built with the :class:`~openturns.ConstantBasisFactory` class.
basis = ot.ConstantBasisFactory(dimension).build()

# %%
# In order to create the Gaussian Process Regression metamodel, we use a squared exponential covariance kernel.
# The :class:`~openturns.SquaredExponential` kernel has one amplitude coefficient and 4 scale coefficients.
# This is because this covariance kernel is anisotropic : each of the 4 input variables is associated with its own scale coefficient.
covariance_model = ot.SquaredExponential(dimension)

# %%
# The optimization algorithm is quite good at setting optimization bounds. In this case, however, the range of the input domain is extreme,
# as we can see below.
print("Lower and upper bounds of X_train:")
print(X_train.getMin(), X_train.getMax())

# %%
# Thus, we need to manually define sensible optimization bounds.
# Note that since the amplitude parameter is computed analytically (this is possible when the output dimension is 1),
# we only need to set bounds on the scale parameter.
scaleOptimizationBounds = ot.Interval(
    [1.0, 1.0, 1.0, 1.0e-10], [1.0e11, 1.0e3, 1.0e1, 1.0e-5]
)

# %%
# To create the Gaussian Process Regression metamodel, we first build the :math:`Y(\omega, x)` Gaussian process with the class
# :class:`~openturns.experimental.GaussianProcessFitter`. It requires a training sample, a covariance kernel and a
# trend basis as input arguments.
#
# We need to set the initial scale parameter for the optimization. The upper bound of the input domain is a sensitive choice here.
# We must not forget to actually set the optimization bounds defined above.
covariance_model.setScale(X_train.getMax())
algo_fit = otexp.GaussianProcessFitter(X_train, Y_train, covariance_model, basis)
algo_fit.setOptimizationBounds(scaleOptimizationBounds)
algo_fit.run()
fit_result = algo_fit.getResult()

# %%
# Then, we condition the process :math:`Y(\omega, x)` to the data set with the class
# :class:`~openturns.experimental.GaussianProcessRegression`.
algo_gpr = otexp.GaussianProcessRegression(fit_result)
algo_gpr.run()

# %%
# Get the Gaussian Process Regression metamodel.
gpr_result_cst = algo_gpr.getResult()
metamodel_cst = gpr_result_cst.getMetaModel()

# %%
# The :meth:`~openturns.experimental.GaussianProcessRegressionResult.getTrendCoefficients` method returns the coefficients of the trend.
# The constant trend always has only one coefficient (if there is one single output).
print(gpr_result_cst.getTrendCoefficients())

# %%
# We can check the estimated covariance model.
print(gpr_result_cst.getCovarianceModel())

# %%
# Linear basis
# ------------
#
# In this paragraph we choose a linear basis for the estimation of the trend.
# The basis is built with the :class:`~openturns.LinearBasisFactory` class. The same methodology is followed:
# we do not detail it.
basis = ot.LinearBasisFactory(dimension).build()
algo_fit = otexp.GaussianProcessFitter(X_train, Y_train, covariance_model, basis)
algo_fit.setOptimizationBounds(scaleOptimizationBounds)
algo_fit.run()
fit_result = algo_fit.getResult()
algo_gpr = otexp.GaussianProcessRegression(fit_result)
algo_gpr.run()
gpr_result_lin = algo_gpr.getResult()
metamodel_lin = gpr_result_lin.getMetaModel()
print(gpr_result_lin.getTrendCoefficients())
print(gpr_result_lin.getCovarianceModel())

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
# * dim = 4 coefficients for the linear part.
#
#
# Quadratic basis
# ---------------
#
# In this paragraph we choose a quadratic basis for the estimation of the trend.
# The basis is built with the :class:`~openturns.QuadraticBasisFactory` class. The same methodology is followed:
# we do not detail it.
#
# However we can see that the default optimization algorithm which is :class:`~openturns.Cobyla` does not manage to converge.
# Thus, we can either:
#
# * change the default optimization algorithm and select for example the :class:`~openturns.TNC`
#   algorithm (Truncated Newton Constrained) using the entry of :class:`~openturns.ResourceMap` called
#   *GaussianProcessFitter-DefaultOptimizationAlgorithm*: *ot.ResourceMap.SetAsString("GaussianProcessFitter-
#   DefaultOptimizationAlgorithm", "TNC")*,
# * or keep the default optimization algorithm but change the default maximum constrainte error value which is equal to
#   :math:`10^{-5}`. We move it to :math:`10^{-6}`  using the entry of :class:`~openturns.ResourceMap` called
#   *OptimizationAlgorithm-DefaultMaximumConstraintError*: *ot.ResourceMap.SetAsScalar("OptimizationAlgorithm
#   -DefaultMaximumConstraintError", 1e-6)*.
#
ot.ResourceMap.SetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError", 1e-6)
basis = ot.QuadraticBasisFactory(dimension).build()
algo_fit = otexp.GaussianProcessFitter(X_train, Y_train, covariance_model, basis)
algo_fit.setOptimizationBounds(scaleOptimizationBounds)
algo_fit.run()
fit_result = algo_fit.getResult()
algo_gpr = otexp.GaussianProcessRegression(fit_result)
algo_gpr.run()
gpr_result_quad = algo_gpr.getResult()
metamodel_quad = gpr_result_quad.getMetaModel()
print(gpr_result_quad.getTrendCoefficients())
print(gpr_result_quad.getCovarianceModel())

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
#    \frac{dim \times (dim+1)}{2}=\frac{4\times 5}{2}=10
#
#
# coefficients, associated with the symmetric matrix of the quadratic function.
#
# Validate the metamodel
# ----------------------
#
# We finally want to validate the Gaussian Process Regression metamodel. This is why we generate a validation sample
# with size 100 and we evaluate the output of the model on this sample.
sampleSize_test = 100
X_test = input_dist.getSample(sampleSize_test)
Y_test = model(X_test)

# %%
# We define a function to easily draw the QQ-plot graphs.


def drawMetaModelValidation(X_test, Y_test, metamodel_gpr, title):
    metamodel_predictions = metamodel_gpr(X_test)
    val = ot.MetaModelValidation(Y_test, metamodel_predictions)
    r2Score = val.computeR2Score()[0]
    graph = val.drawValidation().getGraph(0, 0)
    graph.setLegends([""])
    graph.setLegends(["%s, R2 = %.2f%%" % (title, 100 * r2Score), ""])
    graph.setLegendPosition("upper left")
    return graph


# %%
# We plot here the validation graph for each metamodel.
grid = ot.GridLayout(1, 3)
grid.setTitle("Different trends")
graphConstant = drawMetaModelValidation(X_test, Y_test, metamodel_cst, "Constant")
graphLinear = drawMetaModelValidation(X_test, Y_test, metamodel_lin, "Linear")
graphQuadratic = drawMetaModelValidation(X_test, Y_test, metamodel_quad, "Quadratic")
grid.setGraph(0, 0, graphConstant)
grid.setGraph(0, 1, graphLinear)
grid.setGraph(0, 2, graphQuadratic)
view = otv.View(grid)

# %%
# We observe that the three trends perform very well in this case.
# With more coefficients, the Gaussian Process Regression metamodel is more flexibile and can adjust better to the training sample.
# This does not mean, however, that the trend coefficients will provide a good fit for the validation sample.
#
# The number of parameters in each Gaussian Process Regression metamodel is the following :
#
# * with the constant trend, we have 6 coefficients to estimate: 5 coefficients for the covariance matrix and 1 coefficient for the trend,
# * with the  linear trend, we have 10 coefficients to estimate: 5 coefficients for the covariance matrix and 5 coefficients for the trend,
# * with the quadratic trend, we have 20 coefficients to estimate: 5 coefficients for the covariance matrix and 15 coefficients for the trend.
#
# In the cantilever beam example, fitting the metamodel to a training sample with only 10 points is made much easier because the function to approximate is almost linear.
# In this case, a quadratic trend is not advisable because it can interpolate all points in the training sample.


# %%
# Display figures
otv.View.ShowAll()
