"""
Kriging : generate trajectories from a metamodel
================================================
"""
# %%
# The main goal of this example is to show how to simulate new trajectories from a kriging metamodel.

# %%
# Introduction
# ------------
#
# We consider the sine function:
#
# .. math::
#    y = \sin(x)
#
#
# for any :math:`x\in[0,12]`.
#
# We want to create a metamodel of this function. This is why we create a sample of :math:`n` observations of the function:
#
# .. math::
#    y_i=\sin(x_i)
#
#
# for :math:`i=1,...,7`, where :math:`x_i` is the i-th input and :math:`y_i` is the corresponding output.
#
# We consider the seven following inputs :
#
# ============ === === === === ===== ==== ======
#  :math:`i`    1   2   3   4   5     6    7
#  :math:`x_i`  1   3   4   6   7.9   11   11.5
# ============ === === === === ===== ==== ======
#
# We are going to consider a kriging metamodel with a
#
# * constant trend,
# * a Matern covariance model.

# %%
# Creation of the metamodel
# -------------------------
#
# We begin by defining the function `g` as a symbolic function.
# Then we define the `x_train` variable which contains the inputs of the design of experiments of the training step.
# Then we compute the `y_train` corresponding outputs.
# The variable `n_train` is the size of the training design of experiments.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
g = ot.SymbolicFunction(["x"], ["sin(x)"])

# %%
x_train = [[x] for x in [1.0, 3.0, 4.0, 6.0, 7.9, 11.0, 11.5]]
y_train = g(x_train)
n_train = len(x_train)
n_train

# %%
# In order to compare the function and its metamodel, we use a test (i.e. validation) design of experiments
# made of a regular grid of 100 points from 0 to 12.
# Then we convert this grid into a `Sample` and we compute the outputs of the function on this sample.

# %%
xmin = 0.0
xmax = 12.0
n_test = 101
step = (xmax - xmin) / (n_test - 1)
myRegularGrid = ot.RegularGrid(xmin, step, n_test)
x_test = myRegularGrid.getVertices()
y_test = g(x_test)


# %%
# In order to observe the function and the location of the points in the input design of experiments, we define the following functions which plots the data.

# %%
def plot_data_train(x_train, y_train):
    """Plot the data (x_train,y_train) as a Cloud, in red"""
    graph_train = ot.Cloud(x_train, y_train)
    graph_train.setColor("red")
    graph_train.setLegend("Data")
    return graph_train


# %%
def plot_data_test(x_test, y_test):
    """Plot the data (x_test,y_test) as a Curve, in dashed black"""
    graphF = ot.Curve(x_test, y_test)
    graphF.setLegend("Exact")
    graphF.setColor("black")
    graphF.setLineStyle("dashed")
    return graphF


# %%
graph = ot.Graph()
graph.add(plot_data_test(x_test, y_test))
graph.add(plot_data_train(x_train, y_train))
graph.setAxes(True)
graph.setXTitle("X")
graph.setYTitle("Y")
graph.setLegendPosition("topright")
view = viewer.View(graph)

# %%
# We use the `ConstantBasisFactory` class to define the trend and the `MaternModel` class to define the covariance model. This Matérn model is based on the regularity parameter :math:`\nu=3/2`.

# %%
dimension = 1
basis = ot.ConstantBasisFactory(dimension).build()
covarianceModel = ot.MaternModel([1.0] * dimension, 1.5)
algo = ot.KrigingAlgorithm(x_train, y_train, covarianceModel, basis)
algo.run()
krigingResult = algo.getResult()
krigingResult

# %%
# We observe that the `scale` and `amplitude` hyper-parameters have been optimized by the `run` method. Then we get the metamodel with `getMetaModel` and evaluate the outputs of the metamodel on the test design of experiments.

# %%
krigeageMM = krigingResult.getMetaModel()
y_test_MM = krigeageMM(x_test)


# %%
# The following function plots the kriging data.

# %%
def plot_data_kriging(x_test, y_test_MM):
    """Plots (x_test,y_test_MM) from the metamodel as a Curve, in blue"""
    graphK = ot.Curve(x_test, y_test_MM)
    graphK.setColor("blue")
    graphK.setLegend("Kriging")
    return graphK


# %%
graph = ot.Graph()
graph.add(plot_data_test(x_test, y_test))
graph.add(plot_data_train(x_train, y_train))
graph.add(plot_data_kriging(x_test, y_test_MM))
graph.setAxes(True)
graph.setXTitle("X")
graph.setYTitle("Y")
graph.setLegendPosition("topright")
view = viewer.View(graph)

# %%
# Simulate new trajectories
# -------------------------
#
# In order to generate new trajectories of the conditioned gaussian process,
# we could technically use the `KrigingRandomVector` class, because it provides
# the `getSample` method that we need.
# However, the `KrigingRandomVector` class was more specifically designed to
# create a `RandomVector` so that it can feed, for example, a function which has a field as input argument.
#
# This is why we use the `ConditionedGaussianProcess`, which provides a `Process`.

# %%
process = ot.ConditionedGaussianProcess(krigingResult, myRegularGrid)

# %%
trajectories = process.getSample(10)
type(trajectories)

# %%
# The `getSample` method returns a `ProcessSample`. By comparison, the `getSample` method of a `KrigingRandomVector` would return a `Sample`.

# %%
# sphinx_gallery_thumbnail_number = 3
graph = trajectories.drawMarginal()
graph.add(plot_data_test(x_test, y_test))
graph.add(plot_data_train(x_train, y_train))
graph.setAxes(True)
graph.setXTitle("X")
graph.setYTitle("Y")
graph.setLegendPosition("topright")
graph.setTitle("10 simulated trajectories")
view = viewer.View(graph)
plt.show()

# %%
# References
# ----------
#
# * Metamodeling with Gaussian processes, Bertrand Iooss, EDF R&D, 2014, www.gdr-mascotnum.fr/media/sssamo14_iooss.pdf
#
