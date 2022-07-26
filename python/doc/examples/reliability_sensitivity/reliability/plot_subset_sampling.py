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
# and the input random vector :math:`X = (X_1, X_2)` which follows a Normal distribution with independent components, and identical marginals with 0.25 mean and unit variance:
#
# .. math::
#   \begin{align*}
#   X \sim  \mathcal{N}(\mu = [0.25, 0.25], \sigma = [1,1], cov = I_2)
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
from openturns.viewer import View

# %%
# Create the probabilistic model :math:`Y = g(X)`
# -----------------------------------------------

# %%
# Create the input random vector :math:`X`:

# %%
X = ot.RandomVector(ot.Normal([0.25]*2, [1]*2, ot.IdentityMatrix(2)))

# %%
# Create the function :math:`g`:

# %%
g = ot.SymbolicFunction(['x1', 'x2'], ['20-(x1-x2)^2-8*(x1+x2-4)^3'])
print('function g: ', g)

# %%
# In order to be able to get the subset samples used in the algorithm, it is necessary to transform the *SymbolicFunction* into a *MemoizeFunction*:

# %%
g = ot.MemoizeFunction(g)

# %%
# Create the output random vector :math:`Y = g(X)`:

# %%
Y = ot.CompositeRandomVector(g, X)

# %%
# Create the event :math:`\{ Y = g(X) \leq 0 \}`
# ----------------------------------------------

# %%
myEvent = ot.ThresholdEvent(Y, ot.LessOrEqual(), 0.0)

# %%
# Evaluate the probability with the subset sampling technique
# -----------------------------------------------------------

# %%
algo = ot.SubsetSampling(myEvent)

# %%
# In order to get all the inputs and outputs that realize the event, you have to mention it now:

# %%
algo.setKeepEventSample(True)

# %%
# Now you can run the algorithm!

# %%
algo.run()

# %%
result = algo.getResult()
proba = result.getProbabilityEstimate()
print('Proba Subset = ',  proba)
print('Current coefficient of variation = ',
      result.getCoefficientOfVariation())

# %%
# The length of the confidence interval of level :math:`95\%` is:

# %%
length95 = result.getConfidenceLength()
print('Confidence length (0.95) = ', result.getConfidenceLength())

# %%
# which enables to build the confidence interval:

# %%
print('Confidence interval (0.95) = [', proba -
      length95/2, ', ', proba + length95/2, ']')

# %%
# You can also get the succesive thresholds used by the algorithm:

# %%
levels = algo.getThresholdPerStep()
print('Levels of g = ', levels)

# %%
# Draw the subset samples used by the algorithm
# ---------------------------------------------
#
# The following manipulations are possible onfly if you have created a *MemoizeFunction* that enables to store all the inputs and output of the function :math:`g`.
#
# Get all the inputs where :math:`g` were evaluated:

# %%
inputSampleSubset = g.getInputHistory()
nTotal = inputSampleSubset.getSize()
print('Number of evaluations of g = ', nTotal)

# %%
# Within each step of the algorithm, a sample of size :math:`N` is created, where:

# %%
N = algo.getMaximumOuterSampling()*algo.getBlockSize()
print('Size of each subset = ', N)

# %%
# You can get the number :math:`N_s` of steps with:

# %%
Ns = algo.getStepsNumber()
print('Number of steps= ', Ns)

# %%
# and you can verify that :math:`N_s` is equal to :math:`\frac{nTotal}{N}`:

# %%
print('nTotal / N = ', int(nTotal / N))

# %%
# Now, we can split the initial sample into subset samples of size :math:`N_s`:

# %%
list_subSamples = list()
for i in range(Ns):
    list_subSamples.append(inputSampleSubset[i*N:i*N + N])

# %%
# The following graph draws each subset sample and the frontier :math:`g(x_1, x_2) = l_i` where :math:`l_i` is the threshold at the step :math:`i`:

# %%
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
graph.setTitle('Subset sampling: samples')
graph.setXTitle(r'$x_1$')
graph.setYTitle(r'$x_2$')
graph.setLegendPosition('bottomleft')

# %%
# Add all the subset samples:

# %%
for i in range(Ns):
    cloud = ot.Cloud(list_subSamples[i])
    # cloud.setPointStyle("dot")
    graph.add(cloud)
col = ot.Drawable().BuildDefaultPalette(Ns)
graph.setColors(col)

# %%
# Add the frontiers :math:`g(x_1, x_2) = l_i` where :math:`l_i` is the threshold at the step :math:`i`:

# %%
gIsoLines = g.draw([-3]*2, [5]*2, [128]*2)
dr = gIsoLines.getDrawable(0)
for i in range(levels.getSize()):
    dr.setLevels([levels[i]])
    dr.setLineStyle('solid')
    dr.setLegend(r'$g(X) = $' + str(round(levels[i], 2)))
    dr.setLineWidth(3)
    dr.setColor(col[i])
    graph.add(dr)

# %%
View(graph)

# %%
# Draw the frontiers only
# -----------------------
#
# The following graph enables to understand the progresison of the algorithm:

# %%
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
dr = gIsoLines.getDrawable(0)
for i in range(levels.getSize()):
    dr.setLevels([levels[i]])
    dr.setLineStyle('solid')
    dr.setLegend(r'$g(X) = $' + str(round(levels[i], 2)))
    dr.setLineWidth(3)
    graph.add(dr)

graph.setColors(col)
graph.setLegendPosition('bottomleft')
graph.setTitle('Subset sampling: thresholds')
graph.setXTitle(r'$x_1$')
graph.setYTitle(r'$x_2$')

View(graph)

# %%
# Get all the input and output points that realized the event
# -----------------------------------------------------------
# The following lines are possible only if you have mentioned that you wanted to keep the points that realize the event with the method *algo.setKeepEventSample(True)*

# %%
inputEventSample = algo.getEventInputSample()
outputEventSample = algo.getEventOutputSample()
print('Number of event realizations = ', inputEventSample.getSize())

# %%
# Here we have to avoid a bug of the version 1.15 because *getEventInputSample()* gives the sample in the stadrad space: we have to push it backward to the physical space.

# %%
dist = ot.Normal([0.25]*2, [1]*2, ot.IdentityMatrix(2))
transformFunc = dist.getInverseIsoProbabilisticTransformation()
inputEventSample = transformFunc(inputEventSample)

# %%
# Draw them! They are all in the event space.

# %%
graph = ot.Graph()
graph.setAxes(True)
graph.setGrid(True)
cloud = ot.Cloud(inputEventSample)
cloud.setPointStyle('dot')
graph.add(cloud)
gIsoLines = g.draw([-3]*2, [5]*2, [1000]*2)
dr = gIsoLines.getDrawable(0)
dr.setLevels([0.0])
dr.setColor('red')
graph.add(dr)
View(graph)
