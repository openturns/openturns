"""
Bayesian calibration of the flooding model
==========================================
"""

# %%
#
# Abstract
# --------
#
# The goal of this example is to present the Bayesian calibration of the
# :ref:`flooding model<use-case-flood-model>`.
# We use the :class:`~openturns.RandomWalkMetropolisHastings` and
# :class:`~openturns.Gibbs` classes
# and simulate a sample of the posterior distribution using
# :ref:`metropolis_hastings`.

# %%
# Parameters to calibrate
# -----------------------
#
# The vector of parameters to calibrate is:
#
# .. math::
#    \vect{\theta} = (K_s,Z_v,Z_m).
#
#
# The reference values are:
#
# .. math::
#    K_s = 30, \qquad Z_v = 50, \qquad Z_m = 55.
#
#
# Observations
# ------------
#
# We consider the probabilistic model:
#
# .. math::
#    H =  \model(Q,K_s,Z_v,Z_m) + \epsilon
#
# where:
#
# .. math::
#    \epsilon \sim \mathcal{N}(0,\sigma^2)
#
# Thus, the error of the water height follows a normal distribution with a zero mean and a standard variation :math:`\sigma`.
#
# We have some observations :math:`\{(Q_i,h_i)\}_{i=1,...,\sampleSize}`. Each observation is a couple made of the flowrate
# and the corresponding river height. We assume that the observations are associated to independent errors.
# We consider a sample size equal to:
#
# .. math::
#    \sampleSize = 10.
#
#
# Variables
# ---------
#
# - :math:`Q` : Input. Observed.
# - :math:`K_s`, :math:`Z_v`, :math:`Z_m` : Input. Calibrated.
# - :math:`h`: Output. Observed.
#
# Analysis
# --------
#
# In the description of the :ref:`flooding model<use-case-flood-model>`, we see that only one parameter
# can be identified.
# Hence, calibrating this model requires some regularization.
# In this example, we use Bayesian methods as a way to regularize the model.

# %%
# Generate the observations
# -------------------------

# %%
from matplotlib import pyplot as plt
from openturns.usecases import flood_model
import openturns.viewer as viewer
import numpy as np
import openturns as ot

# %%
# A basic implementation of the probabilistic model is available in the usecases module :
fm = flood_model.FloodModel()

# %%
# We define the model :math:`\metamodel` which has 4 inputs and one output :math:`h`.
#
# The non linear least squares algorithm does not take into account bounds in the parameters.
# Therefore, we ensure that the output is computed whatever the inputs.
# The model fails into two situations:
#
# * if :math:`K_s<0`,
# * if :math:`Z_v-Z_m<0`.
#
# In these cases, we return an infinite number.

# %%


def functionFlooding(X):
    L = 5.0e3
    B = 300.0
    Q, K_s, Z_v, Z_m = X
    alpha = (Z_m - Z_v) / L
    if alpha < 0.0 or K_s <= 0.0:
        H = np.inf
    else:
        H = (Q / (K_s * B * np.sqrt(alpha))) ** (3.0 / 5.0)
    return [H]


# %%
g = ot.PythonFunction(4, 1, functionFlooding)
g = ot.MemoizeFunction(g)
g.setOutputDescription(["H (m)"])

# %%
# We load the input distribution for the flow :math:`Q`.
Q = fm.Q

# %%
# Set the parameters to be calibrated.

# %%
K_s = ot.Dirac(30.0)
Z_v = ot.Dirac(50.0)
Z_m = ot.Dirac(55.0)
K_s.setDescription(["Ks (m^(1/3)/s)"])
Z_v.setDescription(["Zv (m)"])
Z_m.setDescription(["Zm (m)"])

# %%
# We create the joint input distribution of :math:`(Q, K_s, Z_v, Z_m)`.

# %%
inputRandomVector = ot.JointDistribution([Q, K_s, Z_v, Z_m])

# %%
# We import some noisy observations of the flow rate (column 0) and the height (column 1) already
# stored of the  :ref:`flooding model<use-case-flood-model>` in the field *data*.
Q_H_obs = fm.data
Qobs = Q_H_obs.getMarginal(0)
Hobs = Q_H_obs.getMarginal(1)

# %%
graph = ot.Graph("Observations", "Q (m3/s)", "h (m)", True)
cloud = ot.Cloud(Q_H_obs)
# cloud = ot.Cloud(Qobs, Hobs)
graph.add(cloud)
view = viewer.View(graph)


# %%
# Setting the calibration parameters
# ----------------------------------

# %%
# We assume that the output of the model :math:`H` is random and follows a normal distribution with the standard
# deviation :math:`\sigma = 0.5` and centered on :math:`\mu = \model(Q, K_s, Z_v, Z_m)`.
#
# Then, we define the parametric model :math:`\vect z = f_Q(\vect\theta)` that associates each observation
# :math:`Q` and value of the parameters :math:`\vect \theta = (K_s, Z_v, Z_m)`
# to the parameters :math:`\vect z=(\mu, \sigma)`.
# We want to get the posterior distribution of the random vector :math:`\vect \Theta` that fits the best to the
# observations :math:`(Q_i, h_i)_i`.


# %%
def fullModelPy(X):
    Q, K_s, Z_v, Z_m = X
    mu = g(X)[0]
    sigma = 0.5  # (m^2) The standard deviation of the observation error.
    return [mu, sigma]


fullModel = ot.PythonFunction(4, 2, fullModelPy)
linkFunction = ot.ParametricFunction(fullModel, [0], [np.nan])
print(linkFunction)

# %%
# We define the prior distribution of  :math:`\vect\Theta`. We assume that it is a normal distribution.
# In the Bayesian framework, this is called the *prior* normal distribution. In the data assimilation framework, this
# is called the *background*.
# We assume that the prior distribution has independent components, that the mean marginal values are equal to the reference values
# and that the marginal standard deviations are known.

# %%
KsInitial = 20.0
ZvInitial = 49.0
ZmInitial = 51.0
parameterPriorMean = [KsInitial, ZvInitial, ZmInitial]
paramDim = len(parameterPriorMean)

# %%
sigmaKs = 5.0
sigmaZv = 1.0
sigmaZm = 1.0

# %%
parameterPriorCovariance = ot.CovarianceMatrix(paramDim)
parameterPriorCovariance[0, 0] = sigmaKs**2
parameterPriorCovariance[1, 1] = sigmaZv**2
parameterPriorCovariance[2, 2] = sigmaZm**2

# %%
# Define the prior distribution of :math:`\vect\Theta`.
prior = ot.Normal(parameterPriorMean, parameterPriorCovariance)
prior.setDescription(["Ks", "Zv", "Zm"])

# %%
# Define the distribution of the output :math:`H | \vect{z}` conditional on model predictions.
# Note that its parameter dimension is the one of :math:`\vect{z}`, so the model must be adjusted accordingly.
# In other words, the input argument of the `setParameter` method of the conditional distribution must be equal to the dimension of the output of the `model`.
# Hence, we do not have to set the actual parameters: only the type of distribution is used.

# %%
conditional = ot.Normal()

# %%
# The proposed steps for :math:`K_s` :math:`Z_v` and :math:`Z_m`
# will all follow uniform distributions,
# but with different supports.

# %%
proposal = [ot.Uniform(-5.0, 5.0), ot.Uniform(-1.0, 1.0), ot.Uniform(-1.0, 1.0)]

# %%
# Build a Gibbs sampler
# ---------------------
initialState = parameterPriorMean
mh_coll = [
    ot.RandomWalkMetropolisHastings(prior, initialState, proposal[i], [i])
    for i in range(paramDim)
]
for mh in mh_coll:
    mh.setLikelihood(conditional, Hobs, linkFunction, Qobs)
sampler = ot.Gibbs(mh_coll)

# %%
# Generate a sample from the posterior distribution of :math:`\vect \Theta`.
sampleSize = 1000
sample = sampler.getSample(sampleSize)

# %%
# Look at the acceptance rates of the random walk Metropolis-Hastings samplers.
[mh.getAcceptanceRate() for mh in sampler.getMetropolisHastingsCollection()]

# %%
# Build the posterior distribution of :math:`\vect \Theta` by kernel smoothing.
kernel = ot.KernelSmoothing()
posterior = kernel.build(sample)

# %%
# Display prior vs posterior for each parameter.


def plot_bayesian_prior_vs_posterior_pdf(prior, posterior):
    """
    Plot the prior and posterior distribution of a Bayesian calibration

    Parameters
    ----------
    prior : ot.Distribution(dimension)
        The prior.
    posterior : ot.Distribution(dimension)
        The posterior.

    Return
    ------
    grid : ot.GridLayout(1, dimension)
        The prior and posterior PDF for each marginal.
    """
    paramDim = prior.getDimension()
    grid = ot.GridLayout(1, paramDim)
    parameterNames = prior.getDescription()
    for parameter_index in range(paramDim):
        graph = ot.Graph("", parameterNames[parameter_index], "PDF", True)
        # Prior
        curve = prior.getMarginal(parameter_index).drawPDF().getDrawable(0)
        curve.setLineStyle(
            ot.ResourceMap.GetAsString("CalibrationResult-PriorLineStyle")
        )
        curve.setLegend("Prior")
        graph.add(curve)
        # Posterior
        curve = posterior.getMarginal(parameter_index).drawPDF().getDrawable(0)
        curve.setLineStyle(
            ot.ResourceMap.GetAsString("CalibrationResult-PosteriorLineStyle")
        )
        curve.setLegend("Posterior")
        graph.add(curve)
        #
        if parameter_index < paramDim - 1:
            graph.setLegends([""])
        if parameter_index > 0:
            graph.setYTitle("")
        graph.setLegendPosition("upper right")
        grid.setGraph(0, parameter_index, graph)
    grid.setTitle("Bayesian calibration")
    return grid


# %%
# sphinx_gallery_thumbnail_number = 2
grid = plot_bayesian_prior_vs_posterior_pdf(prior, posterior)
viewer.View(
    grid,
    figure_kw={"figsize": (8.0, 3.0)},
    legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"},
)
plt.subplots_adjust(right=0.8, bottom=0.2, wspace=0.3)

# %%
viewer.View.ShowAll()
