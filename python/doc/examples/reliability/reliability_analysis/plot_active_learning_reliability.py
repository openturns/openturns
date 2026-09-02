"""
Active learning algorithms for reliability analysis
===================================================
"""

# %%
#
# The objective is to illustrate the behavior of active learning algorithms in the context of reliability analysis.
# These algorithms couple a simulation algorithm (e.g., Monte-Carlo simulation, Importance Sampling, Subset Sampling), a Gaussian Process
# and an active learning criterion to enrich the metamodel in a goal-oriented approach to efficiently estimate a probability of failure.
# For more details, please consult :class:`~openturns.ActiveLearningReliabilityAlgorithm`.
# In this example, two cases are described.
# Firstly, :class:`~openturns.ProbabilitySimulationAlgorithm` using :class:`~openturns.MonteCarloExperiment` is combined with :class:`~openturns.ActiveLearningUFunction`.
# Secondly, :class:`~openturns.SubsetSampling` is associated with :class:`~openturns.ActiveLearningGMMFunction`.
#
# We consider the four-branch function :math:`g : \mathbb{R}^2 \rightarrow \mathbb{R}` defined by:
#
# .. math::
#   \begin{align*}
#   g(\vect{x}) = \min \begin{pmatrix}3+0.1(x_1-x_2)^2-\frac{(x_1+x_2)}{\sqrt{2}}\\
#   3+0.1(x_1-x_2)^2+\frac{(x_1+x_2)}{\sqrt{2}}\\
#   (x_1-x_2)+ \frac{6}{\sqrt{2}}\\
#   (x_2-x_1)+ \frac{6}{\sqrt{2}}
#   \end{pmatrix}
#   \end{align*}
#
# and the input random vector :math:`\vect{X} = (X_1, X_2)` which follows the 2-dimensional Normal distribution:
#
# .. math::
#   \begin{align*}
#   \vect{X} \sim  \mathcal{N}(\mu = [0, 0], \sigma = [1.5,1.5], corr = \mat{I}_2)
#   \end{align*}
#
# We want to evaluate the probability:
#
# .. math::
#   \begin{align*}
#   p = \mathbb{P} ( g(\vect{X}) \leq 0 )
#   \end{align*}
#

# %%
# First, import the python modules:

# %%
import openturns as ot
import openturns.viewer as otv
import math

# %%
# Create the probabilistic model :math:`Y = g(\vect{X})`
# ------------------------------------------------------

# %%
# Create the input random vector :math:`\vect{X}`:

# %%
input_distribution = ot.Normal([0.0, 0.0], [1.5, 1.5])
X = ot.RandomVector(input_distribution)
input_dimension = input_distribution.getDimension()

# %%
# Create the function :math:`g` from a :class:`~openturns.PythonFunction`:

# %%


def fourBranch(x):
    x1 = x[0]
    x2 = x[1]

    g1 = 3 + 0.1 * (x1 - x2) ** 2 - (x1 + x2) / math.sqrt(2)
    g2 = 3 + 0.1 * (x1 - x2) ** 2 + (x1 + x2) / math.sqrt(2)
    g3 = (x1 - x2) + 6 / math.sqrt(2)
    g4 = (x2 - x1) + 6 / math.sqrt(2)

    return [min((g1, g2, g3, g4))]


g = ot.PythonFunction(input_dimension, 1, fourBranch)

# %%
# Draw the function :math:`g` to understand the shape of the limit state function:

# %%
graph = ot.Graph("Four Branch function", "x1", "x2")
graph.setAxes(True)
graph.setGrid(True)
graph.setLegendPosition("upper right")
drawfunction = g.draw(
    [-5] * input_dimension, [5] * input_dimension, [100] * input_dimension
)
graph.add(drawfunction)
view = otv.View(graph)

# %%
# Create the output random vector :math:`Y = g(\vect{X})`:

# %%
Y = ot.CompositeRandomVector(g, X)

# %%
# Create the event :math:`\{ Y = g(\vect{X}) \leq 0 \}`
# -----------------------------------------------------

# %%
threshold = 0.0
event = ot.ThresholdEvent(Y, ot.Less(), threshold)

# %%
# Evaluate the probability with various active learning algorithms
# ----------------------------------------------------------------
# To set an active learning algorithm, we must define its core components: the selected simulation algorithm, the Gaussian Process fitter, and the active learning criterion.

# %%
# Definition of Gaussian Process fitter
# -------------------------------------

# %%
# Creation of DoE with a limited size (number_samples_DoE = 10) and evaluation of the exact limit state function.
# The definition domain for the DoE (through the lower and upper bounds) is extended in order to get samples in rare event regions.
# It can be also defined by a combination of mean and standard deviation of the input distribution (for example, :math:`\mu \pm 4 \sigma`).

# %%
lower_bound_DoE = -5.0
upper_bound_DoE = 5.0
number_samples_DoE = 10
distribution_LHS = ot.JointDistribution(
    [ot.Uniform(lower_bound_DoE, upper_bound_DoE)] * input_dimension
)
lhs = ot.LHSExperiment(distribution_LHS, number_samples_DoE)
input_DoE = lhs.generate()
output_DoE = g(input_DoE)

# %%
# Creation of Gaussian Process Fitter

# %%
basis = ot.ConstantBasisFactory(input_dimension).build()
covariance_model = ot.MaternModel(input_dimension)
fitter = ot.GaussianProcessFitter(input_DoE, output_DoE, covariance_model, basis)

# %%
# First example: Monte-Carlo simulation and "U" function for active learning criterion
# ------------------------------------------------------------------------------------

# %%
# Definition of Monte-Carlo algorithm.
# The settings of the Monte-Carlo algorithm can have a large impact on the results of the active learning algorithm.
# These settings should be carefully chosen.
Monte_Carlo_experiment = ot.MonteCarloExperiment()
Monte_Carlo_algorithm = ot.ProbabilitySimulationAlgorithm(event, Monte_Carlo_experiment)
Monte_Carlo_algorithm.setMaximumCoefficientOfVariation(0.01)
Monte_Carlo_algorithm.setMaximumOuterSampling(5000)

# %%
# Definition of active learning function. Here, we choose the "U" function. This function takes two arguments. The first one is the event threshold.
# The second one is used to assess the convergence of active learning if the Convergence Criterion of :class:`~openturns.ActiveLearningReliabilityAlgorithm` is set to `ACTIVE_LEARNING`.
# Here, we choose that the algorithm will stop if the "U" function evaluations for all the samples of the simulation algorithm are greater than 2.
active_learning_convergence_threshold = 2.0
u_function = ot.ActiveLearningUFunction(
    event.getThreshold(), active_learning_convergence_threshold
)

# %%
# Now, we can build the active learning algorithm. We define the maximal number of exact limit state function evaluations to 75.
simulation_budget = 75
active_learning_MonteCarlo = ot.ActiveLearningReliabilityAlgorithm(
    fitter, Monte_Carlo_algorithm, u_function, simulation_budget
)

# %%
# We need to select the stopping criterion of active learning algorithm.
# Here we choose to use a criterion based on the active learning function (with the settings given at the :class:`~openturns.ActiveLearningUFunction` instantiation).
convergence_criterion = active_learning_MonteCarlo.ACTIVE_LEARNING
active_learning_MonteCarlo.setConvergenceCriterion(convergence_criterion)

# %%
# Now, we can run the algorithm and get results
active_learning_MonteCarlo.run()
results_active_MonteCarlo = active_learning_MonteCarlo.getResult()

# %%
# Probability estimate
print("Probability estimate:", results_active_MonteCarlo.getProbabilityEstimate())

# %%
# Confidence interval of probability estimation due to Gaussian Process uncertainty
print(
    "Confidence interval:", results_active_MonteCarlo.getProbabilityConfidenceInterval()
)

# %%
# Number of function calls during active learning process
print(
    "Number of function calls during active learning:",
    results_active_MonteCarlo.getFunctionCallNumber(),
)

# %%
# Visualization of infill samples. We can draw the metamodel of the function and final DoE.
# We use the Gaussian process regressor refined with the active learning algorithm. We plot both the initial and enriched DoEs.
# We also plot the contours of the limit state approximation with the metamodel.

# %%
gprResult = results_active_MonteCarlo.getGprResult()
metamodel = gprResult.getMetaModel()
final_input_DoE = results_active_MonteCarlo.getGprResult().getInputSample()
final_output_DoE = results_active_MonteCarlo.getGprResult().getOutputSample()
graph = ot.Graph("Final Gaussian Process metamodel", "x1", "x2")
graph.setAxes(True)
graph.setGrid(True)
graph.setLegendPosition("upper right")
drawfunction = metamodel.draw(
    [-5] * input_dimension, [5] * input_dimension, [100] * input_dimension
)
drawfunction.setTitle("Metamodel after active learning")
drawfunction.setXTitle("x1")
drawfunction.setYTitle("x2")
cloud1 = ot.Cloud(final_input_DoE, "final DoE")
cloud2 = ot.Cloud(input_DoE, "initial DoE")
graph.add(drawfunction)
graph.add(cloud1)
graph.add(cloud2)
view = otv.View(graph)
# sphinx_gallery_thumbnail_number = 2

# %%
# We can see that as the active learning is goal-oriented, the metamodel is accurate only in the vicinity of the event threshold.

# %%
# We can compare the metamodel before and after refinement.
fitter.run()
gpr_initial = ot.GaussianProcessRegression(fitter.getResult())
gpr_initial.run()
metamodel_initial = gpr_initial.getResult().getMetaModel()

drawfunction_initial = metamodel_initial.draw(
    [-5] * input_dimension, [5] * input_dimension, [100] * input_dimension
)
drawfunction_initial.setTitle("Metamodel before active learning")
drawfunction_initial.setXTitle("x1")
drawfunction_initial.setYTitle("x2")
grid = ot.GridLayout(1, 2)
grid.setGraph(0, 1, drawfunction)
grid.setGraph(0, 0, drawfunction_initial)
_ = otv.View(grid)


# %%
# We can  plot the probability history along the active learning process iterations.
graph_history = ot.Graph("Probability history", "iterations", "Probability estimate")
graph_history.setGrid(True)
probability_history = results_active_MonteCarlo.getProbabilityHistory()
size_history = len(probability_history)
graph_history.add(ot.Curve(range(size_history), probability_history))
_ = otv.View(graph_history)

# %%
# We can also plot the limit state provided by the true function and compare with the one estimated by the refined metamodel.
graph = ot.Graph("Limit states", "x1", "x2")
g_IsoLines = g.draw(
    [-5] * input_dimension, [5] * input_dimension, [128] * input_dimension
)
dr = g_IsoLines.getDrawable(0)
dr.setLevels([threshold])
dr.setLegend("Limit state - true function")
dr.setColor("blue")
dr.setLineWidth(1)

metamodel_initial_IsoLines = metamodel_initial.draw(
    [-5] * input_dimension, [5] * input_dimension, [128] * input_dimension
)
dr_metamodel_initial = metamodel_initial_IsoLines.getDrawable(0)
dr_metamodel_initial.setLevels([threshold])
dr_metamodel_initial.setLineStyle("dashed")
dr_metamodel_initial.setLegend("Limit state - metamodel before active learning")
dr_metamodel_initial.setColor("orange")
dr_metamodel_initial.setLineWidth(1)

metamodel_IsoLines = metamodel.draw(
    [-5] * input_dimension, [5] * input_dimension, [128] * input_dimension
)
dr_metamodel = metamodel_IsoLines.getDrawable(0)
dr_metamodel.setLevels([threshold])
dr_metamodel.setLineStyle("dashed")
dr_metamodel.setLegend(
    "Limit state - metamodel obtained with Monte-Carlo and U function"
)
dr_metamodel.setColor("red")
dr_metamodel.setLineWidth(1)

graph.add(dr)
graph.add(dr_metamodel_initial)
graph.add(dr_metamodel)
graph.setLegendCorner([-0.05, -0.2])
graph.setLegendPosition("upper left")
_ = otv.View(graph)


# %%
# Second example: Subset Sampling simulation and "Generalized Max Min" function for active learning criterion
# ------------------------------------------------------------------------------------------------------------

# %%
# Definition of Subset sampling with default parameters
subset_algorithm = ot.SubsetSampling(event)

# %%
# Definition of active learning function. Here, we choose the "generalized max min" function
active_learning_convergence_threshold = 1e-10
gmm_output_quantile = 0.1
gmm_function = ot.ActiveLearningGMMFunction(
    threshold,
    active_learning_convergence_threshold,
    input_distribution,
    gmm_output_quantile,
)

# %%
# Now, we can build the active learning algorithm
active_learning_subset = ot.ActiveLearningReliabilityAlgorithm(
    fitter, subset_algorithm, gmm_function, simulation_budget
)

# %%
# Here we choose to stop the algorithm based on the probability uncertainty estimation due to the Gaussian Process Regressor model.
# We stop the algorithm if the difference between
# the probability estimated using  :math:`\hat{g}(\vect{X}) + k \widehat{\sigma}(\vect{X})` and the probability estimated using
# :math:`\hat{g}(\vect{X}) - k \widehat{\sigma}(\vect{X})` is below a given threshold.
# Mathematically, the stopping criterion is given by
#
# .. math::
#   \begin{align*}
#   \frac{|\Prob{\hat{g}(\vect{X}) + k \hat{\sigma}(\vect{X})\leq 0} - \Prob{\hat{g}(\vect{X}) - k \hat{\sigma}(\vect{X})\leq 0}|}{\Prob{\hat{g}(\vect{X})\leq 0}} \leq \epsilon
#   \end{align*}
#
# In this example, we choose :math:`k = 2` and :math:`\epsilon = 0.2`.
convergence_criterion = active_learning_subset.PROBABILITY_UNCERTAINTY
active_learning_subset.setConvergenceUncertaintyFactor(2.0)
active_learning_subset.setConvergenceCriterionThreshold(0.2)
active_learning_subset.setConvergenceCriterion(convergence_criterion)

# %%
# Now we can run the algorithm and get results
active_learning_subset.run()
results_active_subset = active_learning_subset.getResult()

# %%
# Probability estimate
print("Probability estimate:", results_active_subset.getProbabilityEstimate())

# %%
# Confidence interval of probability estimation due to Gaussian Process uncertainty
print("Confidence interval:", results_active_subset.getProbabilityConfidenceInterval())

# %%
# Number of function calls
print("Number of function calls:", results_active_subset.getFunctionCallNumber())

# %%
# Visualization of infill samples.
# We use the Gaussian process regressor refined with the active learning algorithm. We plot both the initial and enriched DoEs.
# We also plot the contours of the limit state approximation with the metamodel.
# We can see that the infilled samples using the GMM are more spread out that with the "U"-function.
# This is due to the fact that the GMM function takes the distance between the currrent DoE samples into account.
gprResult_subset = results_active_subset.getGprResult()

metamodel_subset = gprResult_subset.getMetaModel()
final_input_DoE_subset = results_active_subset.getGprResult().getInputSample()

graph = ot.Graph("Final Gaussian Process metamodels", "x1", "x2")
graph.setAxes(True)
graph.setGrid(True)
graph.setLegendPosition("upper right")
drawfunction = metamodel_subset.draw(
    [-5] * input_dimension, [5] * input_dimension, [100] * input_dimension
)
cloud1 = ot.Cloud(final_input_DoE_subset, "final DoE")
cloud2 = ot.Cloud(input_DoE, "initial DoE")
graph.add(drawfunction)
graph.add(cloud1)
graph.add(cloud2)
view = otv.View(graph)
otv.View.ShowAll()

# %%
# We can also plot the limit state provided by the true function and compare with the one estimated by the new refined metamodel.
graph = ot.Graph("Limit states", "x1", "x2")
levels = [0.0]
graph.add(dr)
graph.add(dr_metamodel_initial)
graph.add(dr_metamodel)
metamodel_IsoLines_subset = metamodel_subset.draw(
    [-5] * input_dimension, [5] * input_dimension, [128] * input_dimension
)
dr_metamodel_subset = metamodel_IsoLines_subset.getDrawable(0)
dr_metamodel_subset.setLevels([threshold])
dr_metamodel_subset.setLineStyle("dashed")
dr_metamodel_subset.setLegend(
    "Limit state - metamodel obtained with Subset sampling and GMM"
)
dr_metamodel_subset.setColor("green")
dr_metamodel_subset.setLineWidth(1)
graph.add(dr_metamodel_subset)
graph.setLegendCorner([-0.05, -0.2])
graph.setLegendPosition("upper left")
_ = otv.View(graph)

otv.View.ShowAll()
