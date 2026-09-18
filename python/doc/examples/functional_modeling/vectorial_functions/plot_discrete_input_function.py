"""
Function with a discrete input
==============================

This example shows how to define a function which depends on a discrete
input variable, ie a variable which only takes a finite set of values,
and discusses the consequences for the gradient of the function.

In reliability or optimization studies, some variables are discrete:
a number of components, a maintenance policy, a design choice, etc.
Such variables cannot be handled like continuous ones when computing
derivatives, because an infinitesimal change of a discrete variable is
meaningless.
"""
# %%
# Definition using the input as an index
# ======================================
#
# A convenient way to define a function with a discrete input is to use it
# as an index in a table of numeric values. We consider a variant of the
# Swiler 2014 problem (see also ``t_Bonmin_swiler2014``), where the first
# input :math:`x_1` is discrete in :math:`\{0, 1, 2, 3, 4\}` and selects one
# of five coefficients:
#
# .. math::
#    f(x_1, x_2) = \left(1 + c_{\mathrm{round}(x_1)}\right)\,\sin(x_2)
#
import math
import openturns as ot

coefficients = [0.0, 12.0, 0.5, 8.0, 3.5]


def model(x):
    return [math.sin(x[1]) * (1.0 + coefficients[int(round(x[0]))])]


fun = ot.PythonFunction(2, 1, model)
print(fun([1.0, math.pi / 3]))
print(fun([4.0, math.pi / 3]))

# %%
# Impact on the gradient
# ======================
#
# The function is piecewise constant along :math:`x_1`, so its derivative
# with respect to that variable does not exist. However, OpenTURNS builds a
# default finite difference gradient for Python functions. Away from the
# discontinuities, the estimated derivative along the discrete direction is
# zero, which silently ignores any dependence of the output on
# :math:`x_1`:

print(fun.gradient([1.0, math.pi / 3]))

# %%
# The situation is worse close to a discontinuity: if the finite difference
# step crosses a boundary between two levels, the quotient of two very small
# steps by a large jump produces an arbitrarily large value. Here the
# estimate at :math:`x_1 = 0.499999` is completely misleading:

print(fun([0.49, math.pi / 3]))
print(fun([0.51, math.pi / 3]))
print(fun.gradient([0.499999, math.pi / 3]))

# %%
# Consequences for optimization
# =============================
#
# Finite differences should not be trusted along discrete directions, both
# by metamodel-based algorithms and by optimization solvers relying on
# derivatives. When optimizing such a function, the discrete nature of the
# variable must be declared explicitly so that dedicated solvers handle it.
# This is done with :meth:`~openturns.OptimizationProblem.setVariablesType`,
# as illustrated in the mixed integer optimization examples using
# :class:`~openturns.Bonmin`.

problem = ot.OptimizationProblem(fun)
problem.setMinimization(True)
problem.setBounds(ot.Interval([0.0, 0.0], [4.0, math.pi]))
problem.setVariablesType(
    [
        ot.OptimizationProblemImplementation.INTEGER,
        ot.OptimizationProblemImplementation.CONTINUOUS,
    ]
)
