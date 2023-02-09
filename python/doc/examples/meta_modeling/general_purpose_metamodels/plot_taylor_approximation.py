"""
Taylor approximations
=====================
"""
# %%
# In this example we are going to build a local approximation of a model using the Taylor decomposition:
#
# Here is the decomposition at the first order:
#
# .. math::
#    \vect{y} \, \approx \, \widehat{h}(\vect{x}) \,
#          = \, h(\vect{x}_0) \, +
#         \, \sum_{i=1}^{n_{X}} \; \frac{\partial h}{\partial x_i}(\vect{x}_0).\left(x_i - x_{0,i} \right)
#
# where :math:`h : \Rset^2 \rightarrow \Rset^2` is defined by:
#
# .. math::
#    h(\vect{x}) = \left( \cos(x_1 + x_2), (x_2 + 1) e^{x_1 - 2 x_2} \right).
#

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# Prepare some data.
formulas = ["cos(x1 + x2)", "(x2 + 1) * exp(x1 - 2 * x2)"]
model = ot.SymbolicFunction(["x1", "x2"], formulas)

# Center of the approximation.
x0 = [-0.4, -0.4]

# Drawing bounds.
a = -0.4
b = 0.0

# %%
# Create a linear (first-order) Taylor approximation.
algo = ot.LinearTaylor(x0, model)
algo.run()
responseSurface = algo.getMetaModel()

# %%
# Plot the second output of our model with :math:`x_1=x_{0,1}`.
graph = ot.ParametricFunction(responseSurface, [0], [x0[1]]).getMarginal(1).draw(a, b)
graph.setLegends(["taylor"])
curve = (
    ot.ParametricFunction(model, [0], [x0[1]]).getMarginal(1).draw(a, b).getDrawable(0)
)
curve.setColor("red")
curve.setLegend("model")
graph.add(curve)
graph.setLegendPosition("topright")
view = viewer.View(graph)

# %%
# Here is the decomposition at the second order:
#
# .. math::\vect{y} \, \approx \, \widehat{h}(\vect{x}) \, = \,
#         h(\vect{x}_0) \, + \, \sum_{i=1}^{n_{X}} \;
#       \frac{\partial h}{\partial x_i}(\vect{x}_0).\left(x_i - x_{0,i} \right) \, +
#      \, \frac{1}{2} \; \sum_{i,j=1}^{n_X} \;
#       \frac{\partial^2 h}{\partial x_i \partial x_j}(\vect{x}_0).\left(x_i - x_{0,i} \right).\left(x_j - x_{0,j} \right):math:``

# %%
# Create a quadratic (second-order) Taylor approximation.
algo = ot.QuadraticTaylor(x0, model)
algo.run()
responseSurface = algo.getMetaModel()

# %%
# Plot second output of our model with :math:`x_1=x_{0,1}`.
graph = ot.ParametricFunction(responseSurface, [0], [x0[1]]).getMarginal(1).draw(a, b)
graph.setLegends(["taylor"])
curve = (
    ot.ParametricFunction(model, [0], [x0[1]]).getMarginal(1).draw(a, b).getDrawable(0)
)
curve.setColor("red")
curve.setLegend("model")
graph.add(curve)
graph.setLegendPosition("topright")
view = viewer.View(graph)
plt.show()
