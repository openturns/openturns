"""
Create a process from random vectors and processes
==================================================
"""
# %% 
#
# The objective is to create a process defined from a random vector and a process.
#
# We consider the following limit state function, defined as the difference between a degrading resistance :math:`r(t) = R - bt`  and a time-varying load :math:`S(t)`:
#
# .. math::
#    \begin{align*}
#    g(t)= r(t) - S(t) = R - bt - S(t)
#    \end{align*}
#
# We propose the following probabilistic model: 
# - :math:`R` is the initial resistance, and :math:`R \sim \mathcal{N}(\mu_R, \sigma_R)`;   
# - :math:`b` is the deterioration rate of the resistance; it is deterministic; 
# - :math:`S(t)` is the time-varying stress, which is modeled by a stationary Gaussian process of mean value :math:`\mu_S`, standard deviation :math:`\sigma_S` and a squared exponential covariance model;
# - :math:`t` is the time, varying in :math:`[0,T]`.
#

# %%
# First, import the python modules: 

# %%
from  openturns import *
from openturns.viewer import View
from math import *

# %%
# 1. Create the gaussian process :math:`(\omega, t) \rightarrow S(\omega,t)`
# --------------------------------------------------------------------------

# %%
# Create the mesh which is a regular grid on :math:`[0,T]`, with :math:`T=50`, by step =1:

# %%
b = 0.01
t0 = 0.0
step = 1
tfin = 50
n = round((tfin-t0)/step)
myMesh = RegularGrid(t0, step, n)

# %%
# Create the squared exeponential covariance model: 
# 
# .. math::
#    C(s,t) = \sigma^2e^{-\frac{1}{2} \left( \dfrac{s-t}{l} \right)^2}
# 
# where the scale parameter is :math:`l=\frac{10}{\sqrt{2}}` and the amplitude :math:`\sigma = 1`.
#

# %%
l = 10/sqrt(2)
myCovKernel = SquaredExponential([l])
print('cov model = ', myCovKernel)

# %%
# Create the gaussian process :math:`S(t)`:

# %%
S_proc = GaussianProcess(myCovKernel, myMesh)


# %%
# 2. Create the process :math:`(\omega, t) \rightarrow R(\omega)-bt`
# ------------------------------------------------------------------

# %%
# First, create the random variable :math:`R \sim \mathcal{N}(\mu_R, \sigma_R)`, with :math:`\mu_R = 5` and :math:`\sigma_R = 0.3`:

# %%
muR = 5
sigR = 0.3
R = Normal(muR, sigR)

# %%
# The create the Dirac random variable :math:`B = b`:

# %%
B = Dirac(b)

# %%
# Then create the process :math:`(\omega, t) \rightarrow R(\omega)-bt` using the :math:`FunctionalBasisProcess` class and the functional basis :math:`\phi_1 : t \rightarrow 1` and :math:`\phi_2: -t \rightarrow t` : 
# 
# .. math::
#    R(\omega)-bt = R(\omega)\phi_1(t) + B(\omega) \phi_2(t)
# 
# with :math:`(R,B)` independent.

# %%
const_func = SymbolicFunction(['t'], ['1'])
linear_func = SymbolicFunction(['t'], ['-t'])
myBasis = Basis([const_func, linear_func])

coef = ComposedDistribution([R, B])

R_proc = FunctionalBasisProcess(coef, myBasis, myMesh)

# %%
# 3. Create the process :math:`Z: (\omega, t) \rightarrow R(\omega)-bt + S(\omega, t)`
# ------------------------------------------------------------------------------------

# %%
# First, aggregate both processes into one process of dimension 2: :math:`(R_{proc}, S_{proc})`

# %%
myRS_proc = AggregatedProcess([R_proc, S_proc])

# %%
# Then create the spatial field function that acts only on the values of the process, keeping the mesh unchanged, using the *ValueFunction* class. 
# We define the function :math:`g` on :math:`\mathbb{R}^2` by:
# 
# .. math::
#    g(x,y) = x-y
# 
# in order to define the spatial field function :math:`g_{dyn}` that acts on fields, defined by: 
# 
# .. math::
#    \forall t\in [0,T], g_{dyn}(X(\omega, t), Y(\omega, t)) = X(\omega, t) - Y(\omega, t)
# 

# %%
g = SymbolicFunction(['x1', 'x2'], ['x1-x2'])
gDyn = ValueFunction(g, myMesh)

# %%
# Now you have to create the final process :math:`Z` thanks to :math:`g_{dyn}`:

# %%
Z_proc = CompositeProcess(gDyn, myRS_proc)

# %%
# 4. Draw some realizations of the process
# ----------------------------------------

# %%
N=10
sampleZ_proc = Z_proc.getSample(N)
graph = sampleZ_proc.drawMarginal(0)
graph.setTitle(r'Some realizations of $Z(\omega, t)$')
view = View(graph)

# %%
# 5. Evaluate the probability that :math:`Z(\omega, t) \in \mathcal{D}`
# ---------------------------------------------------------------------

# %%
# We define the domaine :math:`\mathcal{D} = [2,4]` and the event :math:`Z(\omega, t) \in \mathcal{D}`:

# %%
domain = Interval([2], [4])
print('D = ', domain)
event = ProcessEvent(Z_proc, domain)

# %%
# We use the Monte Carlo sampling to evaluate the probability:

# %%
MC_algo = ProbabilitySimulationAlgorithm(event)
MC_algo.setMaximumOuterSampling(1000000)
MC_algo.setBlockSize(100)
MC_algo.setMaximumCoefficientOfVariation(0.01)
MC_algo.run()

result = MC_algo.getResult()

proba = result.getProbabilityEstimate()
print('Probability = ', proba)
variance =  result.getVarianceEstimate()
print('Variance Estimate = ', variance)
IC90_low = proba- result.getConfidenceLength(0.90)/2
IC90_upp = proba + result.getConfidenceLength(0.90)/2
print('IC (90%) = [', IC90_low, ', ', IC90_upp, ']')
view.ShowAll()
