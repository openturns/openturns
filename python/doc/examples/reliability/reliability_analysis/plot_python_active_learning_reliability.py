"""
Custom active learning criterion for reliability analysis
==========================================================

This example shows how to define a custom active learning criterion in Python
and use it with the
:class:`~openturns.experimental.ActiveLearningReliabilityAlgorithm`.
The criterion used here is the least improvement function (LIF) proposed by
Sun et al. [sun2017]_.

The reliability problem is the four-branch function already used in the
:doc:`active learning gallery example <plot_active_learning_reliability>`,
but revisited as a union event. Using the same model, input distribution
and initial design of experiments makes it easy to compare the custom
criterion with the built-in :class:`~openturns.experimental.ActiveLearningUFunction`.
"""

# %%
import math

import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

# %%
# The four-branch reliability problem
# -----------------------------------
#
# The four-branch function :math:`g : \mathbb{R}^2 \rightarrow \mathbb{R}` is defined by:
#
# .. math::
#   :nowrap:
#
#   \begin{align*}
#   g(\vect{x}) = \min \begin{pmatrix}3+0.1(x_1-x_2)^2-\frac{(x_1+x_2)}{\sqrt{2}}\\
#   3+0.1(x_1-x_2)^2+\frac{(x_1+x_2)}{\sqrt{2}}\\
#   (x_1-x_2)+ \frac{6}{\sqrt{2}}\\
#   (x_2-x_1)+ \frac{6}{\sqrt{2}}
#   \end{pmatrix}
#   \end{align*}
#
# with the input random vector :math:`\vect{X} = (X_1, X_2)` which follows the 2-dimensional Normal distribution:
#
# .. math::
#   :nowrap:
#
#   \begin{align*}
#   \vect{X} \sim  \mathcal{N}(\mu = [0, 0], \sigma = [1.5,1.5], corr = \mat{I}_2)
#   \end{align*}
#
# The probability to estimate is defined by:
#
# .. math::
#   :nowrap:
#
#   \begin{align*}
#   p = \mathbb{P} ( g(\vect{X}) \leq 0 )
#   \end{align*}
#
# Here, we revisit the four-branch example as a union of four events, one for each branch.
# The four branch function for reliability is then defined as a composed event of the
# union events, each them defined by a single branch function.


# %%
def firstBranch(x):
    x1 = x[0]
    x2 = x[1]
    sqrt2 = math.sqrt(2.0)
    g1 = 3 + 0.1 * (x1 - x2) ** 2 - (x1 + x2) / sqrt2
    return [g1]


def secondBranch(x):
    x1 = x[0]
    x2 = x[1]
    sqrt2 = math.sqrt(2.0)
    g2 = 3 + 0.1 * (x1 - x2) ** 2 + (x1 + x2) / sqrt2
    return [g2]


def thirdBranch(x):
    x1 = x[0]
    x2 = x[1]
    sqrt2 = math.sqrt(2.0)
    g3 = (x1 - x2) + 6 / sqrt2
    return [g3]


def fourthBranch(x):
    x1 = x[0]
    x2 = x[1]
    sqrt2 = math.sqrt(2.0)
    g4 = (x2 - x1) + 6 / sqrt2
    return [g4]

input_distribution = ot.Normal([0.0, 0.0], [1.5, 1.5])
input_dimension = input_distribution.getDimension()
g1 = ot.PythonFunction(input_dimension, 1, firstBranch)
g2 = ot.PythonFunction(input_dimension, 1, secondBranch)
g3 = ot.PythonFunction(input_dimension, 1, thirdBranch)
g4 = ot.PythonFunction(input_dimension, 1, fourthBranch)

# %%
X = ot.RandomVector(input_distribution)
Y1 = ot.CompositeRandomVector(g1, X)
Y2 = ot.CompositeRandomVector(g2, X)
Y3 = ot.CompositeRandomVector(g3, X)
Y4 = ot.CompositeRandomVector(g4, X)

e1 = ot.ThresholdEvent(Y1, ot.Less(), 0.0)
e2 = ot.ThresholdEvent(Y2, ot.Less(), 0.0)
e3 = ot.ThresholdEvent(Y3, ot.Less(), 0.0)
e4 = ot.ThresholdEvent(Y4, ot.Less(), 0.0)

event = ot.UnionEvent([e1, e2, e3, e4]).asComposedEvent()
g = event.getFunction()

# %%
# Initial design of experiments and Gaussian process fitter
# ---------------------------------------------------------
#
# A latin hypercube sample of size 10 is used to fit the initial Gaussian
# process. The covariance model and the trend are the same as in the reference
# active learning example.

# %%
input_DoE = ot.LHSExperiment(
    ot.JointDistribution([ot.Uniform(-5.0, 5.0)] * input_dimension), 10
).generate()
output_DoE = g(input_DoE)
covariance_model = ot.MaternModel(input_dimension)
basis = ot.ConstantBasisFactory(input_dimension).build()
fitter = ot.GaussianProcessFitter(input_DoE, output_DoE, covariance_model, basis)

# %%
# The least improvement function
# ------------------------------
#
# The LIF criterion combines the probability of misclassifying the sign of the
# limit state with the probability density of the input vector:
#
# .. math::
#
#    \mathrm{LIF}(x) = \Phi(-U(x)) \, f_X(x),
#    \qquad
#    U(x) = \frac{|\mu(x) - t|}{\sigma(x)},
#
# where :math:`\mu(x)` and :math:`\sigma(x)` are the conditional mean and
# standard deviation of the Gaussian process, :math:`t` is the event threshold
# and :math:`f_X` is the joint probability density function of the inputs.
#
# The first factor :math:`\Phi(-U(x))` is the probability that the Gaussian
# process predicts the wrong side of the threshold. The second factor
# :math:`f_X(x)` favors points that are likely to be sampled. The point that
# maximizes the LIF is added to the design of experiments.
#
# The class below implements this criterion by subclassing
# :class:`~openturns.experimental.PythonActiveLearningReliabilityFunction`.
# Three methods are required:
#
# - ``computeAsScalar`` evaluates the criterion at a given point;
# - ``checkConvergenceLearning`` decides whether the enrichment can stop;
# - ``isMaximization`` gives the direction of the criterion.
#
# Here, the enrichment stops when the largest LIF value over the candidate
# sample falls below a small threshold. As LIF values are probability
# densities, this threshold is problem dependent; the value used below was
# calibrated on this example. The convergence of the active learning algorithm
# can also be monitored with its ``PROBABILITY_UNCERTAINTY`` criterion.


# %%
class LIFCriterion(otexp.PythonActiveLearningReliabilityFunction):
    def __init__(self, reliability_threshold, learning_threshold, distribution):
        super().__init__(reliability_threshold, learning_threshold)
        self._distribution = distribution

    def computeAsScalar(self, x):
        covariance = ot.GaussianProcessConditionalCovariance(
            self.getGaussianProcessRegression()
        )
        mean = covariance.getConditionalMean(x)[0]
        variance = covariance.getConditionalMarginalVariance(x)
        if not variance > 0.0:
            return 0.0
        u = abs(mean - self.getReliabilityThreshold()) / math.sqrt(variance)
        misclassification_probability = 0.5 * math.erfc(u / math.sqrt(2.0))
        return misclassification_probability * self._distribution.computePDF(x)

    def checkConvergenceLearning(self, values):
        return values.getMax()[0] <= self.getLearningThreshold()

    def isMaximization(self):
        return True


# %%
# The Python object is wrapped into an
# :class:`~openturns.experimental.ActiveLearningReliabilityFunction` so that it
# can be passed to the active learning algorithm.
learning_threshold = 1.0e-3
lif_criterion = otexp.ActiveLearningReliabilityFunction(
    LIFCriterion(0.0, learning_threshold, input_distribution)
)

# %%
# Active learning with the custom criterion
# -----------------------------------------
#
# The simulation algorithm is a Monte Carlo simulation. Its input sample is the
# candidate pool used by the criterion at each iteration.

# %%
monte_carlo = ot.ProbabilitySimulationAlgorithm(event, ot.MonteCarloExperiment())
monte_carlo.setMaximumCoefficientOfVariation(0.01)
monte_carlo.setMaximumOuterSampling(5000)

active_learning = otexp.ActiveLearningReliabilityAlgorithm(
    fitter, monte_carlo, lif_criterion
)
active_learning.setMaximumIterations(80)
active_learning.setConvergenceCriterion(active_learning.ACTIVE_LEARNING)
ot.RandomGenerator.SetSeed(0)
active_learning.run()
result = active_learning.getResult()

# %%
print(
    f"LIF active learning probability estimate: {result.getProbabilityEstimate():.4f}"
)
print(f"Number of limit state function calls: {result.getFunctionCallNumber()}")
print(f"Converged: {result.getHasConverged()}")

# %%
# A direct Monte Carlo simulation with a large sample gives a reference value
# for this problem.

# %%
ot.RandomGenerator.SetSeed(12345)
reference = ot.ProbabilitySimulationAlgorithm(event, ot.MonteCarloExperiment(100000))
reference.run()
reference_probability = reference.getResult().getProbabilityEstimate()
print(f"Reference Monte Carlo probability estimate: {reference_probability:.4f}")

# %%
# The probability estimate is also exposed at each enrichment iteration.
graph_history = ot.Graph("Probability history", "iteration", "probability estimate")
graph_history.setGrid(True)
probability_history = result.getProbabilityHistory()
graph_history.add(ot.Curve(range(len(probability_history)), probability_history))
_ = otv.View(graph_history)

# %%
# Visualization of the final metamodel
# ------------------------------------
#
# The final Gaussian process is accurate near the limit state, where the
# enrichment points have been added.

# %%
metamodel = result.getGprResult().getMetaModel()
final_input_DoE = result.getGprResult().getInputSample()
graph = ot.Graph("Final Gaussian process metamodel", "x1", "x2")
graph.setAxes(True)
graph.setGrid(True)
graph.setLegendPosition("upper right")
graph.add(
    metamodel.draw(
        [-5.0] * input_dimension, [5.0] * input_dimension, [100] * input_dimension
    )
)
graph.add(ot.Cloud(final_input_DoE, "enriched DoE"))
graph.add(ot.Cloud(input_DoE, "initial DoE"))
_ = otv.View(graph)
# sphinx_gallery_thumbnail_number = 2

# %%
# The enriched design of experiments concentrates around the four branches of
# the limit state, where the misclassification probability is the largest,
# while the density factor avoids spending points in low-probability regions.
otv.View.ShowAll()
