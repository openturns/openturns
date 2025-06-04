"""
Gaussian Process Regression : quick-start
=========================================
"""

# %%
# Abstract
# --------
#
# In this example, we create a Gaussian Process Regression for a function which has
# scalar real inputs and outputs.
# We show how to create the learning and the validation samples.
# We show how to create the surrogate model by choosing a trend and a covariance model.
# Finally, we compute the predicted confidence interval using the conditional variance.

# %%
# Introduction
# ------------
#
# We consider the sine function:
#
# .. math::
#    g(x) = \sin(x)
#
#
# for any :math:`x\in[0,12]`.
#
# We want to create a surrogate model of this function. This is why we create a sample of :math:`n` observations of the function:
#
# .. math::
#    y_i=g(x_i)
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
# We are going to consider a Gaussian Process Regression surrogate model with:
#
# * a constant trend,
# * a Matern covariance model.
#
# We begin by defining the function `g` as a symbolic function.
# Then we define the `x_train` variable which contains the inputs of the design of experiments of the training step.
# Then we compute the `y_train` corresponding outputs. The variable `n_train` is the size of the training design of experiments.

# %%
import openturns as ot
from openturns import viewer
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
# In order to compare the function and its surrogate model, we use a test (i.e. validation) design of experiments made of a regular grid of 100 points from 0 to 12.
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
# In order to observe the function and the location of the points in the input design of experiments, we define the following function which plots the data.


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
# Here, we draw the model and the train sample.
graph = ot.Graph("Model and Train sample", "X", "Y", True, "")
graph.add(
    plot_1d_data(x_test, y_test, legend="model", color="black", linestyle="dashed")
)
graph.add(
    plot_1d_data(x_train, y_train, type="Cloud", legend="train sample", color="red")
)
graph.setLegendPosition("upper right")
view = viewer.View(graph)

# %%
# Creation of the surrogate model
# -------------------------
#
# We use the :class:`~openturns.ConstantBasisFactory` class to define the trend and the
# :class:`~openturns.MaternModel` class to define the covariance model.
# In this example,, the smoothness parameter of the Matérn model is fixed to :math:`\nu=3/2` and
# we only estimate the scale and the amplitude parameters.
#
# Nevertheless, we could modify the list of the
# parameters that have to be estimated (the *active* parameters) and in particular we can add the
# estimation of :math:`\nu`: see the documentation of the method
# :meth:`~openturns.CovarianceModel.setActiveParameter` of
# the class :class:`~openturns.CovarianceModel` to get more details.
dimension = 1
basis = ot.ConstantBasisFactory(dimension).build()
covarianceModel = ot.MaternModel([1.0] * dimension, 1.5)

# %%
# The class :class:`~openturns.experimental.GaussianProcessFitter`  builds the Gaussian process :math:`Y` defined by:
#
# .. math::
#
#   Y(\omega, x) = \mu(x) + W(\omega, x)
#
# where:
#
# - :math:`\mu(x) = \sum_{j=1}^{b} \beta_j \varphi_j(x)` and :math:`\varphi_j: \Rset \rightarrow \Rset`
#   the trend function for :math:`1 \leq j \leq b`. Here the functional basis is reduced to the constant
#   function;
# - :math:`W` is a Gaussian process of dimension 1 with zero mean and a Matérn covariance model
#   which covariance function is denoted by :math:`C`.
#
# The coefficients of the trend function and the active covariance model parameters are estimated by
# maximizing the *reduced* log-likelihood of the model.
fitter_algo = otexp.GaussianProcessFitter(x_train, y_train, covarianceModel, basis)
fitter_algo.run()
fitter_result = fitter_algo.getResult()
print(fitter_result)

# %%
# We can draw the trend function.
trend_func = fitter_result.getMetaModel()
g_trend = trend_func.draw(xmin, xmax, 256)
g_trend.setTitle(r"Trend function of the Gaussian process $Y$")
g_trend.setXTitle(r"$x$")
g_trend.setYTitle(r"$\mu(x)$")
view = viewer.View(g_trend)

# %%
# The class :class:`~openturns.experimental.GaussianProcessRegression` is built from the  Gaussian process :math:`Y` and makes
# the  Gaussian process approximation :math:`\vect{Z}` interpolate the data set and is defined as:
#
# .. math::
#    :label: GPRdefEx
#
#    \vect{Z}(\omega, \vect{x}) = \vect{Y}(\omega, \vect{x})\, | \,  \cC
#
# where :math:`\cC` is the condition :math:`\vect{Y}(\omega, \vect{x}_k) = \vect{y}_k` for
# :math:`1 \leq k \leq \sampleSize`. The Gaussian process regression surrogate model is defined by the mean of :math:`\vect{Z}`:
#
# .. math::
#
#    \surrogateModel(\vect{x}) = \vect{\mu}(\vect{x}) + \sum_{i=1}^\sampleSize \gamma_i \mat{C}( \vect{x},  \vect{x}_i)
#
# where the :math:`\gamma_i` are called the *covariance coefficients* and :math:`C` the covariance # function of the Matérn
# covariance model.
gpr_algo = otexp.GaussianProcessRegression(fitter_result)
gpr_algo.run()
gpr_result = gpr_algo.getResult()
print(gpr_result)

# %%
# We observe that the `scale` and `amplitude` parameters have been optimized by the
# :meth:`~openturns.experimental.GaussianProcessFitter.run` method, while the :math:`\nu`
# parameter has remained unchanged.
# Then we get the surrogate model with
# :meth:`~openturns.experimental.GaussianProcessFitterResult.getMetaModel` and we
# evaluate the outputs of the surrogate model on the test
# design of experiments.

# %%
gprMetamodel = gpr_result.getMetaModel()
y_test_MM = gprMetamodel(x_test)


# %%
# Now we plot Gaussian process regression surrogate model, in addition to the previous plots.
graph = ot.Graph("Gaussian process regression surrogate model", "X", "Y", True, "")
graph.add(
    plot_1d_data(x_test, y_test, legend="model", color="black", linestyle="dashed")
)
graph.add(
    plot_1d_data(x_train, y_train, type="Cloud", legend="train sample", color="red")
)
graph.add(plot_1d_data(x_test, y_test_MM, legend="GPR", color="blue"))
graph.setLegendPosition("upper right")
view = viewer.View(graph)

# %%
# We observe that the Gaussian process regression surrogate model is interpolating. This is what is meant by
# *conditioning* a Gaussian process.
#
# We see that, when the sine function has a strong curvature between two points which are separated
# by a
# large distance (e.g. between :math:`x=4` and :math:`x=6`),
# then the Gaussian regression is not close to the function :math:`g`.
# However, when the training points are close (e.g. between :math:`x=11` and :math:`x=11.5`) or when the function is nearly
# linear (e.g. between :math:`x=8` and :math:`x=11`),
# then the Gaussian process regression is quite accurate.

# %%
# Compute confidence bounds
# -------------------------
#
# In order to assess the quality of the surrogate model, we can estimate the variance and compute a
# :math:`1-\alpha = 95\%` confidence interval associated with the conditioned Gaussian process.
#
# We denote by :math:`q_{p}` the quantile of order :math:`p` of the Gaussian distribution.
# Therefore, the confidence interval of level :math:`1-\alpha` is :math:`\left[q_{\alpha/2},q_{1-\alpha/2}\right]`.
alpha = 0.05


def computeQuantileAlpha(alpha):
    bilateralCI = ot.Normal().computeBilateralConfidenceInterval(1 - alpha)
    return bilateralCI.getUpperBound()[0]


quantileAlpha = computeQuantileAlpha(alpha)
print("alpha=%f" % (alpha))
print("Quantile alpha=%f" % (quantileAlpha))

# %%
# The Gaussian process regression computed on the sample :math:`(\xi_1, \dots, \xi_N)` is a Gaussian vector. It is possible to
# get the variance of each :math:`\vect{Z}_i(\omega) = \vect{Y}(\omega, \vect{\xi}_i)\, | \,  \cC` for :math:`1 \leq i \leq N`
# with
# the meth:`~openturns.experimental.GaussianProcessConditionalCovariance.getConditionalMarginalVariance` method. That method
# returns a point which is the sequence of the variances of each :math:`\vect{Z}_i(\omega)`.
# Since this is a variance, we use the square root in order to compute the
# standard deviation.
# However, some coefficients in the diagonal are very close to zero and
# nonpositive, which leads to an exception of the `sqrt` function.
# This is why we add an epsilon on the diagonal (nugget factor), which prevents this issue.
sqrt = ot.SymbolicFunction(["x"], ["sqrt(x)"])
epsilon = ot.Sample(n_test, [1.0e-8])
gccc = otexp.GaussianProcessConditionalCovariance(gpr_result)
conditionalVariance = gccc.getConditionalMarginalVariance(x_test) + epsilon
conditionalSigma = sqrt(conditionalVariance)

# %%
# The following figure presents the conditional standard deviation depending on :math:`x`.
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
# sphinx_gallery_thumbnail_number = 5
graph = ot.Graph(
    "Gaussian process regression surrogate model and confidence bounds", "X", "Y", True, ""
)

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

graph.add(
    plot_1d_data(x_test, y_test, legend="model", color="black", linestyle="dashed")
)
graph.add(plot_1d_data(x_train, y_train, type="Cloud", legend="Data", color="red"))
graph.add(plot_1d_data(x_test, y_test_MM, legend="GPR", color="blue"))
graph.setLegendPosition("upper right")
view = viewer.View(graph)

# %%
# We see that the confidence intervals are small in the regions where two
# consecutive training points are close to each other
# (e.g. between :math:`x=11` and :math:`x=11.5`) and large when the two points
# are not (e.g. between :math:`x=8.` and :math:`x=11`) or when the curvature
# of the function is large (between :math:`x=4` and :math:`x=6`).

# %%
# Display all figures
viewer.View.ShowAll()
