"""
Bayesian calibration of a computer code
=======================================
"""
# %%
# In this example we are going to compute the parameters of a computer model thanks to Bayesian estimation.
#
# Let us denote :math:`\underline y = (y_1, \dots, y_n)` the observation sample, :math:`\underline z = (f(x_1|\underline{\theta}), \ldots, f(x_n|\underline{\theta}))` the model prediction, :math:`p(y |z)` the density function of observation :math:`y` conditional on model prediction :math:`z`, and :math:`\underline{\theta} \in \mathbb{R}^p` the calibration parameters we wish to estimate.
#
#
# The posterior distribution is given by Bayes theorem:
#
# .. math::\pi(\underline{\theta} | \underline y) \quad \propto \quad L\left(\underline y | \underline{\theta}\right) \times \pi(\underline{\theta}):math:``
#   
# where :math:`\propto` means "proportional to", regarded as a function of :math:`\underline{\theta}`. 
#
# The posterior distribution is approximated here by the empirical distribution of the sample :math:`\underline{\theta}^1, \ldots, \underline{\theta}^N` generated by the Metropolis-Hastings algorithm. This means that any quantity characteristic of the posterior distribution (mean, variance, quantile, ...) is approximated by its empirical counterpart.
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
#    \theta_{true} = (-4.5,4.8,2.2)^T.
# 
#
# We use a normal prior on :math:`\underline{\theta}`:
#
# .. math::
#    \pi(\underline{\theta}) = \mathcal N(\mu_\theta, \Sigma_\theta)
#   
# where
#
# .. math:: 
#     \mu_\theta = 
#     \begin{pmatrix}
#      -3 \\
#       4 \\
#       1
#     \end{pmatrix}
#   
# is the mean of the prior and 
#
# .. math:: 
#    \Sigma_\theta = 
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
# - The conditional density :math:`p(y|z)` must be defined as a probability distribution
# - The computer model must be implemented thanks to the ParametricFunction class.
#   This takes a value of :math:`\underline{\theta}` as input, and outputs the vector of model predictions :math:`\underline z`,
#   as defined above (the vector of covariates :math:`\underline x = (x_1, \ldots, x_n)` is treated as a known constant).
#   When doing that, we have to keep in mind that :math:`z` will be used as the vector of parameters corresponding
#   to the distribution specified for :math:`p(y |z)`. For instance, if :math:`p(y|z)` is normal,
#   this means that :math:`z` must be a vector containing the mean and variance of :math:`y`
# - The prior density :math:`\pi(\underline{\theta})` encoding the set of possible values for the calibration parameters,
#   each value being weighted by its a priori probability, reflecting the beliefs about the possible values
#   of :math:`\underline{\theta}` before consideration of the experimental data.
#   Again, this is implemented as a probability distribution
# - The Metropolis-Hastings algorithm that samples from the posterior distribution of the calibration parameters
#   requires a vector :math:`\underline{\theta}_0` initial values for the calibration parameters,
#   as well as the proposal laws used to update each parameter sequentially.
#

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Dimension of the vector of parameters to calibrate
paramDim = 3
# The number of obesrvations
obsSize = 10

# %%
# - Define the observed inputs :math:`x_i`

# %%
xmin = -2.
xmax = 3.
step = (xmax-xmin)/(obsSize-1)
rg = ot.RegularGrid(xmin, step, obsSize)
x_obs = rg.getVertices()
x_obs

# %%
# - Define the parametric model :math:`z = f(x,\theta)` that associates each observation :math:`x_i` and values of the  parameters :math:`\theta_i` to the parameters of the distribution of the corresponding observation: here :math:`z=(\mu, \sigma)` where :math:`\mu`, the first output of the model, is the mean and :math:`\sigma`, the second output of the model, is the standard deviation.

# %%
fullModel = ot.SymbolicFunction(
    ['x1', 'theta1', 'theta2', 'theta3'], ['theta1+theta2*x1+theta3*x1^2','1.0'])
model = ot.ParametricFunction(fullModel, [0], x_obs[0])
model

# %%
# - Define the observation noise :math:`\varepsilon {\sim} \mathcal N(0, 1)` and create a sample from it.

# %%
ot.RandomGenerator.SetSeed(0)
noiseStandardDeviation = 1.
noise = ot.Normal(0,noiseStandardDeviation)
noiseSample = noise.getSample(obsSize)
noiseSample

# %%
# - Define the vector of observations :math:`y_i`

# %%
# In this model, we use a constant value of the parameter. The "true" value of :math:`\theta` is used to compute the model outputs. 

# %%
thetaTrue = [-4.5,4.8,2.2]

# %%
y_obs = ot.Sample(obsSize,1)
for i in range(obsSize):
    model.setParameter(x_obs[i])
    y_obs[i,0] = model(thetaTrue)[0] + noiseSample[i,0]
y_obs

# %%
# - Draw the model vs the observations.

# %%
functionnalModel = ot.ParametricFunction(fullModel, [1,2,3], thetaTrue)
graphModel = functionnalModel.getMarginal(0).draw(xmin,xmax)
observations = ot.Cloud(x_obs,y_obs)
observations = ot.Cloud(x_obs,y_obs)
observations.setColor("red")
graphModel.add(observations)
graphModel.setLegends(["Model","Observations"])
graphModel.setLegendPosition("topleft")
view = viewer.View(graphModel)

# %%
# - Define the distribution of observations :math:`\underline{y} | \underline{z}` conditional on model predictions
#
# Note that its parameter dimension is the one of :math:`\underline{z}`, so the model must be adjusted accordingly

# %%
conditional = ot.Normal()
conditional

# %%
# - Define the mean :math:`\mu_\theta`, the covariance matrix :math:`\Sigma_\theta`, then the prior distribution :math:`\pi(\underline{\theta})` of the parameter :math:`\underline{\theta}`. 

# %%
thetaPriorMean = [-3.,4.,1.]

# %%
sigma0 = ot.Point([2.,1.,1.5])  # standard deviations
thetaPriorCovarianceMatrix = ot.CovarianceMatrix(paramDim)
for i in range(paramDim):
    thetaPriorCovarianceMatrix[i, i] = sigma0[i]**2

prior = ot.Normal(thetaPriorMean, thetaPriorCovarianceMatrix)
prior.setDescription(['theta1', 'theta2', 'theta3'])
prior

# %%
# - Proposal distribution: uniform.

# %%
proposal = [ot.Uniform(-1., 1.)] * paramDim
proposal

# %%
# Test the MCMC sampler
# ---------------------
#
# The MCMC sampler essentially computes the log-likelihood of the parameters.

# %%
mymcmc = ot.MCMC(prior, conditional, model, x_obs, y_obs, thetaPriorMean)

# %%
mymcmc.computeLogLikelihood(thetaPriorMean)

# %%
# Test the Metropolis-Hastings sampler
# ------------------------------------

# %%
# - Creation of the Random Walk Metropolis-Hastings (RWMH) sampler.

# %%
initialState = thetaPriorMean

# %%
RWMHsampler = ot.RandomWalkMetropolisHastings(
    prior, conditional, model, x_obs, y_obs, initialState, proposal)

# %%
# In order to check our model before simulating it, we compute the log-likelihood.

# %%
RWMHsampler.computeLogLikelihood(initialState)

# %%
# We observe that, as expected, the previous value is equal to the output of the same method in the MCMC object.

# %%
# Tuning of the RWMH algorithm.

# %%
# Strategy of calibration for the random walk (trivial example: default).

# %%
strategy = ot.CalibrationStrategyCollection(paramDim)
RWMHsampler.setCalibrationStrategyPerComponent(strategy)

# %%
# Other parameters.

# %%
RWMHsampler.setVerbose(True)
RWMHsampler.setThinning(1)
RWMHsampler.setBurnIn(2000)

# %%
# Generate a sample from the posterior distribution of the parameters theta.

# %%
sampleSize = 10000
sample = RWMHsampler.getSample(sampleSize)

# %%
# Look at the acceptance rate (basic checking of the efficiency of the tuning; value close to 0.2 usually recommended).

# %%
RWMHsampler.getAcceptanceRate()

# %%
# Build the distribution of the posterior by kernel smoothing.

# %%
kernel = ot.KernelSmoothing()
posterior = kernel.build(sample)

# %%
# Display prior vs posterior for each parameter.

# %%
from openturns.viewer import View
import pylab as pl

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
print(viewer._noshow)
if not viewer._noshow:
    plt.show()
