"""
============================================================================
Create a basis of functions with input and output dimension greater than one
============================================================================
"""
# %%
# Import the required modules

import openturns as ot

# %%
# Description
# ===========
#
# We want to build a basis :math:`\mathcal{B}=\{f_1,...,f_n\}` such that :math:`f_n: \mathbb{R}^d \mapsto \mathbb{R}^q` with :math:`d>1, q>1`.
#
# We can do that:
#
# - Case 1: by the aggregation of :math:`n` given functions :math:`f_i: \mathbb{R}^d \mapsto \mathbb{R}^q`,
# - Case 2: by the aggregation of :math:`n` functions :math:`f_i: \mathbb{R}^d \mapsto \mathbb{R}^q` built from factories of functions :math:`g_j:\mathbb{R}^d \mapsto \mathbb{R}`.
#

# %%
# Case 1: Aggregation of given functions
# ======================================
#

d = 2
q = 3
linear_basis = ot.LinearBasisFactory(d).build()

# Get the constant function
constant_function = linear_basis.build(0)
# Get the affine function
affine_function = linear_basis.build(1)

# Extend the function to a multivariate basis
phi_0 = ot.AggregatedFunction([constant_function] * q)
phi_1 = ot.AggregatedFunction([affine_function] * q)
basis = ot.Basis([phi_0, phi_1])

# %%
# Case 2: Aggregation of functions built from given bases
# =======================================================
#

input_dim = 2

legendre_builder = ot.OrthogonalProductPolynomialFactory([ot.LegendreFactory()] * input_dim)
hermite_builder = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * input_dim)

# Generate the multivariate function (quadratic)
basis_size = ot.LinearEnumerateFunction(input_dim).getStrataCumulatedCardinal(2)
basis = []
for i in range(basis_size):
    phi_i = ot.AggregatedFunction([legendre_builder.build(i), hermite_builder.build(i)])
    basis.append(phi_i)

basis = ot.Basis(basis)
