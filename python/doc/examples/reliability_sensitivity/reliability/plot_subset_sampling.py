"""
Subset Sampling
===============
"""

# %%
#
# The objective is to evaluate a probability from the Subset sampling technique.
#
# We consider the function :math:`g : \mathbb{R}^2 \rightarrow \mathbb{R}` defined by:
#
# .. math::
#   \begin{align*}
#   g(X)= 20-(x_1-x_2)^2-8(x_1+x_2-4)^3
#   \end{align*}
#
# and the input random vector :math:`X = (X_1, X_2)` which follows a Normal distribution with independent components,
# and identical marginals with 0.25 mean and unit variance:
#
# .. math::
#   \begin{align*}
#   X \sim  \mathcal{N}(\mu = [0.25, 0.25], \sigma = [1,1], cor = I_2)
#   \end{align*}
#
# We want to evaluate the probability:
#
# .. math::
#   \begin{align*}
#   p = \mathbb{P} \{ g(X) \leq 0 \}
#   \end{align*}
#

# %%
# First, import the python modules:

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Create the probabilistic model :math:`Y = g(X)`
# -----------------------------------------------

# %%
# Create the input random vector :math:`X`:
X = ot.RandomVector(ot.Normal([0.25] * 2, [1] * 2, ot.IdentityMatrix(2)))

# %%
# Create the function :math:`g`:
g = ot.SymbolicFunction(["x1", "x2"], ["20-(x1-x2)^2-8*(x1+x2-4)^3"])
print("function g: ", g)

# %%
# Create the output random vector :math:`Y = g(X)`:
Y = ot.CompositeRandomVector(g, X)

# %%
# Create the event :math:`\{ Y = g(X) \leq 0 \}`
# ----------------------------------------------
event = ot.ThresholdEvent(Y, ot.Less(), 0.0)

# %%
# Evaluate the probability with the subset sampling technique
# -----------------------------------------------------------

# %%
algo = ot.SubsetSampling(event)

# %%
# In order to get all the inputs and outputs that realize the event, you have to mention it now:
algo.setKeepSample(True)

# %%
# Now you can run the algorithm!
algo.run()

# %%
result = algo.getResult()
proba = result.getProbabilityEstimate()
print("Proba Subset = ", proba)
print("Current coefficient of variation = ", result.getCoefficientOfVariation())

# %%
# The length of the confidence interval of level :math:`95\%` is:
length95 = result.getConfidenceLength()
print("Confidence length (0.95) = ", result.getConfidenceLength())

# %%
# which enables to build the confidence interval:
print(
    "Confidence interval (0.95) = [",
    proba - length95 / 2,
    ", ",
    proba + length95 / 2,
    "]",
)

# %%
# You can also get the successive thresholds used by the algorithm:
levels = algo.getThresholdPerStep()
print("Levels of g = ", levels)

# %%
# Draw the subset samples used by the algorithm
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
# The following graph draws each subset sample and the frontier :math:`g(x_1, x_2) = l_i` where :math:`l_i` is the threshold at the step :math:`i`:
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
graph.setTitle("Subset sampling: samples")
graph.setXTitle(r"$x_1$")
graph.setYTitle(r"$x_2$")
graph.setLegendPosition("lower left")

# %%
# Add all the subset samples:
for i in range(Ns):
    cloud = ot.Cloud(list_subSamples[i])
    cloud.setPointStyle("dot")
    graph.add(cloud)

# %%
# Add the frontiers :math:`g(x_1, x_2) = l_i` where :math:`l_i` is the threshold at the step :math:`i`:
gIsoLines = g.draw([-3] * 2, [5] * 2, [128] * 2)
dr = gIsoLines.getDrawable(0)
dr.setColor("black")
for i in range(levels.getSize()):
    dr.setLevels([levels[i]])
    dr.setLegend(r"$g(X) = $" + str(round(levels[i], 2)))
    graph.add(dr)

# %%
_ = otv.View(graph)

# %%
# Draw the frontiers only
# -----------------------

# %%
# The following graph enables to understand the progression of the algorithm:
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
dr = gIsoLines.getDrawable(0)
colors = ot.Drawable().BuildDefaultPalette(len(levels))
for i in range(levels.getSize()):
    dr.setLevels([levels[i]])
    dr.setLegend(r"$g(X) = $" + str(round(levels[i], 2)))
    dr.setColor(colors[i])
    graph.add(dr)

graph.setLegendPosition("lower left")
graph.setTitle("Subset sampling: thresholds")
graph.setXTitle(r"$x_1$")
graph.setYTitle(r"$x_2$")

_ = otv.View(graph)

# %%
# Get all the input and output points that realized the event
# -----------------------------------------------------------

# %%
# The following lines are possible only if you have mentioned that you wanted to keep samples with the method *algo.setKeepSample(True)*
select = ot.SubsetSampling.EVENT1  # points that realize the event
step = Ns - 1  # get the working sample from last iteration
inputEventSample = algo.getInputSample(step, select)
outputEventSample = algo.getOutputSample(step, select)
print("Number of event realizations = ", inputEventSample.getSize())

# %%
# Draw them! They are all in the event space.
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
cloud = ot.Cloud(inputEventSample)
cloud.setPointStyle("dot")
graph.add(cloud)
gIsoLines = g.draw([-3] * 2, [5] * 2, [1000] * 2)
dr = gIsoLines.getDrawable(0)
dr.setLevels([0.0])
dr.setColor("red")
graph.add(dr)
_ = otv.View(graph)

# %%
otv.View.ShowAll()
