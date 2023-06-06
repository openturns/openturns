"""
============================================================================
Create a multivariate basis of functions from scalar multivariable functions
============================================================================
"""
# %%
# Description
# ===========
#
# We want to build a finite basis of functions :math:`\mathcal{B}=\{f_1,...,f_n\}` where each
# :math:`f_i: \mathbb{R}^d \mapsto \mathbb{R}^q` with :math:`d>1, q>1`, from given scalar multivariable
# functions :math:`g_i: \mathbb{R}^d \mapsto \mathbb{R}`.
#
# In other words, we want to create a basis of functions having both a multivariate input *and* output from
# functions having a multivariate input and a scalar output.
#
# In this example, we do that by the aggregation of the given functions :math:`g_i`,
# using the :class:`~openturns.AggregatedFunction` class. This class allows one to create a multivariate output
# function from a list of functions having a univariate output dimension. Note that we can only aggregate functions
# that share the same input space.
#
# We illustrate two cases that differ from the way the functions :math:`g_i` have been created:
#
# - Case 1: we suppose that the functions :math:`g_i` have no particular origin,
# - Case 2: we suppose that the functions :math:`g_i` have been built from particular bases.
#
# In both cases, we use the :class:`~openturns.Basis` class to create the basis and the
# :class:`~openturns.AggregatedFunction` class to create a multivariate output function from a
# list of scalar multivariable functions.
#
# Case 1: Aggregation of given functions
# ======================================
# 
# In that case, we have some functions :math:`g_i: \mathbb{R}^d \mapsto \mathbb{R}` and we aggregate them
# in order to build the basis :math:`\mathcal{B}`.
#
# For example, we set :math:`d=2`, :math:`q=3` and we use the functions :math:`(g_1, g_2, g_3)` defined by:
#
# .. math::
#     \begin{array}{lcl}
#       g_1(x_1, x_2) & = & 1\\
#       g_2(x_1, x_2) & = & x_1 + x_2\\
#       g_3(x_1, x_2) & = & x_1^2 + x_2^2 
#     \end{array}
#
# and we want to build the finite basis :math:`\mathcal{B}=\{f_1,f_2\}` defined by:
#
# .. math::
#     \begin{array}{lcl}
#       f_1(x_1, x_2) & = & (1, 1, 1)\\
#       f_2(x_1, x_2) & = & (1, x_1 + x_2, x_1^2 + x_2^2)
#     \end{array}
import openturns as ot
d = 2
q = 3

 # Create the functions g_i
g_1 = ot.SymbolicFunction(['x1', 'x2'], ['1'])
g_2 = ot.SymbolicFunction(['x1', 'x2'], ['x1+x2'])
g_3 = ot.SymbolicFunction(['x1', 'x2'], ['x1^2+x2^2'])

# Create the multivariate basis
f_1 = ot.AggregatedFunction([g_1] * q)
f_2 = ot.AggregatedFunction([g_1, g_2, g_3])
basis = ot.Basis([f_1, f_2])
print('Fonction f_1 : ', basis.build(0))

# %%
# Case 2: Aggregation of functions built from given bases
# =======================================================
#
# In that case, we use some functions :math:`g_i: \mathbb{R}^d \mapsto \mathbb{R}` that have been
# built from particular bases. For example, they are members of polynomial families which are orthonormal
# to given distributions.
#
# In this example, we use both polynomial families:
#
# - :math:`(g_1^i)_{i\geq 0}` which is a orthonormal multivariate polynomial basis with respect to the uniform
#   distributipn on :math:`[-1,1]^d`(it is obtained through a tensor product of univariate Legendre polynomials
#   orthonormal with respect to the uniform distribution on :math:`[-1,1]`),
# - :math:`(g_2^i)_{i\geq 0}` which is a orthonormal multivariate polynomial basis with respect to the multivariate
#   standard normal distribution (it is obtained through a tensor product of univariate Hermite polynomials
#   orthonormal with respect to the standard normal univariate distribution).
#
# We consider the linear
# enumeration of the polynomial family. The functions :math:`f_i` are built as the aggregation of
# the :math:`i`-th polynomials of the families :math:`(g_1^k)_k` and :math:`(g_2^k)_k`.
# We only consider :math:`g_1^i` and :math:`g_2^i` of total degree less than 2:
# then we have 6 polynomials :math:`f_i`. 
#
# We still set :math:`d=2`, :math:`q=3`.
d = 2

legendre_builder = ot.OrthogonalProductPolynomialFactory([ot.LegendreFactory()] * d)
hermite_builder = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * d)

# Generate the multivariate functions f_i (quadratic)
total_deg = 2
basis_size = ot.LinearEnumerateFunction(d).getStrataCumulatedCardinal(total_deg)
basis = []
for i in range(basis_size):
    f_i = ot.AggregatedFunction([legendre_builder.build(i), hermite_builder.build(i)])
    basis.append(f_i)

basis = ot.Basis(basis)
