"""
Gaussian Process Regression: heteroskedastic noise
==================================================
"""

# %%
# The goal of this example is to estimate a Gaussian process regression accounting for a noise (known noise).
#
# It assumes a basic knowledge of Gaussian Process Regression.
# For that purpose, we create a Gaussian Process Regression surrogate model for a function which has scalar real inputs and outputs.

# %%
# Introduction
# ------------
#
# We consider the sine function:
#
# .. math::
#    y = x \sin(x)
#
#
# for any :math:`x\in[0,12]`.
#
# We want to create a surrogate of this function using GPR:
#
# .. math::
#    y_k=x_k \sin(x_k)
#
# We will consider a known centered Gaussian observation noise:
#
# .. math::
#   \widehat{\vect{y}}_k
#    = \vect{y}_k + \epsilon_k, \quad \epsilon_k \sim \mathcal{N}\left(0, \tau_k^2\right)

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# First let us introduce some useful function.
# In order to observe the function and the location of the points in the input design of experiments, we define `plot_1d_data`.


# %%
def plot_1d_data(x_data, y_data, type="Curve", legend=None, color=None, linestyle=None):
    """Plot the data (x_data,y_data) as a Cloud/Curve"""
    if type == "Curve":
        graphF = ot.Curve(x_data, y_data)
    else:
        graphF = ot.Cloud(x_data, y_data)
    if legend is not None:
        graphF.setLegend(legend)
    if color is not None:
        graphF.setColor(color)
    if linestyle is not None:
        graphF.setLineStyle(linestyle)
    return graphF


# %%
g = ot.SymbolicFunction(["x"], ["x * sin(x)"])

# %%
xmin = 0.0
xmax = 12.0
n_train = 20
step = (xmax - 1 - xmin) / (n_train - 1.0)
x_train = ot.RegularGrid(xmin + 0.2, step, n_train).getVertices()
y_train = g(x_train)
n_train = x_train.getSize()

# %%
# In order to compare the function and its metamodel, we use a test (i.e. validation) design of experiments made of a regular grid of 100 points from 0 to 12.
# Then we convert this grid into a :class:`~openturns.Sample` and we compute the outputs of the function on this sample.

# %%
n_test = 100
step = (xmax - xmin) / (n_test - 1)
myRegularGrid = ot.RegularGrid(xmin, step, n_test)
x_test = myRegularGrid.getVertices()
y_test = g(x_test)

# %%
# We plot the true function (continuous dashed curve) and train data (cloud points) on the same figure.
graph = ot.Graph("Function of interest", "", "", True, "")
graph.add(
    plot_1d_data(x_test, y_test, legend="Exact", color="black", linestyle="dashed")
)
graph.add(
    plot_1d_data(x_train, y_train, type="Cloud", legend="Train points", color="red")
)
graph.setAxes(True)
graph.setXTitle("X")
graph.setYTitle("Y")
graph.setLegendPosition("upper right")
view = otv.View(graph)

# %%
# We use the :class:`~openturns.ConstantBasisFactory` class to define the trend and the :class:`~openturns.MaternModel` class to define the covariance model.
# This Matérn model is based on the regularity parameter :math:`\nu=3/2`.

# %%
dimension = 1
basis = ot.ConstantBasisFactory(dimension).build()
covarianceModel = ot.MaternModel([1.0] * dimension, 1.5)

# %%
noise = ot.Uniform(0, 0.5).getSample(y_train.getSize())
fitter_algo = ot.GaussianProcessFitter(x_train, y_train, covarianceModel, basis)
fitter_algo.setNoise(noise.asPoint())
fitter_algo.run()
fitter_result = fitter_algo.getResult()
gpr_algo_hsn = ot.GaussianProcessRegression(fitter_result)
gpr_algo_hsn.run()
gpr_result_hsn = gpr_algo_hsn.getResult()
gprMM_hsn = gpr_result_hsn.getMetaModel()

# %%
# Plot the result

# %%
graph = ot.Graph("GPR with known noise", "", "", True, "")
graph.add(
    plot_1d_data(x_test, y_test, legend="Exact", color="black", linestyle="dashed")
)
graph.add(plot_1d_data(x_train, y_train, type="Cloud", legend="Data", color="red"))
graph.add(
    plot_1d_data(x_test, gprMM_hsn(x_test), legend="GPR+noise", color="green")
)
graph.setAxes(True)
graph.setXTitle("X")
graph.setYTitle("Y")
graph.setLegendPosition("upper right")
view = otv.View(graph)

# %%
# The result is slightly different from the previous ones. We take into account that each output `y_train` is potentially "random".

# %%
# Display all figures
otv.View.ShowAll()
