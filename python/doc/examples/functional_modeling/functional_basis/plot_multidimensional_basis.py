"""
============================================================================
Create a basis of functions with input and output dimension greater than one
============================================================================
"""
# %%
# Description
# ===========
#
# We want to build a basis of functions :math:`\mathcal{B}=\{f_1,...,f_n\}` such
# that :math:`f_n: \mathbb{R}^d \mapsto \mathbb{R}^q` with :math:`d>1, q>1`.
# In other words, we want to create a basis of functions having both a multivariate input *and* output.
# In this example, we present methods to use the :class:`~openturns.AggregatedFunction` class to
# create a multivariate output function from a list of functions having a univariate output dimension.
# Note that we can aggregate only functions that share the same input space.
#
# We can do that:
#
# - Case 1: by the aggregation of :math:`n` given functions :math:`f_i: \mathbb{R}^d \mapsto \mathbb{R}^q`,
# - Case 2: by the aggregation of :math:`n` functions :math:`f_i: \mathbb{R}^d \mapsto \mathbb{R}^q` built 
#   from factories of functions :math:`g_j:\mathbb{R}^d \mapsto \mathbb{R}`.
#
# In both cases, we use the :class:`~openturns.Basis` class to create the basis and the
# :class:`~openturns.AggregatedFunction` class to create a multivariate output function from a
# list of functions.
#
# Case 1: Aggregation of given functions
# ======================================
import openturns as ot
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
