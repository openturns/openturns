"""
========================================================================
Orthonormalize a finite set of functions on a multivariate domain
========================================================================
"""

# %%
# Description
# ===========
#
# We consider a finite set of functions :math:`(g_1, \dots, g_n)` defined on
# :math:`\mathbb{R}^d` and a probability distribution :math:`\mu`. These
# functions are usually not orthonormal with respect to :math:`\mu`.
#
# In this example, we build an orthonormal basis :math:`(\varphi_1, \dots, \varphi_n)`
# of the vector space spanned by the functions :math:`g_i`, i.e. such that:
#
# .. math::
#
#     \langle \varphi_i, \varphi_j \rangle = \int \varphi_i(x) \varphi_j(x) \, \mu(\mathrm{d}x)
#     = \delta_{ij}.
#
# The :class:`~openturns.experimental.BasisOrthonormalizationFactory` class
# performs this orthonormalization. Internally, it relies on the
# :class:`~openturns.experimental.BasisOrthonormalizationAlgorithm` class for
# the computation of the basis and on the
# :class:`~openturns.experimental.GaussLPQuadrature` class for the construction
# of minimal quadrature rules.
#
# We consider the uniform distribution on :math:`[0, 1]^2` and the six
# monomials of total degree lower than or equal to 2.

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

# %%
# Define the measure and the initial functions
# --------------------------------------------
dimension = 2
measure = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dimension)

initialFunctions = [
    ot.SymbolicFunction(["x1", "x2"], ["1"]),
    ot.SymbolicFunction(["x1", "x2"], ["x1"]),
    ot.SymbolicFunction(["x1", "x2"], ["x2"]),
    ot.SymbolicFunction(["x1", "x2"], ["x1^2"]),
    ot.SymbolicFunction(["x1", "x2"], ["x1 * x2"]),
    ot.SymbolicFunction(["x1", "x2"], ["x2^2"]),
]

# %%
# Build the orthonormal basis
# ---------------------------
#
# The factory is built from the initial functions and the measure. Then each
# orthonormal function is built from its index.
factory = otexp.BasisOrthonormalizationFactory(initialFunctions, measure)
basisSize = len(initialFunctions)
orthonormalFunctions = [factory.build(i) for i in range(basisSize)]

for i in range(3):
    print(f"phi_{i} = {orthonormalFunctions[i]}")

# %%
# Check orthonormality
# --------------------
#
# We integrate the products of the orthonormal functions against the measure
# with a Gauss-Legendre rule. The resulting Gram matrix is the identity matrix
# up to numerical precision.
degree = 10
integration = ot.GaussLegendre([degree] * dimension)
gram = ot.SymmetricMatrix(basisSize)
for i in range(basisSize):
    for j in range(i + 1):
        def integrand(x, i=i, j=j):
            return [orthonormalFunctions[i](x)[0] * orthonormalFunctions[j](x)[0] * measure.computePDF(x)]

        function = ot.PythonFunction(dimension, 1, integrand)
        gram[i, j] = integration.integrate(function, measure.getRange())[0]
print("Gram matrix:")
print(gram)

# %%
# Build a minimal quadrature rule
# -------------------------------
#
# The :meth:`~openturns.experimental.BasisOrthonormalizationFactory.buildQuadrature`
# method builds a sparse quadrature rule which integrates exactly the first
# functions of the basis. The weights sum to one because the first orthonormal
# function is the constant function equal to one.
n = 4
nodes, weights = factory.buildQuadrature(n)
print(f"Number of nodes: {len(nodes)}")
print(f"Sum of weights: {sum(weights):.6f}")

for k in range(n):
    integral = sum(weights[i] * orthonormalFunctions[k](nodes[i])[0] for i in range(len(nodes)))
    print(f"Integral of phi_{k}: {integral:.3e}")

# %%
# The quadrature nodes are represented in the next figure. The rule is sparse:
# it contains fewer nodes than a tensorized Gauss product rule of the same
# exactness degree.
graph = ot.Graph("Minimal quadrature rule", "x1", "x2", True)
cloud = ot.Cloud(nodes)
cloud.setPointStyle("fcircle")
cloud.setColor("steelblue")
graph.add(cloud)
graph.setAxes(True)
view = otv.View(graph)

# %%
view.ShowAll()
