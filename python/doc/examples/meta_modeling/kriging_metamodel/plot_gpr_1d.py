"""
Gaussian Process Regression : quick-start
=========================================
"""

# %%
# Abstract
# --------
#
# In this example, we create a Gaussian process Regression for a function which has
# scalar real inputs and outputs.
# We show how to create the learning and the validation samples.
# We show how to create the metamodel by choosing a trend and a covariance model.
# Finally, we compute the predicted confidence interval using the conditional variance.

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
#  :math:`i`    1   2   3   4    5     6    7
# ============ === === === === ===== ==== ======
#  :math:`x_i`  1   3   4   6   7.9   11   11.5
# ============ === === === === ===== ==== ======
#
# We are going to consider a Gaussian Process Regression metamodel with:
#
# * a constant trend,
# * a Matern covariance model.

# %%
# Creation of the metamodel
# -------------------------
#
# We begin by defining the function `g` as a symbolic function.
# Then we define the `x_train` variable which contains the inputs of the design of experiments of the training step.
# Then we compute the `y_train` corresponding outputs. The variable `n_train` is the size of the training design of experiments.

# %%
import openturns as ot
from openturns import viewer
from matplotlib import pylab as plt
import openturns.experimental as otexp

ot.Log.Show(ot.Log.NONE)

# %%
g = ot.SymbolicFunction(["x"], ["sin(x)"])

# %%
x_train = ot.Sample([[x] for x in [1.0, 3.0, 4.0, 6.0, 7.9, 11.0, 11.5]])
y_train = g(x_train)
n_train = x_train.getSize()
n_train

# %%
# In order to compare the function and its metamodel, we use a test (i.e. validation) design of experiments made of a regular grid of 100 points from 0 to 12.
# Then we convert this grid into a `Sample` and we compute the outputs of the function on this sample.

# %%
xmin = 0.0
xmax = 12.0
n_test = 100
step = (xmax - xmin) / (n_test - 1)
myRegularGrid = ot.RegularGrid(xmin, step, n_test)
x_test = myRegularGrid.getVertices()
y_test = g(x_test)


# %%
# In order to observe the function and the location of the points in the input design of experiments, we define the following functions which plots the data.

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
graph = ot.Graph("test and train", "", "", True, "")
graph.add(plot_1d_data(x_test, y_test, legend="Exact", color="black", linestyle="dashed"))
graph.add(plot_1d_data(x_train, y_train, type="Cloud", legend="Data", color="red"))
graph.setAxes(True)
graph.setXTitle("X")
graph.setYTitle("Y")
graph.setLegendPosition("upper right")
view = viewer.View(graph)

# %%
# We use the :class:`~openturns.ConstantBasisFactory` class to define the trend and the :class:`~openturns.MaternModel` class to define the covariance model.
# This Matérn model is based on the regularity parameter :math:`\nu=3/2`.

# %%
dimension = 1
basis = ot.ConstantBasisFactory(dimension).build()
covarianceModel = ot.MaternModel([1.0] * dimension, 1.5)
fitter_algo = otexp.GaussianProcessFitter(x_train, y_train, covarianceModel, basis)
fitter_algo.run()
fitter_result = fitter_algo.getResult()
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()
print(gpr_result)

# %%
# We observe that the `scale` and `amplitude` hyper-parameters have been optimized by the :meth:`~openturns.experimental.GaussianProcessFitter.run` method.
# Then we get the metamodel with `getMetaModel` and evaluate the outputs of the metamodel on the test design of experiments.

# %%
gprMetamodel = gpr_result.getMetaModel()
y_test_MM = gprMetamodel(x_test)


# %%
# Now we plot Gaussian process Regression output, in addition to the previous plots


# %%
graph = ot.Graph("", "", "", True, "")
graph.add(plot_1d_data(x_test, y_test, legend="Exact", color="black", linestyle="dashed"))
graph.add(plot_1d_data(x_train, y_train, type="Cloud", legend="Data", color="red"))
graph.add(plot_1d_data(x_test, y_test_MM, legend="GPR", color="blue"))
graph.setAxes(True)
graph.setXTitle("X")
graph.setYTitle("Y")
graph.setLegendPosition("upper right")
view = viewer.View(graph)

# %%
# We see that the Gaussian process regression is interpolating. This is what is meant by *conditioning* a Gaussian process.
#
# We see that, when the sine function has a strong curvature between two points which are separated by a large distance (e.g. between :math:`x=4` and :math:`x=6`),
# then the gaussian regression is not close to the function :math:`g`.
# However, when the training points are close (e.g. between :math:`x=11` and :math:`x=11.5`) or when the function is nearly linear (e.g. between :math:`x=8` and :math:`x=11`),
# then the gaussian process regression is quite accurate.

# %%
# Compute confidence bounds
# -------------------------

# %%
# In order to assess the quality of the metamodel, we can estimate the variance and compute a 95% confidence interval associated with the conditioned Gaussian process.
#
# We begin by defining the `alpha` variable containing the complementary of the confidence level than we want to compute.
# Then we compute the quantile of the Gaussian distribution corresponding to `1-alpha/2`. Therefore, the confidence interval is:
#
# .. math::
#    P\in\left(X\in\left[q_{\alpha/2},q_{1-\alpha/2}\right]\right)=1-\alpha.
#
#

# %%
alpha = 0.05


def computeQuantileAlpha(alpha):
    bilateralCI = ot.Normal().computeBilateralConfidenceInterval(1 - alpha)
    return bilateralCI.getUpperBound()[0]


quantileAlpha = computeQuantileAlpha(alpha)
print("alpha=%f" % (alpha))
print("Quantile alpha=%f" % (quantileAlpha))

# %%
# In order to compute the regression error, we can consider the conditional variance.
# The :meth:`~openturns.experimental.GaussianProcessConditionalCovariance.getConditionalMarginalVariance` method returns the covariance matrix `covGrid`
# evaluated at each points in the given sample. Then we can use the diagonal
# coefficients in order to get the marginal conditional Kriging variance.
# Since this is a variance, we use the square root in order to compute the
# standard deviation.
# However, some coefficients in the diagonal are very close to zero and
# nonpositive, which leads to an exception of the sqrt function.
# This is why we add an epsilon on the diagonal (nugget factor), which prevents this issue.

# %%
sqrt = ot.SymbolicFunction(["x"], ["sqrt(x)"])
epsilon = ot.Sample(n_test, [1.0e-8])
gccc = otexp.GaussianProcessConditionalCovariance(gpr_result)
conditionalVariance = gccc.getConditionalMarginalVariance(x_test) + epsilon
conditionalSigma = sqrt(conditionalVariance)

# %%
# The following figure presents the conditional standard deviation depending on :math:`x`.

# %%
graph = ot.Graph(
    "Conditional standard deviation", "x", "Conditional standard deviation", True, ""
)
curve = ot.Curve(x_test, conditionalSigma)
graph.add(curve)
view = viewer.View(graph)


# %%
# We now compute the bounds of the confidence interval. For this purpose we define a small function
# `computeBoundsConfidenceInterval` :


# %%
def computeBoundsConfidenceInterval(quantileAlpha):
    dataLower = [
        [y_test_MM[i, 0] - quantileAlpha * conditionalSigma[i, 0]]
        for i in range(n_test)
    ]
    dataUpper = [
        [y_test_MM[i, 0] + quantileAlpha * conditionalSigma[i, 0]]
        for i in range(n_test)
    ]
    dataLower = ot.Sample(dataLower)
    dataUpper = ot.Sample(dataUpper)
    return dataLower, dataUpper


# %%
# We define two small lists to draw three different confidence intervals (defined by the alpha value) :
alphas = [0.05, 0.1, 0.2]
# three different green colors defined by HSV values
mycolors = [[120, 1.0, 1.0], [120, 1.0, 0.75], [120, 1.0, 0.5]]

# %%
# We are ready to display all the previous information and the three confidence intervals we want.

# %%
# sphinx_gallery_thumbnail_number = 4
graph = ot.Graph("", "", "", True, "")

# Now we loop over the different values :
for idx, v in enumerate(alphas):
    quantileAlpha = computeQuantileAlpha(v)
    vLow, vUp = computeBoundsConfidenceInterval(quantileAlpha)
    boundsPoly = ot.Polygon.FillBetween(x_test, vLow, vUp)
    boundsPoly.setColor(
        ot.Drawable.ConvertFromHSV(mycolors[idx][0], mycolors[idx][1], mycolors[idx][2])
    )
    boundsPoly.setLegend(" %d%% bounds" % ((1.0 - v) * 100))
    graph.add(boundsPoly)

graph.add(plot_1d_data(x_test, y_test, legend="Exact", color="black", linestyle="dashed"))
graph.add(plot_1d_data(x_train, y_train, type="Cloud", legend="Data", color="red"))
graph.add(plot_1d_data(x_test, y_test_MM, legend="GPR", color="blue"))

graph.setAxes(True)
graph.setXTitle("X")
graph.setYTitle("Y")
graph.setLegendPosition("upper right")
view = viewer.View(graph)

# %%
# We see that the confidence intervals are small in the regions where two
# consecutive training points are close to each other
# (e.g. between :math:`x=11` and :math:`x=11.5`) and large when the two points
# are not (e.g. between :math:`x=8.` and :math:`x=11`) or when the curvature
# of the function is large (between :math:`x=4` and :math:`x=6`).

plt.show()
