"""
=========================================
Get the gradient and hessian as functions
=========================================
"""

# %%
# Context
# =======
#
# Let :math:`f : \mathbb{R}^\inputDim \mapsto \mathbb{R}^q` a vectorial function defined by
# its component functions :math:`f_i: \mathbb{R}^d \mapsto \mathbb{R}`. We want to get its gradient
# and hessian as functions. The way to get it depends on the type of the function :math:`f`.
#
# We can do that:
#
# - Case 1: This case covers the :class:`~openturns.SymbolicFunction`,
# - Case 2: This case covers all the general case.
#
import openturns as ot

# %%
# Case 1: The :class:`~openturns.SymbolicFunction`
# ================================================
#
# Here, the function :math:`f` is a :class:`~openturns.SymbolicFunction`. In this
# particular case, we have access to the analytical
# expression of the gradient and hessian functions.
#
# Let :math:`f : \Rset^2 \rightarrow \Rset^2` be defined by:
#
# .. math::
#    f(\vect{x}) = (f_1(x_1, x_2), f_2(x_1, x_2)) = (x_1^2 + 2x_2^2, x_1 + 3x_2^4)
#
# We want to define the gradient function :math:`\Rset^2 \rightarrow \Rset^4` defined by:
#
# .. math::
#    :label: gradientFct
#
#    \vect{x} \rightarrow & = (\dfrac{\partial f_1 (\vect{x})}{\partial x_1},
#                     \dfrac{\partial f_1 (\vect{x})}{\partial x_2},
#                     \dfrac{\partial f_2 (\vect{x})}{\partial x_1},
#                     \dfrac{\partial f_2 (\vect{x})}{\partial x_2})\\
#                & = (2x_1, 4x_2, 1, 12x_2^3)
#
# To get the gradient as a function, we first get the analytical expressions of the derivatives:
f = ot.SymbolicFunction(['x1', 'x2'], ['x1^2 + 2 * x2^2', 'x1 + 3 * x2^4'])
formula_f1_deriv_x1 = f.getGradient().getImplementation().getFormula(0, 0)
formula_f1_deriv_x2 = f.getGradient().getImplementation().getFormula(1, 0)
formula_f2_deriv_x1 = f.getGradient().getImplementation().getFormula(0, 1)
formula_f2_deriv_x2 = f.getGradient().getImplementation().getFormula(1, 1)

# %%
# Then we create a new :class:`~openturns.SymbolicFunction` from these analytical expressions:
gradient_AsFunction = ot.SymbolicFunction(['x1', 'x2'], [formula_f1_deriv_x1, formula_f1_deriv_x2,
                                                         formula_f2_deriv_x1, formula_f2_deriv_x2])
print(gradient_AsFunction)

# %%
# We want to define the hessian function :math:`\Rset^2 \rightarrow \Rset^6` defined by:
#
# .. math::
#    :label: hessianFct
#
#    \vect{x} \rightarrow & = (\dfrac{\partial^2 f_1 (\vect{x})}{\partial x_1^2},
#                    \dfrac{\partial^2 f_1 (\vect{x})}{ \partial x_1\partial x_2},
#                    \dfrac{\partial^2 f_1 (\vect{x})}{\partial x_2^2},
#                    \dfrac{\partial f_2 (\vect{x})}{\partial x_1^2},
#                    \dfrac{\partial f_2 (\vect{x})}{ \partial x_1\partial x_2}),
#                    \dfrac{\partial^2 f_2 (\vect{x})}{\partial x_2^2}\\
#                & = (2, 0 , 4, 0, 0, 36x_2^2)
#
# To get the hessian as a function, we first get the analytical expressions of the second derivatives:
formula_f1_hessian_x1x1 = f.getHessian().getImplementation().getFormula(0, 0, 0)
formula_f1_hessian_x1x2 = f.getHessian().getImplementation().getFormula(1, 0, 0)
formula_f1_hessian_x2x2 = f.getHessian().getImplementation().getFormula(1, 1, 0)
formula_f2_hessian_x1x1 = f.getHessian().getImplementation().getFormula(0, 0, 1)
formula_f2_hessian_x1x2 = f.getHessian().getImplementation().getFormula(1, 0, 1)
formula_f2_hessian_x2x2 = f.getHessian().getImplementation().getFormula(1, 1, 1)

# %%
# Then we create a new :class:`~openturns.SymbolicFunction` from these analytical expressions:
hessian_AsFunction = ot.SymbolicFunction(['x1', 'x2'], [formula_f1_hessian_x1x1, formula_f1_hessian_x1x2,
                                                        formula_f1_hessian_x2x2, formula_f2_hessian_x1x1,
                                                        formula_f2_hessian_x1x2, formula_f2_hessian_x2x2])
print(hessian_AsFunction)

# %%
# Case 2: The general case
# ========================
#
# Here, we consider a function :math:`f` such that we do not have access to the analytical
# expression of its gradient and hessian functions.
#
# To get the gradient function as a function defined in :eq:`gradientFct`, we have to use a
# :class:`~openturns.PythonFunction`. We re-use the prevous function for for educational purposes.


def gradient_AsFunction_Python(inPoint):
    f1_deriv_x1 = f.gradient(inPoint)[0, 0]
    f1_deriv_x2 = f.gradient(inPoint)[1, 0]
    f2_deriv_x1 = f.gradient(inPoint)[0, 1]
    f2_deriv_x2 = f.gradient(inPoint)[1, 1]
    return [f1_deriv_x1, f1_deriv_x2, f2_deriv_x1, f2_deriv_x2]


gradient_AsFunction_OT = ot.PythonFunction(2, 4, gradient_AsFunction_Python)
print(gradient_AsFunction_OT([1.0, 2.0]))

# %%
# To get the hessian function as a functiond efined in :eq:`hessianFct`, we do the same:


def hessian_AsFunction_Python(inPoint):
    f1_hessian_x1x1 = f.hessian(inPoint)[0, 0, 0]
    f1_hessian_x1x2 = f.hessian(inPoint)[1, 0, 0]
    f1_hessian_x2x2 = f.hessian(inPoint)[1, 1, 0]
    f2_hessian_x1x1 = f.hessian(inPoint)[0, 0, 1]
    f2_hessian_x1x2 = f.hessian(inPoint)[1, 0, 1]
    f2_hessian_x2x2 = f.hessian(inPoint)[1, 1, 1]
    return [f1_hessian_x1x1, f1_hessian_x1x2, f1_hessian_x2x2, f2_hessian_x1x1, f2_hessian_x1x2, f2_hessian_x2x2]


hessian_AsFunction_OT = ot.PythonFunction(2, 6, hessian_AsFunction_Python)
print(hessian_AsFunction_OT([1.0, 2.0]))
