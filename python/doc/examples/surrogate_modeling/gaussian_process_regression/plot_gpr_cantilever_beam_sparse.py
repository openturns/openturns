"""
Sparse Gaussian Process Regression : cantilever beam model
==========================================================
"""

# %%
# In this example, we create a sparse Gaussian Process Regression (GPR) metamodel (refer to
# :ref:`gaussian_process_regression`) of the :ref:`cantilever beam <use-case-cantilever-beam>`.
# We use a squared exponential covariance kernel for the Gaussian process. In order to estimate
# the hyper-parameters, we use a design of experiments of size 200 and 30 inducing points.
#
# The sparse Gaussian process, also known as sparse variational Gaussian process (SVGP),
# relies on a variational approximation of the posterior, based on a reduced set of :math:`m`
# inducing points. It is a good alternative to the exact
# :class:`~openturns.GaussianProcessRegression` when the training sample is large, since the
# cost of the exact method scales with the cube of the sample size while the complexity of the
# sparse model depends on the number of inducing points only, with a complexity scaling
# as :math:`n \times m^2` where :math:`n` is the sample size and :math:`m` the number of
# inducing points. Unlike the exact GPR, the sparse
# model explicitly takes into account an independent Gaussian noise on the observations, with
# variance estimated during the fitting.
from openturns.usecases import cantilever_beam
import openturns as ot
import openturns.viewer as otv
from openturns.experimental import SparseGaussianProcessFitter
from openturns.experimental import SparseGaussianProcessRegression

ot.RandomGenerator.SetSeed(0)

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
# We consider a simple Monte Carlo sample as a design of experiments.
# This is why we generate an input sample using the method :meth:`~openturns.Distribution.getSample` of the
# distribution. Then we evaluate the output using the `model` function.
#
# We perturb the outputs with a Gaussian noise of standard deviation 0.005 cm, in order to
# illustrate the noise model handled by the sparse Gaussian process.
sampleSize_train = 200
X_train = myDistribution.getSample(sampleSize_train)
noise_std = 0.005
Y_train = model(X_train) + ot.Normal(0.0, noise_std).getSample(sampleSize_train)

# %%
# The following figure presents the distribution of the vertical deviations Y on the training sample. We observe that the large deviations occur less often.
histo = ot.HistogramFactory().build(Y_train).drawPDF()
histo.setXTitle("Vertical deviation (cm)")
histo.setTitle("Distribution of the vertical deviation")
histo.setLegends([""])
view = otv.View(histo)

# %%
# The four input variables have very different orders of magnitude, from :math:`10^{-7}` to
# :math:`10^{10}`. Such heterogeneous scales make the optimization of the hyperparameters
# numerically difficult. We standardize each input variable to the interval :math:`[0, 1]`
# before fitting the model.
X_train_min = X_train.getMin()
X_train_range = X_train.computeRange()
X_train_std = (X_train - X_train_min) / X_train_range

# %%
# Create the metamodel
# --------------------
#
# In order to create the sparse GPR metamodel, we use a squared exponential covariance kernel.
# The :class:`~openturns.SquaredExponential` kernel has one amplitude coefficient and 4 scale coefficients.
# This is because this covariance kernel is anisotropic : each of the 4 input variables is associated with its own scale coefficient.
covarianceModel = ot.SquaredExponential(cb.dim)

# %%
# Finally, we use the :class:`~openturns.experimental.SparseGaussianProcessFitter` class to create the sparse GPR metamodel.
# It requires a training sample, a covariance kernel and the number of inducing points as input arguments.
# The inducing points are deterministically selected as a subset of the training sample.
# The :meth:`~openturns.experimental.SparseGaussianProcessFitter.run` method optimizes the Gaussian process
# hyperparameters and the noise variance by maximizing the collapsed variational bound (ELBO).
# The inducing points are not optimized by default: this is controlled by the
# :meth:`~openturns.experimental.SparseGaussianProcessFitter.setOptimizeInducingPoints` method.
m = 30
fitter_algo = SparseGaussianProcessFitter(X_train_std, Y_train, covarianceModel, m)
fitter_algo.run()
fitter_result = fitter_algo.getResult()

# %%
# We can print the hyperparameters of the covariance model, which have been estimated by maximizing the ELBO,
# as well as the estimated noise standard deviation and the optimal value of the ELBO.
print(fitter_result.getCovarianceModel())
print(fitter_result.getNoiseStdDev())
print(fitter_result.getOptimalELBO())

# %%
# The estimated noise standard deviation is close to the value 0.005 used to generate the data.
# The number of inducing points is smaller than the number of observations, which is the
# cornerstone of the sparse approximation: the model complexity depends on :math:`m`, not on
# the sample size.
print(fitter_result.getInducingPoints().getSize())

# %%
# Then, we condition the sparse Gaussian process to the data with the
# :class:`~openturns.experimental.SparseGaussianProcessRegression` class. It requires a
# sparse Gaussian process fitter result as input argument.
gpr_algo = SparseGaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()
gprMetamodel = gpr_result.getMetaModel()

# %%
# The conditional variance of the prediction at any point is given by the
# :meth:`~openturns.experimental.SparseGaussianProcessFitterResult.getConditionalVariance` method.
conditionalVariance = gpr_result.getConditionalVariance(X_train_std[:5])
print(conditionalVariance)

# %%
# Validate the metamodel
# ----------------------
#
# We finally want to validate the sparse GPR metamodel. This is why we generate a validation sample with size 100 and we evaluate the output of the model on this sample.
sampleSize_test = 100
X_test = myDistribution.getSample(sampleSize_test)
Y_test = model(X_test)

# %%
# The validation inputs must be standardized in the same way as the training inputs.
X_test_std = (X_test - X_train_min) / X_train_range

# %%
# The class :class:`~openturns.MetaModelValidation`  makes the validation easy. To create it, we use the validation samples and the metamodel.
val = ot.MetaModelValidation(Y_test, gprMetamodel(X_test_std))

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
view = otv.View(graph)

# %%
# We observe that the negative residuals occur with nearly the same frequency of the positive residuals: this is a first sign of good quality.
# The method :meth:`~openturns.MetaModelValidation.drawValidation` compares the observed outputs and the metamodel outputs.
graph = val.drawValidation()
graph.setTitle("R2 = %.2f%%" % (100 * R2))
view = otv.View(graph)

# %%
# Display all figures
otv.View.ShowAll()
