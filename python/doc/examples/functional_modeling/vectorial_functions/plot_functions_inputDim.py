"""
==========================================
Increase the input dimension of a function
==========================================
"""
# %%
# Description
# ===========
#
# We want to build a function :math:`f : \mathbb{R}^d \mapsto \mathbb{R}` from
# *p* functions :math:`f_i: \mathbb{R} \mapsto \mathbb{R}`.
#
# We can do that:
#
# - Case 1: using the tensor product of the functions :math:`f_i`,
# - Case 2: by addition of the functions :math:`f_i`.
#
# We need to implement both basic steps:
#
# - Step 1: creation of the projection function: :math:`\pi_i : (x_1, \dots, x_d) \mapsto x_i`,
# - Step 2: creation of the composed function: :math:`g_i = f_i \circ \pi_i : (x_1, \dots, x_d) \mapsto f_i(x_i)`.


# %%
# Step 1: Creation of the  projection function
# ============================================
#
# The projection function is defined by:
#
# .. math::
#
#      \pi_i : (x_1, \dots, x_d) \mapsto x_i
#
# We can do that using:
#
# - a :class:`~openturns.SymbolicFunction` class,
# - a :class:`~openturns.LinearFunction` class.

# %%
# **Method 1**: We use the :class:`~openturns.SymbolicFunction` class.
import openturns as ot


def buidProjSymbolic(p, i):
    # R^p --> R
    # (x1, ..., xp) --> xi
    inputVar = ot.Description.BuildDefault(p, "x")
    return ot.SymbolicFunction(inputVar, [inputVar[i]])


d = 2
all_projections = [buidProjSymbolic(d, i) for i in range(d)]
print(
    "Input dimension = ",
    all_projections[0].getInputDimension(),
    "Output dimension = ",
    all_projections[0].getOutputDimension(),
)

# %%
# **Method 2**: We use the :class:`~openturns.LinearFunction` class.
#
# The function :math:`\pi_i(\vect{x}) = \mat{A}\left(\vect{x}-\vect{c}\right) + \vect{b}`.


def buildProjLinear(d, i):
    # R^d --> R
    # (x1, ..., xd) --> xi
    matA = ot.Matrix(1, d)
    matA[0, i] = 1.0
    cVect = [0.0] * d
    bVect = [0.0]
    return ot.LinearFunction(cVect, bVect, matA)


all_projections = [buildProjLinear(d, i) for i in range(d)]

# %%
# Step 2: Creation of the  composed function
# ==========================================
#
# The composed function is defined by: :math:`g_i = f_i \circ \pi_i` defined by:
#
# .. math::
#
#      g_i: (x_1, \dots, x_d) \mapsto f_i(x_i)
#
# We use the :class:`~openturns.ComposedFunction` class.

f1 = ot.SymbolicFunction(["x1"], ["x1^2"])
f2 = ot.SymbolicFunction(["x2"], ["3*x2"])
fi_list = [f1, f2]
all_g = [ot.ComposedFunction(f, proj) for (f, proj) in zip(fi_list, all_projections)]
print(all_g[0].getInputDimension(), all_g[0].getOutputDimension())

# %%
# Case 1: Tensor product
# ======================
#
# We want to build the function :math:`f : \mathbb{R}^d \mapsto \mathbb{R}` defined by:
#
# .. math::
#
#     f: (x_1, \dots, x_d) \mapsto \prod_{i=1}^d f_i(x_i)
#
# As the operator :math:`*` can only be applied to functions sharing the same input space, we need to
# use the projection function :math:`\pi_i` and the functions :math:`g_i` all defined on :math:`\mathbb{R}^d`.


def tensorProduct(factors):
    prod = factors[0]
    for i in range(1, len(factors)):
        prod = prod * factors[i]
    return prod


f = tensorProduct(all_g)
print("input dimension =", f.getInputDimension())
print("output dimension =", f.getOutputDimension())
print("f([1.0, 2.0]) = ", f([1.0, 2.0]))

# %%
# Case 2: Sum
# ===========
#
# We want to build the function :math:`f : \mathbb{R}^d \mapsto \mathbb{R}` defined by:
#
# .. math::
#
#     f: (x_1, \dots, x_d) \mapsto \sum_{i=1}^d f_i(x_i)
#
# We use the :class:`~openturns.LinearCombinationFunction` class.

coef = [1.0, 1.0]
f = ot.LinearCombinationFunction(all_g, [1.0] * len(all_g))
print("input dimension =", f.getInputDimension())
print("output dimension =", f.getOutputDimension())
