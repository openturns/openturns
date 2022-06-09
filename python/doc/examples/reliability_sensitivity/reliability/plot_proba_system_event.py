"""
Time variant system reliability problem
=======================================
"""
# %%

# %%
# The objective is to evaluate the outcrossing rate from a safe to a failure domain in a time variant reliability problem.
#
# We consider the following limit state function, defined as the difference between a degrading resistance :math:`r(t) = R - bt`  and a time-varying load :math:`S(t)`:
#
# ..math:
#   \begin{align*}
#   g(t)= r(t) - S(t) = R - bt - S(t) \quad \forall t \in [0,T]
#   \end{align*}
#
# The failure domaine is defined by:
#
# .. math::
#    g(t) \leq 0
#
#
# which means that the resistance at :math:`t` is less thant the stress at :math:`t`.
#
#
# We propose the following probabilistic model:
#
# - :math:`R` is the initial resistance, and :math:`R \sim \mathcal{N}(\mu_R, \sigma_R)`;
# - :math:`b` is the deterioration rate of the resistance; it is deterministic;
# - :math:`S(\omega,t)` is the time-varying stress, which is modeled by a stationary Gaussian process of mean value :math:`\mu_S`, standard deviation :math:`\sigma_S` and a squared exponential covariance model :math:`C(s,t)`.
#
#
# The outcrossing rate from the safe to the failure domain at instant :math:`t` is defined by:
#
# .. math::
#    \nu^+(t) = \lim_{\Delta t \rightarrow 0+} \dfrac{\mathbb{P}\{ g(t) \ge 0 \cap g(t+\Delta t) \leq 0\} }{\Delta t}
#
#
# For each :math:`t`, we note the random variable  :math:`Z_t = R - bt - S_t` where :math:`S_t = S(., t)`.
#
# To evaluate :math:`\nu^+(t)`, we need to consider the bivariate random vector :math:`(Z_t, Z_{t+\Delta t})`.
#
# The event :math:`\{ g(t) \geq 0 \cap g(t+\Delta t) <0\}` writes as the intersection of both events :
#
# - :math:`\mathcal{E}_t^1 = \{   Z_t \geq 0\}` and
# - :math:`\mathcal{E}_t^2 = \{   Z_{t+\Delta t} \leq 0\}`.
#
# The objective is to evaluate:
#
# .. math::
#    \mathbb{P}\{\mathcal{E}_t^1 \cap \mathcal{E}_t^2\} \quad \forall t \in [0,T]
#

# %%
# 1. Define some useful functions
# -------------------------------

# %%
# We define the bivariate random vector :math:`Y_t = (bt + S_t, b(t+\Delta t) + S_{t+\Delta t})`.
# Here, :math:`Y_t` is a bivariate Normal random vector:
#
# - whith mean :math:`[bt, b(t+\delta t)]` and
# - whith covariance matrix :math:`\Sigma` defined by:
#
# ..math::
#   \begin{align*}
#   \Sigma = \left(
#   \begin{array}{cc}
#   C(t, t) & C(t, t+\Delta t) \\
#   C(t, t+\Delta t) & C(t+\Delta t, t+\Delta t)
#   \end{array}
#   \right)
#   \end{align*}
#
# This function buils :math:`Y_t =(Y_t^1, Y_t^2)`.

# %%
from math import sqrt
from openturns.viewer import View
import openturns as ot


def buildNormal(b, t, mu_S, covariance, delta_t=1e-5):
    sigma = ot.CovarianceMatrix(2)
    sigma[0, 0] = covariance(t, t)[0, 0]
    sigma[0, 1] = covariance(t, t+delta_t)[0, 0]
    sigma[1, 1] = covariance(t+delta_t, t+delta_t)[0, 0]
    return ot.Normal([b*t + mu_S, b*(t+delta_t) + mu_S], sigma)


# %%
# This function creates the trivariate random vector :math:`(R, Y_t^1, Y_t^2)` where :math:`R` is independent from :math:`(Y_t^1, Y_t^2)`. We need to create this random vector because both events  :math:`\mathcal{E}_t^1` and :math:`\mathcal{E}_t^2` must be defined from the same random vector!

# %%
def buildCrossing(b, t, mu_S, covariance, R, delta_t=1e-5):
    normal = buildNormal(b, t, mu_S, covariance, delta_t)
    return ot.BlockIndependentDistribution([R, normal])


# %%
# This function evaluates the probability using the Monte Carlo sampling. It defines the intersection event :math:`\mathcal{E}_t^1 \cap \mathcal{E}_t^2`.

# %%
def getXEvent(b, t, mu_S, covariance, R, delta_t):
    full = buildCrossing(b, t, mu_S, covariance, R, delta_t)
    X = ot.RandomVector(full)
    f1 = ot.SymbolicFunction(["R", "X1", "X2"], ["X1 - R"])
    e1 = ot.ThresholdEvent(ot.CompositeRandomVector(f1, X), ot.Less(), 0.0)
    f2 = ot.SymbolicFunction(["R", "X1", "X2"], ["X2 - R"])
    e2 = ot.ThresholdEvent(ot.CompositeRandomVector(f2, X), ot.GreaterOrEqual(), 0.0)
    event = ot.IntersectionEvent([e1, e2])
    return X, event


# %%
def computeCrossingProbability_MonteCarlo(b, t, mu_S, covariance, R, delta_t, n_block, n_iter, CoV):
    X, event = getXEvent(b, t, mu_S, covariance, R, delta_t)
    algo = ot.ProbabilitySimulationAlgorithm(event, ot.MonteCarloExperiment())
    algo.setBlockSize(n_block)
    algo.setMaximumOuterSampling(n_iter)
    algo.setMaximumCoefficientOfVariation(CoV)
    algo.run()
    return algo.getResult().getProbabilityEstimate() / delta_t


# %%
# This function evaluates the probability using the Low Discrepancy sampling.

# %%
def computeCrossingProbability_QMC(b, t, mu_S, covariance, R, delta_t, n_block, n_iter, CoV):
    X, event = getXEvent(b, t, mu_S, covariance, R, delta_t)
    algo = ot.ProbabilitySimulationAlgorithm(event, ot.LowDiscrepancyExperiment(
        ot.SobolSequence(X.getDimension()), n_block, False))
    algo.setBlockSize(n_block)
    algo.setMaximumOuterSampling(n_iter)
    algo.setMaximumCoefficientOfVariation(CoV)
    algo.run()
    return algo.getResult().getProbabilityEstimate() / delta_t


# %%
# This function evaluates the probability using the FORM algorithm for event systems..

# %%
def computeCrossingProbability_FORM(b, t, mu_S, covariance, R, delta_t):
    X, event = getXEvent(b, t, mu_S, covariance, R, delta_t)
    algo = ot.SystemFORM(ot.SQP(), event, X.getMean())
    algo.run()
    return algo.getResult().getEventProbability() / delta_t


# %%
# 2. Evaluate the probability
# ---------------------------

# %%

# %%
# First, fix some parameters: :math:`(\mu_R, \sigma_R, \mu_S, \sigma_S, \Delta t, T, b)` and the covariance model which is the Squared Exponential model.
# Be careful to the parameter  :math:`\Delta t` which is of great importance: if it is too small, the simulation methods have problems to converge because the correlation rate is too high between the instants :math:`t` and :math:`t+\Delta t`.
# We advice to take :math:`\Delta t \simeq 10^{-1}`.
#

# %%
mu_S = 3.0
sigma_S = 0.5
l = 10

b = 0.01

mu_R = 5.0
sigma_R = 0.3
R = ot.Normal(mu_R, sigma_R)

covariance = ot.SquaredExponential([l/sqrt(2)], [sigma_S])

t0 = 0.0
t1 = 50.0
N = 26

# Get all the time steps t
times = ot.RegularGrid(t0, (t1 - t0) / (N - 1.0), N).getVertices()

delta_t = 1e-1

# %%
# Use all the methods previously described:
#
# - Monte Carlo: values in values_MC
# - Low discrepancy suites: values in values_QMC
# - FORM: values in values_FORM
#

# %%
values_MC = list()
values_QMC = list()
values_FORM = list()

for tick in times:
    values_MC.append(computeCrossingProbability_MonteCarlo(
        b, tick[0], mu_S, covariance, R, delta_t, 2**12, 2**3, 1e-2))
    values_QMC.append(computeCrossingProbability_QMC(
        b, tick[0], mu_S, covariance, R, delta_t, 2**12, 2**3, 1e-2))
    values_FORM.append(computeCrossingProbability_FORM(
        b, tick[0], mu_S, covariance, R, delta_t))

# %%
print('Values MC = ', values_MC)
print('Values QMC = ', values_QMC)
print('Values FORM = ', values_FORM)

# %%
# Draw the graphs!

# %%
g = ot.Graph()
g.setAxes(True)
g.setGrid(True)
c = ot.Curve(times, [[p] for p in values_MC])
g.add(c)
c = ot.Curve(times, [[p] for p in values_QMC])
g.add(c)
c = ot.Curve(times, [[p] for p in values_FORM])
g.add(c)
g.setLegends(["MC", "QMC", "FORM"])
g.setColors(["red", "blue", 'black'])
g.setLegendPosition("topleft")
g.setXTitle("t")
g.setYTitle("Outcrossing rate")
view = View(g)
view.ShowAll()
