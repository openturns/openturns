"""
EfficientGlobalOptimization examples
====================================
"""

# %%
# The EGO algorithm [jones1998]_ is an adaptative optimization method based on
# Gaussian Process metamodel.
#
# An initial design of experiment is used to build a first metamodel.
# At each iteration a new point that maximizes a criterion is chosen as
# optimizer candidate.
#
# The criterion uses a tradeoff between the metamodel value and the conditional
# variance.
#
# Then the new point is evaluated using the original model and the metamodel is
# relearnt on the extended design of experiments.


# %%
from openturns.usecases import branin_function
from openturns.usecases import ackley_function
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv


# %%
# Ackley test-case
# ----------------
#
# We first apply the EGO algorithm on the :ref:`Ackley function<use-case-ackley>`.

# %%
# Define the problem
# ^^^^^^^^^^^^^^^^^^

# %%
# The Ackley model is defined in the usecases module in a data class `AckleyModel` :
am = ackley_function.AckleyModel()

# %%
# We get the Ackley function :
model = am.model

# %%
# We specify the domain of the model :
dim = am.dim
lowerbound = [-4.0] * dim
upperbound = [4.0] * dim

# %%
# We know that the global minimum is at the center of the domain. It is stored in the `AckleyModel` data class.
xexact = am.x0

# %%
# The minimum value attained `fexact` is :
fexact = model(xexact)
fexact

# %%
graph = model.draw(lowerbound, upperbound, [100] * dim)
graph.setTitle("Ackley function")
view = otv.View(graph)

# %%
# We see that the Ackley function has many local minimas. The global minimum, however, is unique and located at the center of the domain.

# %%
# Create the initial GP
# ^^^^^^^^^^^^^^^^^^^^^
#
# Before using the EGO algorithm, we must create an initial GP metamodel.
# In order to do this, we must create a design of experiment which fills the space.
# In this situation, the :class:`~openturns.LHSExperiment` is a good place to start (but other design of experiments may allow one to better fill the space).
# We use a uniform distribution in order to create a LHS design.
# The length of the first LHS is set to ten times the problem dimension as recommended in [jones1998]_.

# %%
listUniformDistributions = [
    ot.Uniform(lowerbound[i], upperbound[i]) for i in range(dim)
]
distribution = ot.JointDistribution(listUniformDistributions)
sampleSize = 10 * dim
experiment = ot.LHSExperiment(distribution, sampleSize)
inputSample = experiment.generate()
outputSample = model(inputSample)

# %%
graph = ot.Graph(
    "Initial LHS design of experiment - n=%d" % (sampleSize), "$x_0$", "$x_1$", True
)
cloud = ot.Cloud(inputSample)
graph.add(cloud)
view = otv.View(graph)

# %%
# We now create the GP metamodel.
# We selected the :class:`~openturns.MaternModel` covariance model with a constant basis as recommended in [leriche2021]_.

# %%
covarianceModel = ot.MaternModel([1.0] * dim, [0.5], 2.5)
basis = ot.ConstantBasisFactory(dim).build()
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.run()
gpr = ot.GaussianProcessRegression(fitter.getResult())
gpr.run()

# %%
# Create the optimization problem
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# We finally create the :class:`~openturns.OptimizationProblem` and solve it with class:`~openturns.EfficientGlobalOptimization`.

# %%
problem = ot.OptimizationProblem()
problem.setObjective(model)
bounds = ot.Interval(lowerbound, upperbound)
problem.setBounds(bounds)

# %%
# In order to show the various options, we configure them all, even if most could be left to default settings in this case.
#
# The most important method is :class:`~openturns.experimental.EfficientGlobalOptimization.setMaximumCallsNumber` which limits the number of iterations that the algorithm can perform.
# In the Ackley example, we choose to perform 30 iterations of the algorithm.

# %%
algo = otexp.EfficientGlobalOptimization(problem, gpr.getResult())
algo.setMaximumCallsNumber(30)
algo.run()
result = algo.getResult()

# %%
result.getIterationNumber()

# %%
result.getOptimalPoint()

# %%
result.getOptimalValue()

# %%
fexact

# %%
# Compared to the minimum function value, we see that the EGO algorithm
# provides solution which is accurate.
# Indeed, the optimal point is in the neighbourhood of the exact solution,
# and this is quite an impressive success given the limited amount of function
# evaluations: only 20 evaluations for the initial DOE and 30 iterations of
# the EGO algorithm, for a total equal to 50 function evaluations.

# %%
graph = result.drawOptimalValueHistory()
optimum_curve = ot.Curve(ot.Sample([[0, fexact[0]], [29, fexact[0]]]))
graph.add(optimum_curve)
view = otv.View(graph)

# %%
inputHistory = result.getInputSample()

# %%
graph = model.draw(lowerbound, upperbound, [100] * dim)
graph.setTitle("Ackley function")
cloud = ot.Cloud(inputSample, "initial")
cloud.setPointStyle("bullet")
cloud.setColor("black")
graph.add(cloud)
cloud = ot.Cloud(inputHistory, "solution")
cloud.setPointStyle("diamond")
cloud.setColor("forestgreen")
graph.add(cloud)
view = otv.View(graph)

# %%
# We see that the initial (black) points are dispersed in the whole domain,
# while the solution points are much closer to the solution.

# %%
# Branin test-case
# ----------------
#
# We now take a look at the :ref:`Branin-Hoo<use-case-branin>` function.

# %%
# Define the problem
# ^^^^^^^^^^^^^^^^^^

# %%
# The Branin model is defined in the usecases module in a data class `BraninModel` :
bm = branin_function.BraninModel()

# %%
# We load the dimension,
dim = bm.dim

# %%
# the domain boundaries,
lowerbound = bm.lowerbound
upperbound = bm.upperbound

# %%
# and we load the model function and its noise:
objectiveFunction = bm.model
noise = bm.trueNoiseFunction

# %%
# We build a sample out of the three minima :
xexact = ot.Sample([bm.xexact1, bm.xexact2, bm.xexact3])

# %%
# The minimum value attained `fexact` is :
fexact = objectiveFunction(xexact)
fexact

# %%
graph = objectiveFunction.draw(lowerbound, upperbound, [100] * dim)
graph.setTitle("Branin function")
view = otv.View(graph)

# %%
# The Branin function has three local minimas.

# %%
# Create the initial GP
# ^^^^^^^^^^^^^^^^^^^^^

# %%
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dim)
sampleSize = 10 * dim
experiment = ot.LHSExperiment(distribution, sampleSize)
inputSample = experiment.generate()
outputSample = objectiveFunction(inputSample)

# %%
graph = ot.Graph(
    "Initial LHS design of experiment - n=%d" % (sampleSize), "$x_0$", "$x_1$", True
)
cloud = ot.Cloud(inputSample)
graph.add(cloud)
view = otv.View(graph)

# %%
# Configure the covariance model with the noise
covarianceModel = ot.MaternModel([1.0] * dim, [0.5], 2.5)
covarianceModel.setNuggetFactor(noise)

# %%
# Build the initial GP
basis = ot.ConstantBasisFactory(dim).build()
fitter = ot.GaussianProcessFitter(inputSample, outputSample, covarianceModel, basis)
fitter.run()
gpr = ot.GaussianProcessRegression(fitter.getResult())
gpr.run()

# %%
# Create and solve the problem
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^

# %%
# We define the problem :
problem = ot.OptimizationProblem()
problem.setObjective(objectiveFunction)
bounds = ot.Interval(lowerbound, upperbound)
problem.setBounds(bounds)

# %%
# We configure the algorithm
# the nugget factor set in the covariance model with enable the AEI formulation
algo = otexp.EfficientGlobalOptimization(problem, gpr.getResult())
algo.setMaximumCallsNumber(30)

# %%
# We run the algorithm and get the result:
algo.run()
result = algo.getResult()

# %%
result.getIterationNumber()

# %%
result.getOptimalPoint()

# %%
result.getOptimalValue()

# %%
fexact

# %%
inputHistory = result.getInputSample()

# %%
graph = objectiveFunction.draw(lowerbound, upperbound, [100] * dim)
graph.setTitle("Branin function")
cloud = ot.Cloud(inputSample, "initial")
cloud.setPointStyle("bullet")
cloud.setColor("black")
graph.add(cloud)
cloud = ot.Cloud(inputHistory, "solution")
cloud.setPointStyle("diamond")
cloud.setColor("forestgreen")
graph.add(cloud)
view = otv.View(graph)

# %%
# We see that the EGO algorithm reached the different optima locations.

# %%
graph = result.drawOptimalValueHistory()
optimum_curve = ot.Curve(ot.Sample([[0, fexact[0][0]], [29, fexact[0][0]]]))
graph.add(optimum_curve)
view = otv.View(graph, axes_kw={"xticks": range(0, result.getIterationNumber(), 5)})


# %%
otv.View.ShowAll()
