"""
Create a functional basis process
=================================
"""

# %%
# The objective of this example is to define
# :math:`X: \Omega \times \mathcal{D} \rightarrow \mathbb{R}^d` a multivariate stochastic
# process of dimension :math:`d` where :math:`\mathcal{D} \in \mathbb{R}^n`, as a linear
# combination of :math:`K` deterministic functions
# :math:`(\phi_i)_{i=1,\dots,K}: \mathbb{R}^n \rightarrow \mathbb{R}^d`:
#
# .. math::
#     \begin{aligned}
#       X(\omega,\vect{t})=\sum_{i=1}^KA_i(\omega)\phi_i(\vect{t})
#     \end{aligned}
#
# where :math:`\vect{A}=(A_1,\dots, A_K)` is a random vector of dimension :math:`K`.
#
# We suppose that :math:`\mathcal{M}` is discretized on the mesh :math:`\mathcal{M}` which has :math:`N`
# vertices.
#
# A realization of :math:`X` on :math:`\mathcal{M}` consists in generating a realization
# :math:`\vect{\alpha}` of the random vector :math:`\vect{A}` and in evaluating the
# functions :math:`(\phi_i)_{i=1,\dots,K}` on the mesh :math:`\mathcal{M}`.
#
# If we note
# :math:`(\vect{x}_0, \dots, \vect{x}_{N-1})` the realization of :math:`X`, where
# :math:`X(\omega, \vect{t}_k) = \vect{x}_k`, we have:
#
# .. math::
#    \begin{aligned}
#      \forall k \in [0, N-1], \quad \vect{x}_k = \sum_{i=1}^K\alpha_i\phi_i(\vect{t}_k)
#    \end{aligned}
#

# %%
import openturns as ot
import openturns.viewer as otv


# %%
# Define the coefficients distribution
mu = [2.0] * 2
sigma = [5.0] * 2
R = ot.CorrelationMatrix(2)
coefDist = ot.Normal(mu, sigma, R)

# %%
# Create a basis of functions
phi_1 = ot.SymbolicFunction(["t"], ["sin(t)"])
phi_2 = ot.SymbolicFunction(["t"], ["cos(t)^2"])
myBasis = ot.Basis([phi_1, phi_2])

# %%
# Create the mesh
myMesh = ot.RegularGrid(0.0, 0.1, 100)

# %%
# Create the process
process = ot.FunctionalBasisProcess(coefDist, myBasis, myMesh)

# %%
# Draw a sample
N = 6
sample = process.getSample(N)
graph = sample.drawMarginal(0)
graph.setTitle(str(N) + " realizations of functional basis process")
view = otv.View(graph)

# %%
# Display all figures
otv.View.ShowAll()
