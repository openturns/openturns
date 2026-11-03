"""
Select a finite difference step with Stepleman-Winarsky
=======================================================
"""

# %%
# **Introduction**
#
# The centered finite difference approximates the first derivative of a
# function :math:`f` at a point :math:`x`:
#
# .. math::
#    D(h) = \frac{f(x + h) - f(x - h)}{2h}
#
# The approximation error combines a truncation term, which decreases when
# the step :math:`h` decreases, and a round-off term, which grows when
# :math:`h` decreases because of floating point cancellations.
# The error as a function of :math:`h` is U-shaped and its minimum defines
# the optimal step, which depends on the function scaling.
#
# The :class:`~openturns.experimental.SteplemanWinarsky` algorithm
# [stepleman1979]_ selects a step close to the optimum adaptively:
# successive steps :math:`h_{i+1} = \alpha h_i` with :math:`0 < \alpha < 1`
# are tried and the search stops as soon as the successive differences grow,
# which indicates that round-off dominates.
#
# In this example we plot the finite difference error against the step for
# a badly scaled exponential model and compare the minimum of the curve
# with the step selected by the algorithm.

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
from matplotlib import pyplot as plt
from openturns.usecases import chaboche_model
import math

# %%
# **The exponential model**
#
# We consider :math:`f(x) = \exp(-x / \alpha)` with
# :math:`\alpha = 10^6` at :math:`x = 10^{-2}`.
# The exact derivative is :math:`f'(x) = -\exp(-x / \alpha) / \alpha`.
alpha = 1.0e6
x = [1.0e-2]


def scaled_exp(X):
    return [math.exp(-X[0] / alpha)]


f = ot.PythonFunction(1, 1, scaled_exp)
exact = -math.exp(-x[0] / alpha) / alpha
print("exact derivative = %.6e" % exact)

# %%
# Run the adaptive search from a deliberately large initial step.
# A fixed default step would be far too large here.
initial_step = [1.0e8]
algo = otexp.SteplemanWinarsky(f, x, initial_step)
algo.run()
h_star = algo.getResult().getOptimalStep()[0]
print("h* = %.3e" % h_star)


# %%
# **Error against the step**
#
# We evaluate the centered finite difference error over a logarithmic
# grid of steps and locate its minimum.
def fd_error(h):
    return abs((f([x[0] + h])[0] - f([x[0] - h])[0]) / (2.0 * h) - exact)


number_of_points = 200
h_min, h_max = 1.0e-10, 1.0e8
steps = [h_min * (h_max / h_min) ** (i / (number_of_points - 1.0)) for i in range(number_of_points)]
errors = [fd_error(h) for h in steps]
minimum_error = min(errors)
maximum_error = max(errors)
print("minimum grid error = %.3e" % minimum_error)
print("error at h* = %.3e" % fd_error(h_star))

# %%
# The U-shaped curve shows the truncation regime on the right (error
# decreasing with :math:`h`) and the round-off regime on the left (error
# growing when :math:`h` decreases). The step selected by the algorithm
# lies close to the bottom of the curve.
graph = ot.Graph("Finite difference error: scaled exponential", "$h$", "Absolute error")
curve = ot.Curve(steps, errors)
curve.setLegend("Centered finite difference")
graph.add(curve)
vertical = ot.Curve([h_star, h_star], [minimum_error, maximum_error])
vertical.setLineStyle("dashed")
vertical.setLegend("$h^*$ (Stepleman-Winarsky)")
graph.add(vertical)
point = ot.Cloud([[h_star]], [[fd_error(h_star)]])
point.setPointStyle("bullet")
point.setLegend("Error at $h^*$")
graph.add(point)
graph.setLogScale(ot.GraphImplementation.LOGXY)
graph.setLegendPosition("upper left")
_ = otv.View(graph, figure_kw={"figsize": (6.0, 5.0)})
plt.tight_layout()

# %%
# **Reuse the optimal step**
#
# The result provides the model with its gradient replaced by a centered
# finite difference gradient using the optimal step, ready for
# gradient-based algorithms.
fd_function = algo.getResult().getFunctionWithOptimalStep()
print("gradient implementation =", fd_function.getGradient().getImplementation().getClassName())
print("gradient at x =", fd_function.gradient(x))

# %%
# **Apply the method to an actual model: Chaboche**
#
# The :class:`~openturns.usecases.chaboche_model.ChabocheModel` mechanical law
# maps :math:`(\epsilon, R, C, \gamma)` to the stress :math:`\sigma`.
# Its inputs span very different scales (:math:`\epsilon \sim 10^{-2}`,
# :math:`R, C \sim 10^9`), so a single scalar finite-difference step cannot
# fit all directions: the algorithm selects one optimal step per direction.
# We force a finite difference gradient for the illustration purpose as the function is symbolic.
# We can see the gradient has a nan value and that probably indicates the chosen arbitrary
# finite difference step value is not adapted for all the parameters of the model.
cm = chaboche_model.ChabocheModel()
chaboche = cm.model
epsilon = 1e-7
chaboche.setGradient(ot.CenteredFiniteDifferenceGradient(epsilon, chaboche.getEvaluation()))
print("Inputs:", chaboche.getInputDescription())
x_chaboche = [0.02, 750.0e6, 2750.0e6, 10.0]
print("ref gradient =", chaboche.gradient(x_chaboche))
initial_chaboche = [1.0e-3, 1.0e8, 1.0e8, 1.0]

# %%
# Calibrate the finite difference step and compare the gradient.
# We see the step is quite large for some of the R,C parameters (~1e6).
# But now the gradient is properly computed without any nan value.
algo_chaboche = otexp.SteplemanWinarsky(chaboche, x_chaboche, initial_chaboche)
algo_chaboche.run()
print("h* =", algo_chaboche.getResult().getOptimalStep())
fd_chaboche = algo_chaboche.getResult().getFunctionWithOptimalStep()
print("optimal gradient =", fd_chaboche.gradient(x_chaboche))

# %%
# Display all figures
otv.View.ShowAll()
