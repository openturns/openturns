"""
Optimize a failure-prone model with penalization
=================================================
"""

# %%
# In this example we optimize a model which may fail to evaluate,
# e.g. a simulation which raises an exception outside its validity domain.
# This penalization handles simulations that raise exceptions, such as RuntimeError.
# It cannot recover from process-level crashes (segmentation fault, abort, ...).
# Without precaution the optimizer stops on the first failure.
# The :class:`~openturns.experimental.PenalizedEvaluation` and
# :class:`~openturns.experimental.PenalizedProblem` classes replace
# failures by a large penalized value so the optimizer can proceed.

# %%
import math

import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv


# %%
# Define a failure-prone model
# -----------------------------
#
# We minimize a quadratic function with optimum at :math:`(1, 1)`,
# but the model raises an exception inside the disk
# :math:`(x_1 - 0.5)^2 + (x_2 - 0.5)^2 < 0.375^2`,
# e.g. as if a solver failed to converge there.
# This failing region sits in the middle of the search space,
# so any space-filling search cannot avoid stepping into it.

# %%
def failing_py(x):
    if (x[0] - 0.5) ** 2 + (x[1] - 0.5) ** 2 < 0.375**2:
        raise RuntimeError("simulation fails inside the failing region")
    return [(x[0] - 1.0) ** 2 + (x[1] - 1.0) ** 2]


model = ot.PythonFunction(2, 1, failing_py)

# %%
# The model works on feasible points:
print(model([1.0, 1.0]))

# %%
# but it throws inside the failing region:
try:
    model([0.5, 0.5])
except RuntimeError as ex:
    print(f"evaluation failed: {ex}")

# %%
# Wrap the problem with penalization
# -----------------------------------
#
# We define the optimization problem on :math:`[-2, 2]^2`:

# %%
problem = ot.OptimizationProblem(model)
problem.setMinimization(True)
problem.setBounds(ot.Interval([-2.0] * 2, [2.0] * 2))

# %%
# Starting from a failing point, a classical solver stops at the first failure:

# %%
algo = ot.Cobyla(problem)
algo.setStartingPoint([0.5, 0.5])
try:
    algo.run()
except TypeError as ex:
    print(f"optimization failed: {ex}")

# %%
# The :class:`~openturns.experimental.PenalizedProblem` class wraps the problem
# and replaces its objective by a penalized function:
# a failed evaluation returns a large positive value for minimization
# (a large negative value for maximization).
# Bounds, constraints and minimization flags are preserved.

# %%
penalizedProblem = otexp.PenalizedProblem(problem, 1.0e6)

# %%
# The penalized objective agrees with the model on feasible points:
print(penalizedProblem.getObjective()([1.0, 1.0]))

# %%
# and returns the penalized value on failures:
print(penalizedProblem.getObjective()([0.5, 0.5]))

# %%
# Batch evaluations mix both cases, which is useful for
# population-based solvers such as :class:`~openturns.Pagmo`:
print(penalizedProblem.getObjective()(ot.Sample([[0.5, 0.5], [1.0, 1.0]])))

# %%
# The low-level :class:`~openturns.experimental.PenalizedEvaluation` class
# offers the same mechanism on a single evaluation:
penalizedEvaluation = otexp.PenalizedEvaluation(model.getEvaluation(), 1.0e6)
print(ot.Function(penalizedEvaluation)([0.5, 0.5]))

# %%
# Solve the penalized problem
# ----------------------------
#
# We use a global evolutionary solver: its initial population lies in
# :math:`[-2, 0]^2`, far from the optimum, so the search must travel
# across the failing disk to reach :math:`(1, 1)`.
# Without penalization the search stops at the first failure:

# %%
ot.RandomGenerator.SetSeed(0)
pop0 = ot.JointDistribution([ot.Uniform(-2.0, 0.0)] * 2).getSample(30)
algo = ot.Pagmo(problem, "de", pop0)
algo.setMaximumIterationNumber(10)
try:
    algo.run()
except RuntimeError as ex:
    print(f"optimization failed: {ex}")
crashedResult = algo.getResult()
failedInputSample = crashedResult.getInputSample()
print(f"evaluations before failure: {failedInputSample.getSize()}")
print("best f before failure:", model(failedInputSample).getMin())

# %%
# With penalization the failed evaluations are replaced by the penalized
# value and the search proceeds across the failing region:

# %%
algo = ot.Pagmo(penalizedProblem, "de", pop0)
algo.setMaximumIterationNumber(10)
algo.run()
result = algo.getResult()
print("x*=", result.getOptimalPoint())
print("f(x*)=", result.getOptimalValue())

# %%
# The search stepped into the failing region on its way to the optimum:
# these evaluations were replaced by the penalized value
# instead of stopping the optimization:
inputSample = result.getInputSample()
outputSample = penalizedProblem.getObjective()(inputSample)
nFailed = sum(1 for i in range(outputSample.getSize()) if outputSample[i, 0] >= 1.0e6)
print(f"failed evaluations: {nFailed} / {outputSample.getSize()}")

# %%
# Despite these failures the optimizer recovered the
# minimum :math:`(1, 1)` with value :math:`0`.

# %%
# Graphical comparison
# ---------------------

# %%
# Draw the penalized objective isovalues with each search path.
# We use the penalized objective (and not the raw model) as it can be
# evaluated everywhere, including in the failing region.
penalizedFunction = penalizedProblem.getObjective()
# Outline of the failing disk, drawn in red on both graphs:
angles = [2.0 * math.pi * i / 100 for i in range(101)]
disk = ot.Curve(
    [[0.5 + 0.375 * math.cos(t), 0.5 + 0.375 * math.sin(t)] for t in angles]
)
disk.setColor("red")
disk.setLegend("failing region")
graph = penalizedFunction.draw([-2.0] * 2, [2.0] * 2, [100] * 2)
graph.setTitle("Unpenalized search path (failed)")
graph.getDrawable(0).setLevels([0.5, 1.0, 2.0, 4.0, 8.0])
graph.add(disk)
failedCloud = ot.Cloud(failedInputSample)
failedCloud.setColor("orange")
failedCloud.setLegend("evaluations")
graph.add(failedCloud)
popCloud = ot.Cloud(pop0)
popCloud.setColor("green")
popCloud.setLegend("initial population")
graph.add(popCloud)
graph.setLegendPosition("lower right")
view = otv.View(graph)

# %%
graph = penalizedFunction.draw([-2.0] * 2, [2.0] * 2, [100] * 2)
graph.setTitle("Penalized search path")
graph.getDrawable(0).setLevels([0.5, 1.0, 2.0, 4.0, 8.0])
graph.add(disk)
okSample = ot.Sample(
    [inputSample[i] for i in range(inputSample.getSize()) if outputSample[i, 0] < 1.0e6]
)
failedPenalizedSample = ot.Sample(
    [inputSample[i] for i in range(inputSample.getSize()) if outputSample[i, 0] >= 1.0e6]
)
cloud = ot.Cloud(okSample)
cloud.setColor("blue")
cloud.setLegend("evaluations")
graph.add(cloud)
popCloud = ot.Cloud(pop0)
popCloud.setColor("green")
popCloud.setLegend("initial population")
graph.add(popCloud)
failedCloud = ot.Cloud(failedPenalizedSample)
failedCloud.setColor("red")
failedCloud.setLegend("failed evaluations")
graph.add(failedCloud)
optimum = ot.Cloud(ot.Sample([result.getOptimalPoint()]))
optimum.setColor("orange")
optimum.setPointStyle("bullet")
optimum.setLegend("optimum")
graph.add(optimum)
graph.setLegendPosition("lower right")
view = otv.View(graph)

# %%
# The plateau at :math:`10^6` corresponds to the failing disk,
# outlined in red. The unpenalized search stopped at its edge, while
# the penalized search crossed it: the red dots inside the disk
# are failed evaluations which were replaced by the penalized value
# on the way to the minimum (blue dots, orange optimum).

# %%
graph = result.drawOptimalValueHistory()
view = otv.View(graph)

# %%
# Display all figures
otv.View.ShowAll()
