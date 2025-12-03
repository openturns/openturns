"""
Distribution of estimators in linear regression
===============================================
"""

# %%
# Introduction
# ~~~~~~~~~~~~
#
# In this example, we are interested in the distribution of several estimators in linear regression: the variance
# of the observation error, the standard deviation of the error, and the coefficients.
# We show how to use the :class:`~openturns.PythonRandomVector` class in order to
# perform a study of the sample distribution of these estimators.
#
# In the general linear regression model, the observation error :math:`\epsilon` has the
# normal distribution :math:`\cN(0, \sigma^2)` where :math:`\sigma > 0`
# is the standard deviation.
# We are interested in the estimators of the variance :math:`\sigma^2`,
# the standard deviation :math:`\sigma` and the coefficients :math:`\vect{a}`:
#
# - the variance of the residuals, :math:`\sigma^2`, is estimated from :meth:`~openturns.LinearModelResult.getResidualsVariance`
# - the standard deviation :math:`\sigma` is estimated from :meth:`~openturns.LinearModelAnalysis.getResidualsStandardError`
# - the coefficients :math:`\vect{a}` are estimated from :meth:`~openturns.LinearModelResult.getCoefficients`
#
# The asymptotic distribution of these estimators is known (see [vaart2000]_); :class:`~openturns.LinearModelAnalysis` provides
# :meth:`~openturns.LinearModelAnalysis.getVarianceDistribution` and :meth:`~openturns.LinearModelAnalysis.getCoefficientsDistribution`.
# But we want to perform an empirical study, based on simulation.
# In order to see the distribution of the estimator, we simulate an observation of the estimator,
# and repeat that experiment :math:`r \in \Nset` times, where :math:`r`
# is a large integer.
# Then we approximate the distribution using :class:`~openturns.KernelSmoothing`.


import openturns as ot
import openturns.viewer as otv
from matplotlib import pyplot as plt

# %%
# The simulation engine
# ~~~~~~~~~~~~~~~~~~~~~
# The simulation is based on the :class:`~openturns.PythonRandomVector` class,
# which simulates independent observations of a random vector.
# The `getRealization()` method implements the simulation of the observation
# of the estimator.
#


class SampleEstimatorLinearRegression(ot.PythonRandomVector):
    def __init__(
        self, sample_size, true_standard_deviation, coefficients, estimator="variance"
    ):
        """
        Create a RandomVector for an estimator from a linear regression model.

        Parameters
        ----------
        sample_size : int
            The sample size n.
        true_standard_deviation : float
            The standard deviation of the Gaussian observation error.
        coefficients : sequence of p floats
            The coefficients of the linear model.
        estimator : str
            The estimator.
            Available estimators are "variance", "standard-deviation" and "coefficient_i".
        """
        super(SampleEstimatorLinearRegression, self).__init__(1)
        self.sample_size = sample_size
        self.numberOfParameters = coefficients.getDimension()
        input_dimension = self.numberOfParameters - 1  # Because of the intercept
        centerCoefficient = [0.0] * input_dimension
        constantCoefficient = [coefficients[0]]
        linearCoefficient = ot.Matrix([coefficients[1:]])
        self.linearModel = ot.LinearFunction(
            centerCoefficient, constantCoefficient, linearCoefficient
        )
        self.distribution = ot.Normal(input_dimension)
        self.distribution.setDescription(["x%d" % (i) for i in range(input_dimension)])
        self.errorDistribution = ot.Normal(0.0, true_standard_deviation)
        self.estimator = estimator
        # In classical linear regression, the input is deterministic.
        # Here, we set it once and for all.
        self.input_sample = self.distribution.getSample(self.sample_size)
        self.output_sample = self.linearModel(self.input_sample)

    def getRegressionResult(self):
        errorSample = self.errorDistribution.getSample(self.sample_size)
        noisy_output_sample = self.output_sample + errorSample
        algo = ot.LinearModelAlgorithm(self.input_sample, noisy_output_sample)
        return algo.getResult()

    def getRealization(self):
        lmResult = self.getRegressionResult()
        if self.estimator == "variance":
            output = lmResult.getResidualsVariance()
        elif self.estimator == "standard-deviation":
            lmAnalysis = ot.LinearModelAnalysis(lmResult)
            output = lmAnalysis.getResidualsStandardError()
        elif self.estimator.startswith("coefficient"):
            index = int(self.estimator[-1])
            output = lmResult.getCoefficients()[index]
        else:
            raise ValueError("Unknown estimator %s" % (self.estimator))
        return [output]


def plot_sample_by_kernel_smoothing(
    sample_size, true_standard_deviation, coefficients, estimator, repetitions_size, true_value, description
):
    """
    Plot the estimated distribution of the biased sample variance from a sample

    The method uses Kernel Smoothing with default kernel.

    Parameters
    ----------
    repetitions_size : int
        The number of repetitions of the experiments.
        This is the (children) size of the sample of the biased
        sample variance

    Returns
    -------
    graph : ot.Graph
        The plot of the PDF of the estimated distribution.

    """
    pyRV = SampleEstimatorLinearRegression(sample_size, true_standard_deviation, coefficients, estimator)
    myRV = ot.RandomVector(pyRV)
    sample_estimator = myRV.getSample(repetitions_size)
    sample_estimator.setDescription([description])
    mean_sample_estimator = sample_estimator.computeMean()

    graph = ot.KernelSmoothing().build(sample_estimator).drawPDF()
    graph.setLegends(["Sample"])
    min_size = ot.ResourceMap.GetAsUnsignedInteger("LinearModelAnalysis-MinimumSampleSizeForAsymptoticDistributions")
    if sample_size >= min_size:
        # Add the asymptotic distribution
        bb = graph.getBoundingBox()
        xlb = bb.getLowerBound()[0]
        xub = bb.getUpperBound()[0]
        if estimator == "variance":
            lmResult = pyRV.getRegressionResult()
            lmAnalysis = ot.LinearModelAnalysis(lmResult)
            distribution = lmAnalysis.getVarianceDistribution()
            graph2 = distribution.drawMarginal1DPDF(0, xlb, xub, 100)
            graph2.setLegends(["Asymptotic"])
            graph.add(graph2)
        elif estimator.startswith("coefficient"):
            index = int(estimator[-1])
            lmResult = pyRV.getRegressionResult()
            lmAnalysis = ot.LinearModelAnalysis(lmResult)
            distribution = lmAnalysis.getCoefficientsDistribution()
            graph2 = distribution.drawMarginal1DPDF(index, xlb, xub, 100)
            graph2.setLegends(["Asymptotic"])
            graph.add(graph2)
    bb = graph.getBoundingBox()
    ylb = bb.getLowerBound()[1]
    yub = bb.getUpperBound()[1]
    curve = ot.Curve([true_value] * 2, [ylb, yub])
    curve.setLegend("Exact")
    curve.setLineWidth(2.0)
    graph.add(curve)
    graph.setTitle(
        "Size = %d, True = %.4f, Mean = %.4f"
        % (
            sample_size,
            true_value,
            mean_sample_estimator[0]
        )
    )
    return graph


# %%
# Distribution of the variance estimator
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# We first consider the estimation of the variance :math:`\sigma^2`.
# In the next cell, we consider a sample size equal to :math:`n = 6` with
# :math:`p = 3` parameters.
# We use :math:`r = 100` repetitions.


repetitions_size = 100
true_standard_deviation = 0.1
sample_size = 6
coefficients = ot.Point([3.0, 2.0, -1.0])
estimator = "variance"
description = r"$\hat{\sigma}^2$"
view = otv.View(
    plot_sample_by_kernel_smoothing(
        sample_size, true_standard_deviation, coefficients, estimator, repetitions_size, true_standard_deviation**2, description
    ),
    figure_kw={"figsize": (6.0, 3.5)},
)
plt.subplots_adjust(bottom=0.2)

# %%
# If we use a sample size equal to :math:`n = 6` with
# :math:`p = 3` parameters, the distribution is not symmetric.
# The mean of the observations of the sample variances remains close to
# the true value :math:`0.1^2 = 0.01`.

# %%
# Then we increase the sample size :math:`n`.

repetitions_size = 100
true_standard_deviation = 0.1
sample_size = 100
coefficients = ot.Point([3.0, 2.0, -1.0])
estimator = "variance"
description = r"$\hat{\sigma}^2$"
view = otv.View(
    plot_sample_by_kernel_smoothing(
        sample_size, true_standard_deviation, coefficients, estimator, repetitions_size, true_standard_deviation**2, description
    ),
    figure_kw={"figsize": (6.0, 3.5)},
)
plt.subplots_adjust(bottom=0.2)


# %%
# If we use a sample size equal to :math:`n = 100` with
# :math:`p = 3` parameters, the distribution is almost symmetric and
# almost normal.

# %%
# Distribution of the standard deviation estimator
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# We now consider the estimation of the standard deviation :math:`\sigma`.


repetitions_size = 100
true_standard_deviation = 0.1
sample_size = 6
coefficients = ot.Point([3.0, 2.0, -1.0])
estimator = "standard-deviation"
description = r"$\hat{\sigma}$"
view = otv.View(
    plot_sample_by_kernel_smoothing(
        sample_size, true_standard_deviation, coefficients, estimator, repetitions_size, true_standard_deviation, description
    ),
    figure_kw={"figsize": (6.0, 3.5)},
)
plt.subplots_adjust(bottom=0.2)


# %%
# If we use a sample size equal to :math:`n = 6` with
# :math:`p = 3` parameters, we see that the distribution is almost symmetric.
# We notice a slight bias, since the mean of the observations of the
# standard deviation is not as close to the true value 0.1
# as we could expect.


repetitions_size = 100
true_standard_deviation = 0.1
sample_size = 100
coefficients = ot.Point([3.0, 2.0, -1.0])
estimator = "standard-deviation"
description = r"$\hat{\sigma}$"
view = otv.View(
    plot_sample_by_kernel_smoothing(
        sample_size, true_standard_deviation, coefficients, estimator, repetitions_size, true_standard_deviation, description
    ),
    figure_kw={"figsize": (6.0, 3.5)},
)
plt.subplots_adjust(bottom=0.2)


# %%
# If we use a sample size equal to :math:`n = 100` with
# :math:`p = 3` parameters, we see that the distribution is almost normal.
# We notice that the bias disappeared.


# %%
# Distribution of the coefficients
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# We now consider the estimation of the coefficients :math:`\vect{\hat{a}}`.

repetitions_size = 100
true_standard_deviation = 0.1
sample_size = 100
coefficients = ot.Point([3.0, 2.0, -1.0])
coefficients_dimension = coefficients.getDimension()
grid = ot.GridLayout(1, coefficients_dimension)
for i in range(coefficients_dimension):
    estimator = "coefficient_%d" % i
    description = r"$\hat{a}_%d$" % i
    graph = plot_sample_by_kernel_smoothing(sample_size, true_standard_deviation, coefficients, estimator, repetitions_size, coefficients[i], description)
    if i > 0:
        graph.setYTitle("")
    grid.setGraph(0, i, graph)
view = otv.View(
    grid,
    figure_kw={"figsize": (14.0, 3.5)}
)
plt.subplots_adjust(left=0.04, right=0.98, bottom=0.2)

# %%
otv.View.ShowAll()
