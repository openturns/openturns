"""
Create a linear least squares model
===================================
"""
# %%
# In this example we are going to create a global approximation of a model
# response based on a linear function using the :class:`~openturns.LinearLeastSquares` class.
#
# We consider the function :math:`h : \Rset^2 \rightarrow \Rset^2` is defined by:
#
# .. math::
#    h(x_1, x_2) = \left(\cos(x_1 + x_2), (x_2 + 1) e^{x_1 - 2 x_2} \right)
#
# for any :math:`\vect{x} \in \Rset^2`.
# Since the output is a dimension 2 vector, the model has vector
# coefficients.
# We use the linear model:
#
# .. math::
#     \vect{y} \, \approx \, \widehat{\vect{h}}(\vect{x}) \,
#     = \, \sum_{k=0}^2 \; \vect{a}_k \; \psi_k(\vect{x})
#
#
# for any :math:`\vect{x} \in \Rset^2` where :math:`\left\{\vect{a}_k \in \Rset^2\right\}_{k = 0,..., 2}`
# are the vector coefficients and :math:`\left\{\psi_k : \Rset^2 \rightarrow \Rset\right\}_{k = 0, ..., 2}`
# are the basis functions.
# This implies that each marginal output :math:`\widehat{h}_i(\vect{x})`
# is approximated by the linear model:
#
# .. math::
#     \widehat{h}_i(\vect{x}) \,
#     = \, \sum_{k=0}^2 \; a_{ki} \; \psi_k(\vect{x})
#
# for any :math:`\vect{x} \in \Rset^2` and any :math:`i = 1, 2`
# where :math:`a_{ki}` is the :math:`k`-th coefficient of the :math:`i`-th output
# marginal:
#
# .. math::
#      \vect{a}_k = \begin{pmatrix}a_{k1} \\ a_{k2} \end{pmatrix}
#
# for :math:`k = 0, 1, 2`.
# We consider the basis functions:
#
# .. math::
#
#      \psi_0(\vect{x}) & = 1 \\
#      \psi_1(\vect{x}) & = x_1 \\
#      \psi_2(\vect{x}) & = x_2 \\
#
# for any :math:`\vect{x} \in \Rset^2`.


# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# Define the model
# ~~~~~~~~~~~~~~~~

# %%
# Prepare an input sample.
# Each point is a pair of coordinates :math:`(x_1, x_2)`.

# %%
inputTrain = [[0.5, 0.5], [-0.5, -0.5], [-0.5, 0.5], [0.5, -0.5]]
inputTrain += [[0.25, 0.25], [-0.25, -0.25], [-0.25, 0.25], [0.25, -0.25]]
inputTrain = ot.Sample(inputTrain)
inputTrain.setDescription(["x1", "x2"])
inputTrain

# %%
# Compute the output sample from the input sample and a function.

# %%
formulas = ["cos(x1 + x2)", "(x2 + 1) * exp(x1 - 2 * x2)"]
model = ot.SymbolicFunction(["x1", "x2"], formulas)
model.setOutputDescription(["y1", "y2"])
outputTrain = model(inputTrain)
outputTrain

# %%
# Linear least squares
# ~~~~~~~~~~~~~~~~~~~~

# %%
# Create a linear least squares model.

# %%
algo = ot.LinearLeastSquares(inputTrain, outputTrain)
algo.run()

# %%
# Get the linear term.

# %%
algo.getLinear()

# %%
# Get the constant term.

# %%
algo.getConstant()

# %%
# Get the metamodel.
responseSurface = algo.getMetaModel()

# %%
# Plot the second output of our model with :math:`x_1=0.5`.

# %%
graph = ot.ParametricFunction(model, [0], [0.5]).getMarginal(1).draw(-0.5, 0.5)
graph.setLegends(["Model"])
curve = (
    ot.ParametricFunction(responseSurface, [0], [0.5])
    .getMarginal(1)
    .draw(-0.5, 0.5)
    .getDrawable(0)
)
curve.setLineStyle("dashed")
curve.setLegend("Linear L.S.")
graph.add(curve)
graph.setLegendPosition("topright")
graph.setColors(ot.Drawable.BuildDefaultPalette(2))
view = viewer.View(graph)
plt.show()
