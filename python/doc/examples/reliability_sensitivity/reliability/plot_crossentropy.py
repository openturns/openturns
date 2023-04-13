"""
Cross Entropy Importance Sampling
=================================
"""
# %%
#
# The objective is to evaluate a failure probability from the two versions of Cross Entropy Importance Sampling working in Standard or Physical spaces, see :class:`~openturns.StandardSpaceCrossEntropyImportanceSampling` and :class:`~openturns.PhysicalSpaceCrossEntropyImportanceSampling`.
# We consider the simple stress beam example: :ref:`axial stressed beam <use-case-stressed-beam>`.


# %%
# First, we import the python modules:

# %%
import openturns as ot
import openturns.viewer as viewer
from openturns.usecases import stressed_beam

ot.RandomGenerator.SetSeed(0)

# %%
# Create the probabilistic model
# ------------------------------

# %%
axialBeam = stressed_beam.AxialStressedBeam()
distribution = axialBeam.distribution
print("Initial distribution:", distribution)

# %%
# Draw the limit state function :math:`g` to help to understand the shape of the limit state.

# %%
g = axialBeam.model
graph = ot.Graph("Simple stress beam", "R", "F", True, "topright")
drawfunction = g.draw([1.8e6, 600], [4e6, 950.0], [100] * 2)
graph.add(drawfunction)
view = viewer.View(graph)

# %%
# Create the output random vector :math:`Y = g(\vect{X})` with :math:`\vect{X} = [R,F]`.

# %%
X = ot.RandomVector(distribution)
Y = ot.CompositeRandomVector(g, X)

# %%
# Create the event :math:`\{ Y = g(\vect{X}) \leq 0 \}`
# -----------------------------------------------------

# %%
threshold = 0.0
event = ot.ThresholdEvent(Y, ot.Less(), 0.0)

# %%
# Evaluate the probability with the Standard Space Cross Entropy
# --------------------------------------------------------------

# %%
# We choose to set the intermediate quantile level to 0.35.

# %%
standardSpaceIS = ot.StandardSpaceCrossEntropyImportanceSampling(event, 0.35)

# %%
# Now we can run the algorithm and get the results.

# %%
standardSpaceIS.run()
standardSpaceISResult = standardSpaceIS.getResult()
proba = standardSpaceISResult.getProbabilityEstimate()
print("Proba Standard Space Cross Entropy IS = ", proba)
print(
    "Current coefficient of variation = ",
    standardSpaceISResult.getCoefficientOfVariation(),
)

# %%
# The length of the confidence interval of level :math:`95\%` is:

# %%
length95 = standardSpaceISResult.getConfidenceLength()
print("Confidence length (0.95) = ", standardSpaceISResult.getConfidenceLength())

# %%
# which enables to build the confidence interval.

# %%
print(
    "Confidence interval (0.95) = [",
    proba - length95 / 2,
    ", ",
    proba + length95 / 2,
    "]",
)

# %%
# We can analyze the simulation budget.

# %%
print(
    "Numerical budget: ",
    standardSpaceISResult.getBlockSize() * standardSpaceISResult.getOuterSampling(),
)

# %%
# We can also retrieve the optimal auxiliary distribution in Standard Space.

# %%
print(
    "Auxiliary distribution in Standard space: ",
    standardSpaceISResult.getAuxiliaryDistribution(),
)


# %%
# Draw initial samples and final samples
# --------------------------------------
#

# %%
# First we get the auxiliary samples in Standard Space and we project them in Physical Space.

# %%
auxiliaryInputSamples = standardSpaceISResult.getAuxiliaryInputSample()
auxiliaryInputSamplesPhysicalSpace = distribution.getInverseIsoProbabilisticTransformation()(
    auxiliaryInputSamples
)


# %%
myGraph = ot.Graph("Cloud of samples and failure domain", "R", "F", True, "topright")
# Generation of samples with initial distribution
InitialSamples = ot.Cloud(
    distribution.getSample(1000), "blue", "plus", "Initial samples"
)
AuxiliarySamples = ot.Cloud(
    auxiliaryInputSamplesPhysicalSpace, "orange", "fsquare", "Auxiliary samples"
)
# Plot failure domain
nx, ny = 50, 50
xx = ot.Box([nx], ot.Interval([1.80e6], [4e6])).generate()
yy = ot.Box([ny], ot.Interval([600.0], [950.0])).generate()
inputData = ot.Box([nx, ny], ot.Interval([1.80e6, 600.0], [4e6, 950.0])).generate()
outputData = g(inputData)
mycontour = ot.Contour(xx, yy, outputData, [0.0], ["0.0"], True)
mycontour.setLegend("Failure domain")
myGraph.add(InitialSamples)
myGraph.add(AuxiliarySamples)
myGraph.add(mycontour)
view = viewer.View(myGraph)

# %%
# In the previous graph, the blue crosses stand for samples drawn with the initial distribution and the orange squares stand for the samples drawn at the final iteration.

# %%
# Estimation of the probability with the Physical Space Cross Entropy
# -------------------------------------------------------------------

# %%
# For more advanced use, it is possible to work in the physical space to specify the auxiliary distribution.
# In this second example, we take the auxiliary distribution in the same family as the initial distribution and we want to optimize all the parameters.

# %%
# The parameters to be optimized are the parameters of the native distribution. It is necessary to define among all the distribution parameters, which ones will be optimized through the indices of the parameters. In this case, all the parameters will be optimized.

# %%
# Be careful that the native parameters of the auxiliary distribution will be considered. Here for the :class:`~openturns.LogNormalMuSigma` distribution, this corresponds to muLog, sigmaLog and gamma.

# %%
# The user can use `getParameterDescription()` method to access to the parameters of the auxiliary distribution.

# %%
ot.RandomGenerator.SetSeed(0)
distribution_marginR = ot.LogNormalMuSigma().getDistribution()
distribution_marginF = ot.Normal()
aux_marginals = [distribution_marginR, distribution_marginF]
aux_distribution = ot.ComposedDistribution(aux_marginals)
# Definition of parameters to be optimized
active_parameters = [0, 1, 2, 3, 4]
# WARNING : native parameters of distribution have to be considered
bounds = ot.Interval([14, 0.01, 0.0, 500, 20], [16, 0.2, 0.1, 1000, 70])
initial_theta = distribution.getParameter()
desc = aux_distribution.getParameterDescription()
p = aux_distribution.getParameter()
print(
    "Parameters of the auxiliary distribution:",
    ", ".join([f"{param}: {value:.3f}" for param, value in zip(desc, p)]),
)

physicalSpaceIS1 = ot.PhysicalSpaceCrossEntropyImportanceSampling(
    event, active_parameters, initial_theta, bounds, aux_distribution
)

# %%
# Custom solver can be also specified for the auxiliary distribution parameters optimization, here for example we choose :class:`~openturns.TNC`.

# %%
physicalSpaceIS1.setSolver(ot.TNC())

# %%
# The number of samples per step can also be specified.

# %%
physicalSpaceIS1.setMaximumOuterSampling(1000)

# %%
# Finally, we run the algorithm and get the results.

# %%
physicalSpaceIS1.run()
physicalSpaceISResult1 = physicalSpaceIS1.getResult()
print("Probability of failure:", physicalSpaceISResult1.getProbabilityEstimate())
print("Coefficient of variation:", physicalSpaceISResult1.getCoefficientOfVariation())

# %%
# We can also decide to optimize only the means of the margins and keep the other parameters identical to the initial distribution.

# %%
ot.RandomGenerator.SetSeed(0)
distribution_marginR = ot.LogNormalMuSigma(3e6, 3e5, 0.0).getDistribution()
distribution_marginF = ot.Normal(750.0, 50.0)
aux_marginals = [distribution_marginR, distribution_marginF]
aux_distribution = ot.ComposedDistribution(aux_marginals)
print("Parameters of initial distribution", aux_distribution.getParameter())

# Definition of parameters to be optimized
active_parameters = [0, 3]
# WARNING : native parameters of distribution have to be considered
bounds = ot.Interval([14, 500], [16, 1000])
initial_theta = [15, 750]
physicalSpaceIS2 = ot.PhysicalSpaceCrossEntropyImportanceSampling(
    event, active_parameters, initial_theta, bounds, aux_distribution
)
physicalSpaceIS2.run()
physicalSpaceISResult2 = physicalSpaceIS2.getResult()
print("Probability of failure:", physicalSpaceISResult2.getProbabilityEstimate())
print("Coefficient of variation:", physicalSpaceISResult2.getCoefficientOfVariation())

# %%
# Let analyze the auxiliary output samples for the two previous simulations. We can plot initial (in blue) and auxiliary samples in physical space (orange for the first simulation and black for the second simulation).

# %%
myGraph = ot.Graph("Cloud of samples and failure domain", "R", "F", True, "topright")
AuxiliarySamples1 = ot.Cloud(
    physicalSpaceISResult1.getAuxiliaryInputSample(),
    "orange",
    "fsquare",
    "Auxiliary samples, first case",
)
AuxiliarySamples2 = ot.Cloud(
    physicalSpaceISResult2.getAuxiliaryInputSample(),
    "black",
    "bullet",
    "Auxiliary samples, second case",
)
myGraph.add(InitialSamples)
myGraph.add(AuxiliarySamples1)
myGraph.add(AuxiliarySamples2)
myGraph.add(mycontour)
view = viewer.View(myGraph)

# %%
# By analyzing the failure samples, one may want to include correlation parameters in the auxiliary distribution. In this last example, we add a Normal copula. The correlation parameter will be optimized with associated interval between 0 and 1.

# %%
ot.RandomGenerator.SetSeed(0)
distribution_marginR = ot.LogNormalMuSigma(3e6, 3e5, 0.0).getDistribution()
distribution_marginF = ot.Normal(750.0, 50.0)
aux_marginals = [distribution_marginR, distribution_marginF]
copula = ot.NormalCopula()
aux_distribution = ot.ComposedDistribution(aux_marginals, copula)
desc = aux_distribution.getParameterDescription()
p = aux_distribution.getParameter()
print(
    "Initial parameters of the auxiliary distribution:",
    ", ".join([f"{param}: {value:.3f}" for param, value in zip(desc, p)]),
)

# Definition of parameters to be optimized
active_parameters = [0, 1, 2, 3, 4, 5]
bounds = ot.Interval(
    [14, 0.01, 0.0, 500.0, 20.0, 0.0], [16, 0.2, 0.1, 1000.0, 70.0, 1.0]
)
initial_theta = distribution.getParameter()

physicalSpaceIS3 = ot.PhysicalSpaceCrossEntropyImportanceSampling(
    event, active_parameters, initial_theta, bounds, aux_distribution
)
physicalSpaceIS3.run()
physicalSpaceISResult3 = physicalSpaceIS3.getResult()
desc = physicalSpaceISResult3.getAuxiliaryDistribution().getParameterDescription()
p = physicalSpaceISResult3.getAuxiliaryDistribution().getParameter()
print(
    "Optimized parameters of the auxiliary distribution:",
    ", ".join([f"{param}: {value:.3f}" for param, value in zip(desc, p)]),
)
print("Probability of failure: ", physicalSpaceISResult3.getProbabilityEstimate())
print("Coefficient of variation: ", physicalSpaceISResult3.getCoefficientOfVariation())

# %%
# Finally, we plot the new auxiliary samples in black.

# %%
myGraph = ot.Graph("Cloud of samples and failure domain", "R", "F", True, "topright")
AuxiliarySamples1 = ot.Cloud(
    physicalSpaceISResult1.getAuxiliaryInputSample(),
    "orange",
    "fsquare",
    "Auxiliary samples, first case",
)
AuxiliarySamples3 = ot.Cloud(
    physicalSpaceISResult3.getAuxiliaryInputSample(),
    "black",
    "bullet",
    "Auxiliary samples, second case",
)
myGraph.add(InitialSamples)
myGraph.add(AuxiliarySamples1)
myGraph.add(AuxiliarySamples3)
myGraph.add(mycontour)

# sphinx_gallery_thumbnail_number = 4
view = viewer.View(myGraph)

# %%
# The optimized auxiliary distribution with the dependency between the two margins allows one to better fit the failure domain resulting a lower coefficient of variation.
