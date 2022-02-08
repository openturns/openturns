"""
Bayesian calibration of a computer code
=======================================
"""
# %%
# In this example we are going to compute the parameters of a computer model thanks to Bayesian estimation.
#
# Let us denote :math:`(y_1, \dots, y_n)` the observation sample,
# :math:`(\vect z_1, \ldots, \vect z_n) = (f(x_1|\vect\theta), \ldots, f(x_n|\vect\theta))` the model prediction,
# :math:`p(y |\vect z)` the density function of observation :math:`y`
# conditional on model prediction :math:`\vect z`,
# and :math:`\vect\theta \in \mathbb{R}^p` the calibration parameters we wish to estimate.
#
#
# The posterior distribution is given by Bayes theorem:
#
# .. math::\pi(\vect\theta | \vect y) \quad \propto \quad L\left(\vect y | \vect\theta\right) \times \pi(\vect\theta):math:``
#
# where :math:`\propto` means "proportional to", regarded as a function of :math:`\vect\theta`.
#
# The posterior distribution is approximated here by the empirical distribution of the sample :math:`\vect\theta^1, \ldots, \vect\theta^N` generated by the Metropolis-Hastings algorithm. This means that any quantity characteristic of the posterior distribution (mean, variance, quantile, ...) is approximated by its empirical counterpart.
#
# Our model (i.e. the compute code to calibrate) is a standard normal linear regression, where
#
# .. math::
#    y_i = \theta_1 + x_i \theta_2 + x_i^2 \theta_3 + \varepsilon_i
#
# where :math:`\varepsilon_i \stackrel{i.i.d.}{\sim} \mathcal N(0, 1)`.
#
# The "true" value of :math:`\theta` is:
#
# .. math::
#    \vect \theta_{true} = (-4.5,4.8,2.2)^T.
#
#
# We use a normal prior on :math:`\vect\theta`:
#
# .. math::
#    \pi(\vect\theta) = \mathcal N(\vect{\mu}_\vect{\theta}, \mat{\Sigma}_\vect{\theta})
#
# where
#
# .. math::
#     \vect{\mu}_\vect{\theta} =
#     \begin{pmatrix}
#      -3 \\
#       4 \\
#       1
#     \end{pmatrix}
#
# is the mean of the prior and
#
# .. math::
#    \mat{\Sigma}_\vect{\theta} =
#    \begin{pmatrix}
#      \sigma_{\theta_1}^2 & 0 & 0 \\
#      0 & \sigma_{\theta_2}^2 & 0 \\
#      0 & 0 & \sigma_{\theta_3}^2
#    \end{pmatrix}
#
# is the prior covariance matrix with
#
# .. math::
#    \sigma_{\theta_1} = 2, \qquad \sigma_{\theta_2} = 1, \qquad \sigma_{\theta_3} = 1.5.
#
# The following objects need to be defined in order to perform Bayesian calibration:
#
# - The conditional density :math:`p(y|\vect z)` must be defined as a probability distribution.
# - The computer model must be implemented thanks to the ParametricFunction class.
#   This takes a value of :math:`\vect\theta` as input, and outputs the vector of model predictions :math:`\vect z`,
#   as defined above (the vector of covariates :math:`\vect x = (x_1, \ldots, x_n)` is treated as a known constant).
#   When doing that, we have to keep in mind that :math:`\vect z` will be used as the vector of parameters corresponding
#   to the distribution specified for :math:`p(y |\vect z)`. For instance, if :math:`p(y|\vect z)` is normal,
#   this means that :math:`\vect z` must be a vector containing the mean and standard deviation of :math:`y`.
# - The prior density :math:`\pi(\vect\theta)` encoding the set of possible values for the calibration parameters,
#   each value being weighted by its a priori probability, reflecting the beliefs about the possible values
#   of :math:`\vect\theta` before consideration of the experimental data.
#   Again, this is implemented as a probability distribution.
# - Metropolis-Hastings algorithm(s), possibly used in tandem with a Gibbs algorithm
#   in order to sample from the posterior distribution of the calibration parameters.

# %%
import pylab as pl
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Dimension of the vector of parameters to calibrate
paramDim = 3
# The number of obesrvations
obsSize = 10

# %%
# Define the observed inputs :math:`x_i`

# %%
xmin = -2.
xmax = 3.
step = (xmax-xmin)/(obsSize-1)
rg = ot.RegularGrid(xmin, step, obsSize)
x_obs = rg.getVertices()

# %%
# Define the parametric model :math:`\vect z = f(x,\vect\theta)` that associates each
# observation :math:`x` and value of :math:`\vect \theta` to the parameters
# of the distribution of the corresponding observation :math:`y`:
# here :math:`\vect z=(\mu, \sigma)` where :math:`\mu`,
# the first output of the model, is the mean and :math:`\sigma`,
# the second output of the model, is the standard deviation.

# %%
fullModel = ot.SymbolicFunction(
    ['x', 'theta1', 'theta2', 'theta3'], ['theta1+theta2*x+theta3*x^2', '1.0'])

# %%
# To differentiate between the two classes of inputs (:math:`x` and :math:`\vect\theta`),
# we define a :class:`~openturns.ParametricFunction` from `fullModel`
# and make the first input (the observations :math:`x`) its *parameter*:
# :math:`f_x(\vect \theta) := f(x, \vect \theta)`.
# We set :math:`x = 1` as a placeholder,
# but :math:`x` will actually take the values :math:`x_i` of the observations
# when we sample :math:`\vect\theta`.

linkFunction = ot.ParametricFunction(fullModel, [0], [1.0])
print(linkFunction)

# %%
# Define the observation noise :math:`\varepsilon {\sim} \mathcal N(0, 1)` and create a sample from it.

# %%
ot.RandomGenerator.SetSeed(0)
noiseStandardDeviation = 1.
noise = ot.Normal(0, noiseStandardDeviation)
noiseSample = noise.getSample(obsSize)

# %%
# Define the vector of observations :math:`y_i`,
# here sampled using the "true" value of :math:`\vect \theta`: :math:`\vect \theta_{true}`.

# %%
thetaTrue = [-4.5, 4.8, 2.2]

# %%
y_obs = ot.Sample(obsSize, 1)
for i in range(obsSize):
    linkFunction.setParameter(x_obs[i])
    y_obs[i, 0] = linkFunction(thetaTrue)[0] + noiseSample[i, 0]

# %%
# Draw the model predictions vs the observations.

# %%
functionnalModel = ot.ParametricFunction(fullModel, [1, 2, 3], thetaTrue)
graphModel = functionnalModel.getMarginal(0).draw(xmin, xmax)
observations = ot.Cloud(x_obs, y_obs)
observations = ot.Cloud(x_obs, y_obs)
observations.setColor("red")
graphModel.add(observations)
graphModel.setLegends(["Model", "Observations"])
graphModel.setLegendPosition("topleft")
view = viewer.View(graphModel)

# %%
# Define the distribution of observations :math:`\vect{y} | \vect{z}` conditional on model predictions.
#
# Note that its parameter dimension is the one of :math:`\vect{z}`, so the model must be adjusted accordingly.

# %%
conditional = ot.Normal()

# %%
# Define the mean :math:`\mu_\theta`, the covariance matrix :math:`\Sigma_\theta`, then the prior distribution :math:`\pi(\vect\theta)` of the parameter :math:`\vect\theta`.

# %%
thetaPriorMean = [-3., 4., 1.]

# %%
sigma0 = [2., 1., 1.5]  # standard deviations
thetaPriorCovarianceMatrix = ot.CovarianceMatrix(paramDim)
for i in range(paramDim):
    thetaPriorCovarianceMatrix[i, i] = sigma0[i]**2

prior = ot.Normal(thetaPriorMean, thetaPriorCovarianceMatrix)
prior.setDescription(['theta1', 'theta2', 'theta3'])

# %%
# The proposed steps for
# :math:`\theta_1`, :math:`\theta_2` and :math:`\theta_3`
# will all follow a uniform distribution.

proposal = ot.Uniform(-1., 1.)

# %%
# Test the Metropolis-Hastings sampler
# ------------------------------------

# %%
# Creation of a single component random walk Metropolis-Hastings (RWMH) sampler.
# This involves a combination of the RWMH and the Gibbs algorithms.

# %%
initialState = thetaPriorMean

# %%
# We create a :class:`~openturns.RandomWalkMetropolisHastings` sampler for each component.
# Each sampler must be aware of the joint prior distribution.
# We also use the same proposal distribution, but this is not mandatory.

mh_coll = [ot.RandomWalkMetropolisHastings(prior, initialState, proposal, [i]) for i in range(paramDim)]

# %%
# Each sampler must be made aware of the likelihood.
# Otherwise we would sample from the prior!

for mh in mh_coll: mh.setLikelihood(conditional, y_obs, linkFunction, x_obs)

# %%
# Finally, the :class:`~openturns.Gibbs` algorithm is constructed from all Metropolis-Hastings samplers.

sampler = ot.Gibbs(mh_coll)

# %%
# Tuning of the Gibbs algorithm:

# %%
sampler.setThinning(1)
sampler.setBurnIn(2000)

# %%
# Generate a sample from the posterior distribution of the parameters :math:`\vect \theta`.

# %%
sampleSize = 10000
sample = sampler.getSample(sampleSize)

# %%
# Look at the acceptance rate (basic check of the sampling efficiency:
# values close to :math:`0.2` are usually recommended
# for Normal posterior distributions).

# %%
[mh.getAcceptanceRate() for mh in sampler.getMetropolisHastingsCollection()]

# %%
# Build the distribution of the posterior by kernel smoothing.

# %%
kernel = ot.KernelSmoothing()
posterior = kernel.build(sample)

# %%
# Display prior vs posterior for each parameter.

# %%

fig = pl.figure(figsize=(12, 4))

for parameter_index in range(paramDim):
    graph = posterior.getMarginal(parameter_index).drawPDF()
    priorGraph = prior.getMarginal(parameter_index).drawPDF()
    priorGraph.setColors(['blue'])
    graph.add(priorGraph)
    graph.setLegends(['Posterior', 'Prior'])
    ax = fig.add_subplot(1, paramDim, parameter_index+1)
    _ = ot.viewer.View(graph, figure=fig, axes=[ax])

_ = fig.suptitle("Bayesian calibration")

plt.show()
