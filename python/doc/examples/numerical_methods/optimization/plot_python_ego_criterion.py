"""
Custom enrichment criterion for efficient global optimization
=============================================================

This example shows how to define a custom enrichment criterion in Python and
use it with the
:class:`~openturns.experimental.EfficientGlobalOptimization` algorithm.

The algorithm keeps the whole enrichment loop: it grows the design of
experiments, refits the Gaussian process, maximizes the objective returned by
the criterion with its inner solver, evaluates the true model at the new point
and applies its stopping rules. A criterion only answers two questions, at each
iteration:

- which scalar function must the inner solver maximize?
- does the criterion value reached at the new point end the enrichment?

The built-in criterion is the expected improvement,
:class:`~openturns.experimental.ExpectedImprovement`, which balances the
predicted value and the predictive deviation. The custom criterion implemented
here is a *lower confidence bound improvement*: it targets an optimistic
estimate of the objective, the predicted value lowered by a multiple of the
predictive deviation, and rewards the points where that estimate beats the best
value observed so far.
"""

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

# Reproducible runs
ot.RandomGenerator.SetSeed(0)
ot.TBB.Disable()

# %%
# The optimization problem
# ------------------------
#
# A one-dimensional function with two wells, on a bounded domain.

# %%
model = ot.SymbolicFunction(["x"], ["(x - 0.3)^2 + 0.4 * sin(6 * x)"])
bounds = ot.Interval([0.0], [2.0])
problem = ot.OptimizationProblem()
problem.setObjective(model)
problem.setBounds(bounds)
dimension = problem.getDimension()

# %%
# The initial design of experiments and its Gaussian process.

# %%
experiment = ot.LHSExperiment(ot.Uniform(0.0, 2.0), 4)
inputSample = experiment.generate()
outputSample = model(inputSample)
covarianceModel = ot.SquaredExponential([0.5], [1.0])
basis = ot.ConstantBasisFactory(dimension).build()
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.run()
gpr = ot.GaussianProcessRegression(fitter.getResult())
gpr.run()

# %%
# The custom criterion
# --------------------
#
# Subclassing :class:`~openturns.experimental.PythonEfficientGlobalOptimizationCriterion`
# requires one method, `buildObjective`. The current state of the loop is
# available through the accessors of the base class: the Gaussian process of the
# current iteration, the current design, the best value observed so far, the
# direction of the problem and the noise function.
#
# The algorithm always maximizes the objective returned by the criterion, while
# the problem is stated as a minimization, so the objective below is an
# improvement: how much the optimistic estimate of the objective beats the best
# value observed so far. It is nonnegative, and zero means that no point of the
# domain is expected to do better than the current design.

# %%


class LowerConfidenceBoundCriterion(otexp.PythonEfficientGlobalOptimizationCriterion):
    def __init__(self, exploration=2.0, tolerance=1e-3):
        super().__init__()
        self.exploration = float(exploration)
        self.tolerance = float(tolerance)

    def buildObjective(self):
        covariance = ot.GaussianProcessConditionalCovariance(
            self.getGaussianProcessRegression()
        )
        best = self.getOptimalValue()
        # +1 in a minimization, -1 in a maximization: the formula below works in
        # the "smaller is better" scale, whatever the direction of the problem
        direction = 1.0 if self.getMinimization() else -1.0
        exploration = self.exploration

        def lowerConfidenceBoundImprovement(x):
            mean = covariance.getConditionalMean(x)[0]
            deviation = covariance.getConditionalMarginalVariance(x) ** 0.5
            optimistic = mean - direction * exploration * deviation
            return [max(0.0, direction * (best - optimistic))]

        return ot.PythonFunction(1, 1, lowerConfidenceBoundImprovement)

    def checkConvergence(self, criterionValue):
        # the objective is nonnegative: a negligible maximum ends the enrichment
        return criterionValue < self.tolerance


# %%
# The criterion is handed to the algorithm through its handle::

# %%
algo = otexp.EfficientGlobalOptimization(problem, gpr.getResult())
algo.setMaximumCallsNumber(10)
algo.setCriterion(
    otexp.EfficientGlobalOptimizationCriterion(LowerConfidenceBoundCriterion())
)

# %%
# The enrichment stops as soon as the criterion declares convergence, which
# `getHasConverged` reports, and the value of the criterion at each iteration is
# available:

# %%
algo.run()
result = algo.getResult()
algo.getHasConverged()

# %%
algo.getCriterionValues()

# %%
result.getOptimalPoint()

# %%
# The design really followed the custom criterion: the points it added are the
# ones that the lower confidence bound selected, not the ones the expected
# improvement would have selected.

# %%
inputHistory = result.getInputSample()
graph = model.draw([0.0], [2.0], [200])
graph.setTitle("Lower confidence bound enrichment")
# the cloud carries the abscissa and the observed value as two columns
cloud = ot.Cloud(inputSample, model(inputSample), "initial design")
cloud.setPointStyle("bullet")
cloud.setColor("black")
graph.add(cloud)
cloud = ot.Cloud(inputHistory, model(inputHistory), "added points")
cloud.setPointStyle("diamond")
cloud.setColor("forestgreen")
graph.add(cloud)
view = otv.View(graph)

# %%
otv.View.ShowAll()

# %%
ot.TBB.Enable()
