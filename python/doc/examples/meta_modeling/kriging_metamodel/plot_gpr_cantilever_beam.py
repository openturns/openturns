"""
Gaussian Process Regression : cantilever beam model
===================================================
"""

# %%
# In this example, we create a Gaussian Process Regression (GPR) metamodel of the :ref:`cantilever beam <use-case-cantilever-beam>`.
# We use a squared exponential covariance kernel for the Gaussian process. In order to estimate the hyper-parameters, we use a design of experiments of size 20.
from openturns.usecases import cantilever_beam
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as viewer

ot.Log.Show(ot.Log.NONE)
# sphinx_gallery_thumbnail_number = 3

# %%
# Definition of the model
# -----------------------
#
# We load the cantilever beam use case :
cb = cantilever_beam.CantileverBeam()

# %%
# We define the function which evaluates the output depending on the inputs.
model = cb.model

# %%
# Then we define the distribution of the input random vector.
myDistribution = cb.distribution

# %%
# Create the design of experiments
# --------------------------------
#
# We consider a simple Monte-Carlo sample as a design of experiments.
# This is why we generate an input sample using the method :meth:`~openturns.Distribution.getSample` of the
# distribution. Then we evaluate the output using the `model` function.

# %%
sampleSize_train = 20
X_train = myDistribution.getSample(sampleSize_train)
Y_train = model(X_train)

# %%
# The following figure presents the distribution of the vertical deviations Y on the training sample. We observe that the large deviations occur less often.
histo = ot.HistogramFactory().build(Y_train).drawPDF()
histo.setXTitle("Vertical deviation (cm)")
histo.setTitle("Distribution of the vertical deviation")
histo.setLegends([""])
view = viewer.View(histo)

# %%
# Create the metamodel
# --------------------
#
# In order to create the GPR metamodel, we first select a constant trend with the :class:`~openturns.ConstantBasisFactory` class. Then we use a squared exponential covariance kernel.
# The :class:`~openturns.SquaredExponential` kernel has one amplitude coefficient and 4 scale coefficients.
# This is because this covariance kernel is anisotropic : each of the 4 input variables is associated with its own scale coefficient.
basis = ot.ConstantBasisFactory(cb.dim).build()
covarianceModel = ot.SquaredExponential(cb.dim)

# %%
# Typically, the optimization algorithm is quite good at setting sensible optimization bounds.
# In this case, however, the range of the input domain is extreme.
print("Lower and upper bounds of X_train:")
print(X_train.getMin(), X_train.getMax())

# %%
# We need to manually define sensible optimization bounds.
# Note that since the amplitude parameter is computed analytically (this is possible when the output dimension is 1), we only need to set bounds on the scale parameter.
scaleOptimizationBounds = ot.Interval(
    [1.0, 1.0, 1.0, 1.0e-10], [1.0e11, 1.0e3, 1.0e1, 1.0e-5]
)

# %%
# Finally, we use the :class:`~openturns.experimental.GaussianProcessFitter` and `GaussianProcessRegression` classes to create the GPR metamodel.
# It requires a training sample, a covariance kernel and a trend basis as input arguments.
# We need to set the initial scale parameter for the optimization. The upper bound of the input domain is a sensible choice here.
# We must not forget to actually set the optimization bounds defined above.
covarianceModel.setScale(X_train.getMax())
fitter_algo = otexp.GaussianProcessFitter(X_train, Y_train, covarianceModel, basis)
fitter_algo.setOptimizationBounds(scaleOptimizationBounds)


# %%
# The method :meth:`~openturns.experimental.GaussianProcessFitter.run`  of
# the class :class:`~openturns.experimental.GaussianProcessFitter.run` optimizes the Gaussian process
# hyperparameters and the method :meth:`~openturns.experimental.GaussianProcessRegression.run` of the class
# :class:`~openturns.experimental.GaussianProcessRegression` conditions the
# Gaussian process to the data set.
#
# We can then print the constant trend of the metamodel, estimated using the least squares method.
fitter_algo.run()
fitter_result = fitter_algo.getResult()
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()
gprMetamodel = gpr_result.getMetaModel()

# %%
# The method :meth:`~openturns.experimental.GaussianProcessRegressionResult.getTrendCoefficients` of
# the class
# :class:`~openturns.experimental.GaussianProcessRegressionResult` returns the coefficients of the trend.
print(gpr_result.getTrendCoefficients())

# %%
# We can also print the hyperparameters of the covariance model, which have been estimated by maximizing the likelihood.
gpr_result.getCovarianceModel()

# %%
# Validate the metamodel
# ----------------------
#
# We finally want to validate the GPR metamodel. This is why we generate a validation sample with size 100 and we evaluate the output of the model on this sample.
sampleSize_test = 100
X_test = myDistribution.getSample(sampleSize_test)
Y_test = model(X_test)

# %%
# The class :class:`~openturns.MetaModelValidation`  makes the validation easy. To create it, we use the validation samples and the metamodel.
val = ot.MetaModelValidation(Y_test, gprMetamodel(X_test))

# %%
# The method :meth:`~openturns.MetaModelValidation.computeR2Score` computes the R2 score.
R2 = val.computeR2Score()[0]
print(R2)

# %%
# The residuals are the difference between the model and the metamodel.
r = val.getResidualSample()
graph = ot.HistogramFactory().build(r).drawPDF()
graph.setXTitle("Residuals (cm)")
graph.setTitle("Distribution of the residuals")
graph.setLegends([""])
view = viewer.View(graph)

# %%
# We observe that the negative residuals occur with nearly the same frequency of the positive residuals: this is a first sign of good quality.
# The method :meth:`~openturns.MetaModelValidation.drawValidation` compares the observed outputs and the metamodel outputs.
graph = val.drawValidation()
graph.setTitle("R2 = %.2f%%" % (100 * R2))
view = viewer.View(graph)

# %%
# Display all figures
viewer.View.ShowAll()
