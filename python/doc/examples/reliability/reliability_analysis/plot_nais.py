"""
Non parametric Adaptive Importance Sampling (NAIS)
==================================================
"""

# %%
#
# The objective is to evaluate a probability from the Non parametric Adaptive Importance Sampling (NAIS) technique.
#
# We consider the Four Branch function use case (see :ref:`use-case-four-branch`).
# The function is a 2D limit state function with four distinct failure modes.
# The standard benchmark uses independent standard normal inputs and parameter :math:`p=6`.
#
# We want to evaluate the failure probability:
#
# .. math::
#   \begin{align*}
#   p_f = \mathbb{P} ( f(\vect{X}, 6) \leq 0 )
#   \end{align*}
#
# where :math:`f` is the Four Branch function and :math:`\vect{X} = (X_1, X_2)` follows
# the standard 2-dimensional Normal distribution.
#

# %%
# First, import the python modules:

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Create the probabilistic model :math:`Y = f(\vect{X})`
# ------------------------------------------------------

# %%
# Load the Four Branch function use case:

# %%
from openturns.usecases import four_branch_function

fb = four_branch_function.FourBranchModel()

# %%
# Create the input random vector :math:`\vect{X}`:

# %%
X = ot.RandomVector(fb.distribution)

# %%
# Get the limit state function :math:`f`:

# %%
g = fb.model

# %%
# Draw the function :math:`f` to help to understand the shape of the limit state function:

# %%
graph = ot.Graph("Four Branch function", "x1", "x2")
graph.setLegendPosition("upper right")
drawfunction = g.draw([-8] * 2, [8] * 2, [100] * 2)
graph.add(drawfunction)
view = otv.View(graph)


# %%
# In order to be able to get the NAIS samples used in the algorithm, it is necessary to transform the :class:`~openturns.PythonFunction` into a :class:`~openturns.MemoizeFunction`:

# %%
g = ot.MemoizeFunction(g)

# %%
# Create the output random vector :math:`Y = f(\vect{X})`:

# %%
Y = ot.CompositeRandomVector(g, X)

# %%
# Create the event :math:`\{ Y = g(\vect{X}) \leq 0 \}`
# -----------------------------------------------------

# %%
threshold = 0.0
myEvent = ot.ThresholdEvent(Y, ot.Less(), threshold)

# %%
# Evaluate the probability with the NAIS technique
# ------------------------------------------------

# %%
quantileLevel = 0.1
algo = ot.NAIS(myEvent, quantileLevel)

# %%
# In order to get all the inputs and outputs that realize the event, you have to mention it now:

# %%
algo.setKeepSample(True)

# %%
# Now you can run the algorithm.

# %%
algo.run()
result = algo.getResult()
proba = result.getProbabilityEstimate()
print("Proba NAIS = ", proba)
print("Current coefficient of variation = ", result.getCoefficientOfVariation())

# %%
# The confidence interval of level :math:`95\%` is:
ci95 = result.getProbabilityDistribution().computeBilateralConfidenceInterval(0.95)
print("Confidence interval (0.95) = ", ci95)

# %%
# You can also get the successive thresholds used by the algorithm:

# %%
levels = algo.getThresholdPerStep()
print("Levels of g = ", levels)

# %%
# Draw the NAIS samples used by the algorithm
# ---------------------------------------------

# %%
# You can get the number :math:`N_s` of steps with:
Ns = algo.getStepsNumber()
print("Number of steps= ", Ns)

# %%
# Get all the inputs where :math:`g` was evaluated at each step
list_subSamples = list()
for step in range(Ns):
    list_subSamples.append(algo.getInputSample(step))

# %%
# The following graph draws each NAIS sample and the frontier :math:`g(x_1, x_2) = l_i` where :math:`l_i` is the threshold at the step :math:`i`:

# %%
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
graph.setTitle("NAIS sampling: samples")
graph.setXTitle(r"$x_1$")
graph.setYTitle(r"$x_2$")
graph.setLegendPosition("lower left")

# %%
# Add all the NAIS samples:

# %%
for i in range(Ns):
    cloud = ot.Cloud(list_subSamples[i])
    # cloud.setPointStyle("dot")
    graph.add(cloud)
col = graph.getColors()

# %%
# Add the frontiers :math:`g(x_1, x_2) = l_i` where :math:`l_i` is the threshold at the step :math:`i`:

# %%
gIsoLines = g.draw([-5] * 2, [5] * 2, [128] * 2)
dr = gIsoLines.getDrawable(0)
for i, lv in enumerate(levels):
    dr.setLevels([lv])
    dr.setLineStyle("solid")
    dr.setLegend(r"$g(X) = $" + str(round(lv, 2)))
    dr.setLineWidth(3)
    dr.setColor(col[i])
    graph.add(dr)

# %%
_ = otv.View(graph)

# %%
# Draw the frontiers only
# -----------------------
#
# The following graph enables to understand the progression of the algorithm:

# %%
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
dr = gIsoLines.getDrawable(0)
for i, lv in enumerate(levels):
    dr.setLevels([lv])
    dr.setLineStyle("solid")
    dr.setLegend(r"$g(X) = $" + str(round(lv, 2)))
    dr.setLineWidth(3)
    graph.add(dr)

graph.setColors(col)
graph.setLegendPosition("lower left")
graph.setTitle("NAIS sampling: thresholds")
graph.setXTitle(r"$x_1$")
graph.setYTitle(r"$x_2$")

_ = otv.View(graph)

# %%
# Get all the input and output points that realized the event
# -----------------------------------------------------------
# The following lines are possible only if you have mentioned that you wanted to keep samples with the method *algo.setKeepSample(True)*

# %%
select = ot.NAIS.EVENT1  # points that realize the event
step = Ns - 1  # get the working sample from last iteration
inputEventSample = algo.getInputSample(step, select)
outputEventSample = algo.getOutputSample(step, select)
print("Number of event realizations = ", inputEventSample.getSize())

# %%
# Draw them! They are all in the event space.

# %%
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
cloud = ot.Cloud(inputEventSample)
cloud.setPointStyle("bullet")
graph.add(cloud)
# A 500x500 grid is enough for the single level-0 isoline below
gIsoLines = g.draw([-5] * 2, [5] * 2, [500] * 2)
dr = gIsoLines.getDrawable(0)
dr.setLevels([0.0])
dr.setColor("red")
graph.add(dr)
_ = otv.View(graph)

otv.View.ShowAll()
