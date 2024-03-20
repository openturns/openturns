"""
===========================================
Increase the output dimension of a function
===========================================
"""
# %%
# Description
# ===========
#
# We want to build a function :math:`f : \mathbb{R}^d \mapsto \mathbb{R}^q` from *q* functions :math:`f_i: \mathbb{R}^d \mapsto \mathbb{R}`.
#
# We can do that:
#
# - Case 1: by aggregation  of the functions :math:`f_i`,
# - Case 2: by creating a vectorial linear combination  of the functions :math:`f_i`.
#
# Case 1: Aggregation
# ===================
#
# We have :math:`q` functions :math:`f_i : \mathbb{R}^d \mapsto \mathbb{R}` for :math:`1 \leq i \leq q`.
# We create the function  :math:`f : \mathbb{R}^d \mapsto \mathbb{R}^q` defined by:
#
# .. math::
#
#     f(\vect{x}) =
#       \begin{pmatrix}
#         f_1(\vect{x}) \\
#         \vdots \\
#         f_q(\vect{x})
#       \end{pmatrix}
#
#
# We use the :class:`~openturns.AggregatedFunction` class.
#
# In the example, we take :math:`d=2` and :math:`q=3`.
import openturns as ot

f1 = ot.SymbolicFunction(["x1", "x2"], ["x1^2+x2"])
f2 = ot.SymbolicFunction(["x1", "x2"], ["x1+x2^2"])
f3 = ot.SymbolicFunction(["x1", "x2"], ["x1+x2"])
func_coll = [f1, f2, f3]
f = ot.AggregatedFunction(func_coll)
print("input dimension =", f.getInputDimension())
print("output dimension =", f.getOutputDimension())
print("f = ", f)

# %%
# Case 2: Vectorial linear combination
# ====================================
#
# We have :math:`q` functions :math:`f_i : \mathbb{R}^d \mapsto \mathbb{R}` for :math:`1 \leq i \leq q`.
# We create the function  :math:`f : \mathbb{R}^d \mapsto \mathbb{R}^q` defined by:
#
# .. math::
#
#     f(\vect{x}) = \sum_{i=1}^q \vect{c}_i f_i(\vect{x})
#
# where :math:`\vect{c}_i \in \mathbb{R} ^q`.
#
# We use the :class:`~openturns.DualLinearCombinationFunction` class.
# In the example, we take :math:`d=2` and :math:`q=3`.

c1 = [2.0, 3.0, 4.0]
c2 = [5.0, 6.0, 7.0]
c3 = [8.0, 9.0, 10.0]
coef_list = [c1, c2, c3]
f = ot.DualLinearCombinationFunction(func_coll, coef_list)
print("input dimension =", f.getInputDimension())
print("output dimension =", f.getOutputDimension())
print("f = ", f)
