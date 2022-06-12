"""
Estimate a process-based event probability
==========================================
"""
# %%
# The objective of this example is to evaluate the probability of an
# event based on a stochastic process, using the Monte Carlo estimator.
#
# Let :math:`X: \Omega \times \mathcal{D} \rightarrow \mathbb{R}^d` be a stochastic process
# of dimension :math:`d`, where :math:`\mathcal{D} \in \mathbb{R}^n` is discretized on the mesh
# :math:`\mathcal{M}`.
#
# We define the event :math:`\mathcal{E}` as:
#
# .. math::
#    \begin{aligned}
#      \displaystyle \mathcal{E}(X) = \bigcup_{\underline{t}\in \mathcal{M}}\left\{X_{\underline{t}}  \in \mathcal{A}  \right\}
#    \end{aligned}
#
# where :math:`\mathcal{A}` is a domain of :math:`\mathbb{R}^d`.
#
# We estimate the probabilty :math:`p=\mathbb{P}\left(\mathcal{E}(X)\right)` with the Monte Carlo
# estimator.
#
# The Monte Carlo algorithm is manipulated the same way as in the case
# where the event is based on a random variable independent of time.
#
# We illustrate the algorithm on the example of the bidimensionnal white
# noise process :math:`\varepsilon: \Omega \times \mathcal{D} \rightarrow \mathbb{R}^2` where
# :math:`\mathcal{D}\in \mathbb{R}`, distributed according to the bidimensionnal standard
# normal distribution (with zero mean, unit variance and independent
# marginals).
#
# We consider the domain :math:`\mathcal{A} =  [1,2] \times [1,2]`. Then the event :math:`\mathcal{E}` writes:
#
# .. math::
#    \begin{aligned}
#      \displaystyle \mathcal{E}(\varepsilon) = \bigcup_{\underline{t}\in \mathcal{M}}\left\{\varepsilon_{t}  \in \mathcal{A}  \right\}
#    \end{aligned}
#
# For all time stamps :math:`t \in \mathcal{M}`, the probability :math:`p_1` that the process
# enters into the domain :math:`\mathcal{A}` at time :math:`t` writes, using the independence
# property of the marginals:
#
# .. math::
#    \begin{aligned}
#      p_1 = \mathbb{P}\left(\varepsilon_t  \in \mathcal{A}\right) = (\Phi(2) - \Phi(1))^2
#    \end{aligned}
#
# with :math:`\Phi` the cumulative distribution function of the scalar standard *Normal* distribution.
#
# As the proces is discretized on a time grid of size :math:`N` and using the
# independance property of the white noise between two different time
# stamps and the fact that the white noise follows the same distribution
# at each time :math:`t`, the final probability :math:`p` writes:
#
# .. math::
#    p = \mathbb{P}\left(\mathcal{E}(\varepsilon)\right) = 1 - (1 - p_1)^{N}
#
# With :math:`K=10^4` realizations, using the Monte Carlo estimator, we obtain :math:`p_K = 0.1627`,
# to be compared to the exact value :math:`p=0.17008` for a time grid of size :math:`N=10`.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Create a time grid
tMin = 0.0
timeStep = 0.1
n = 100
tgrid = ot.RegularGrid(tMin, timeStep, n)

# %%
# Create a normal process
amplitude = [5.0]
scale = [3.0]
model = ot.ExponentialModel(scale, amplitude)
process = ot.GaussianProcess(model, tgrid)

# %%
# Create the 1-d domain A: [2.,5.]
lowerBound = [2.0]
upperBound = [5.0]
domain = ot.Interval(lowerBound, upperBound)

# %%
# Create an event from a Process and a Domain
event = ot.ProcessEvent(process, domain)

# %%
# Create the Monte-Carlo algorithm
montecarlo = ot.ProbabilitySimulationAlgorithm(event)

# Define the maximum number of simulations
montecarlo.setMaximumOuterSampling(1000)

# Define the block size
montecarlo.setBlockSize(100)

# Define the maximum coefficient of variation
montecarlo.setMaximumCoefficientOfVariation(0.0025)

# Run the algorithm
montecarlo.run()

# Get the result
montecarlo.getResult()

# %%
graph = montecarlo.drawProbabilityConvergence(0.95)
view = viewer.View(graph)
plt.show()
