"""
Create a custom covariance model
================================
"""

# %%
# This example illustrates how the user can define his own covariance model in both cases:
#
# - Case 1: stationary covariance model,
# - Case 2: any covariance model.
#

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pyplot as plt
import math as m

ot.Log.Show(ot.Log.NONE)


# %%
# Case 1: stationary covariance model
# -----------------------------------
#
# In this example, we show how to build our own stationary covariance modelusing the class
# :class:`~openturns.UserDefinedStationaryCovarianceModel`.
#
# We define the covariance model from:
#
# - a mesh :math:`\mathcal{M}` of dimension :math:`n` defined by the vertices :math:`(\vect{\tau}_0,\dots, \vect{\tau}_{N-1})`
#   and the associated simplices,
# - a collection of covariance matrices stored in the object :class:`~openturns.CovarianceMatrixCollection` denoted by
#   :math:`\vect{C}_0, \dots, \vect{C}_{N-1}`, where :math:`\vect{C}_k \in \mathcal{M}_{d \times d}(\mathbb{R})`
#   for :math:`0 \leq k \leq N-1`.
#
# Then we build a stationary covariance function which is a piecewise constant function on :math:`\mathcal{D}` defined by:
#
# .. math::
#    \forall \vect{\tau} \in \mathcal{D}, \, C^{stat}(\vect{\tau}) =  \vect{C}_k
#
# where :math:`k`  is such that :math:`\vect{\tau}_k` is the  vertex of :math:`\mathcal{M}` the nearest to :math:`\vect{t}.`

# %%
# We first create the time grid and the covariance function
t0 = 0.0
dt = 0.5
N = int((20.0 - t0) / dt)
mesh = ot.RegularGrid(t0, dt, N)


def gamma(tau):
    return 1.0 / (1.0 + tau * tau)


# We create the collection of :class:`~openturns.SquareMatrix`.
coll = ot.SquareMatrixCollection()
for k in range(N):
    t = mesh.getValue(k)
    matrix = ot.SquareMatrix([[gamma(t)]])
    coll.add(matrix)

# %%
# We create  the final stationary covariance model.
covmodel = ot.UserDefinedStationaryCovarianceModel(mesh, coll)

# %%
# We can draw the covariance function.
x = ot.Sample(N, 2)
for k in range(N):
    t = mesh.getValue(k)
    x[k, 0] = t
    value = covmodel(t)
    x[k, 1] = value[0, 0]

curve = ot.Curve(x, "User Model")
myGraph = ot.Graph("User covariance model", "Time", "Covariance function", True)
myGraph.add(curve)
myGraph.setLegendPosition("upper right")
view = viewer.View(myGraph)


# %%
# Case 2: any covariance model
# ----------------------------
#
# In this example, we build a covariance model from a time grid and a covariance function depending on :math:`s,t)`,
# using the class :class:`~openturns.UserDefinedCovarianceModel`.
N = 32
a = 4.0
mesh = ot.IntervalMesher([N]).build(ot.Interval(-a, a))


def C(s, t):
    return m.exp(-4.0 * abs(s - t) / (1 + (s * s + t * t)))


# %%
# The, we create the  covariance matrix on the mesh.
covariance = ot.CovarianceMatrix(mesh.getVerticesNumber())
for k in range(mesh.getVerticesNumber()):
    t = mesh.getVertices()[k]
    for ll in range(k + 1):
        s = mesh.getVertices()[ll]
        covariance[k, ll] = C(s[0], t[0])

# %%
# The, we create the final non stationary covariance model.
covmodel = ot.UserDefinedCovarianceModel(mesh, covariance)

# %%
# We can draw the covariance model as a function: first, we define the function to draw.


def f(x):
    return [covmodel([x[0]], [x[1]])[0, 0]]


func = ot.PythonFunction(2, 1, f)
func.setDescription(["$s$", "$t$", "$cov$"])

# %%
# Then we can draw the function with default options.
cov_graph = func.draw([-a] * 2, [a] * 2, [512] * 2)
cov_graph.setLegendPosition("")
view = viewer.View(cov_graph)


# %%
# We can draw the function in a filled contour graph.
# sphinx_gallery_thumbnail_number = 3
cov_graph = func.draw(
    0, 1, 0, [0] * 2, [-a] * 2, [a] * 2, [512] * 2, ot.GraphImplementation.NONE, True
)
view = viewer.View(cov_graph)

# %%
# We can also draw the covariance model as a matrix, using the raw matshow.
_ = plt.matshow(covariance)

# %%
# The, we draw the covariance model as a matrix with the correct axes.
#
# To obtain the correct orientation of the y axis we use the origin argument.
# To obtain the correct graduations we use the extent argument.
# We also change the colormap used.
pas = 2 * a / (N - 1)
plt.matshow(
    covariance,
    cmap="gray",
    origin="lower",
    extent=(-a - pas / 2, a + pas / 2, -a - pas / 2, a + pas / 2),
)
plt.show()

# %%
# Display all figures
viewer.View.ShowAll()
