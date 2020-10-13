"""
Configure the optimization solver of a kriging algorithm
========================================================
"""
# %%
# The goal of this example is to show how to fine-tune the optimization solver used to estimate the hyperparameters of the covariance model of the kriging metamodel. 
#
# Introduction
# ------------
#
# In a kriging metamodel, there are various types of parameters which are estimated from the data. 
#
# * The parameters :math:`{\bf \beta}` associated with the deterministic trend. These parameters are computed based on linear least squares. 
# * The parameters of the covariance model. 
#
# The covariance model has two types of parameters.
#
# * The amplitude parameter :math:`\sigma^2` is estimated from the data. If the output dimension is equal to one, this parameter is estimated using the analytic variance estimator which maximizes the likelihood. Otherwise, if output dimension is greater than one or analytical sigma disabled, this parameter is estimated from numerical optimization. 
# * The other parameters :math:`{\bf \theta}\in\mathbb{R}^d` where :math:`d` is the spatial dimension of the covariance model. Often, the parameter :math:`{\bf \theta}` is a scale parameter. This step involves an optimization algorithm. 
#
# All these parameters are estimated with the `GeneralLinearModelAlgorithm` class. 
#
# The estimation of the :math:`{\bf \theta}` parameters is the step which has the highest CPU cost. Moreover, the maximization of likelihood may be associated with difficulties e.g. many local maximums or even the non convergence of the optimization algorithm. In this case, it might be useful to fine tune the optimization algorithm so that the convergence of the optimization algorithm is, hopefully, improved. 
#
# Furthermore, there are several situations in which the optimization can be initialized or completely bypassed. Suppose for example that we have already created an initial kriging metamodel with :math:`N` points and we want to add a single new point. 
#
# * It might be interesting to initialize the optimization algorithm with the optimum found for the previous kriging metamodel: this may reduce the number of iterations required to maximize the likelihood. 
# * We may as well completely bypass the optimization step: if the previous covariance model was correctly estimated, the update of the parameters may or may not significantly improve the estimates. 
#
# This is why the goal of this example is to see how to configure the optimization of the hyperparameters of a kriging metamodel.

# %%
# Definition of the model
# -----------------------

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We define the symbolic function which evaluates the output Y depending on the inputs E, F, L and I.

# %%
model = ot.SymbolicFunction(["E", "F", "L", "I"], ["F*L^3/(3*E*I)"])

# %%
# Then we define the distribution of the input random vector. 

# %%
# Young's modulus E
E = ot.Beta(0.9, 2.27, 2.5e7, 5.0e7) # in N/m^2
E.setDescription("E")
# Load F
F = ot.LogNormal() # in N
F.setParameter(ot.LogNormalMuSigma()([30.e3, 9e3, 15.e3]))
F.setDescription("F")
# Length L
L = ot.Uniform(250., 260.) # in cm
L.setDescription("L")
# Moment of inertia I
I = ot.Beta(2.5, 1.5, 310, 450) # in cm^4
I.setDescription("I")

# %%
# Finally, we define the dependency using a `NormalCopula`.

# %%
dim = 4 # number of inputs
R = ot.CorrelationMatrix(dim)
R[2, 3] = -0.2 
myCopula = ot.NormalCopula(ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(R))
myDistribution = ot.ComposedDistribution([E, F, L, I], myCopula)

# %%
# Create the design of experiments
# --------------------------------

# %%
# We consider a simple Monte-Carlo sampling as a design of experiments. This is why we generate an input sample using the `getSample` method of the distribution. Then we evaluate the output using the `model` function.

# %%
sampleSize_train = 10
X_train = myDistribution.getSample(sampleSize_train)
Y_train = model(X_train)

# %%
# Create the metamodel
# --------------------

# %%
# In order to create the kriging metamodel, we first select a constant trend with the `ConstantBasisFactory` class. Then we use a squared exponential covariance model. Finally, we use the `KrigingAlgorithm` class to create the kriging metamodel, taking the training sample, the covariance model and the trend basis as input arguments. 

# %%
dimension = myDistribution.getDimension()
basis = ot.ConstantBasisFactory(dimension).build()
covarianceModel = ot.SquaredExponential([1.]*dimension, [1.0])
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.run()
result = algo.getResult()
krigingMetamodel = result.getMetaModel()

# %%
# The `run` method has optimized the hyperparameters of the metamodel. 
#
# We can then print the constant trend of the metamodel, which have been estimated using the least squares method.

# %%
result.getTrendCoefficients()

# %%
# We can also print the hyperparameters of the covariance model, which have been estimated by maximizing the likelihood. 

# %%
basic_covariance_model = result.getCovarianceModel()
basic_covariance_model

# %%
# Get the optimizer algorithm
# ---------------------------

# %%
# The `getOptimizationAlgorithm` method returns the optimization algorithm used to optimize the :math:`{\bf \theta}` parameters of the `SquaredExponential` covariance model. 

# %%
solver = algo.getOptimizationAlgorithm()

# %%
# By default, the optimizer is the TNC algorithm with bound constraints. 

# %%
solverImplementation = solver.getImplementation()
solverImplementation.getClassName()

# %%
# The `getOptimizationBounds` method returns the bounds. The dimension of these bounds correspond to the spatial dimension of the covariance model. In the metamodeling context, this correspond to the input dimension of the model. 

# %%
bounds = algo.getOptimizationBounds()
bounds.getDimension()

# %%
lbounds = bounds.getLowerBound()
lbounds

# %%
ubounds = bounds.getUpperBound()
ubounds

# %%
# The `getOptimizeParameters` method returns `True` if these parameters are to be optimized.

# %%
algo.getOptimizeParameters()

# %%
# Configure the starting point of the optimization
# ------------------------------------------------

# %%
# The starting point of the optimization is based on the parameters of the covariance model. In the following example, we configure the parameters of the covariance model to the arbitrary values `[12.,34.,56.,78.]`.

# %%
covarianceModel = ot.SquaredExponential([12.,34.,56.,78.], [1.0])
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)

# %%
algo.run()

# %%
result = algo.getResult()
result.getCovarianceModel()

# %%
# In order to see the difference with the default optimization, we print the previous optimized covariance model.

# %%
basic_covariance_model

# %%
# We observe that this does not change much the values of the parameters in this case. 

# %%
# Disabling the optimization
# --------------------------

# %%
# It is sometimes useful to completely disable the optimization of the parameters. In order to see the effect of this, we first initialize the parameters of the covariance model with the arbitrary values `[12.,34.,56.,78.]`. 

# %%
covarianceModel = ot.SquaredExponential([12.,34.,56.,78.], [91.0])
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)

# %%
# The `setOptimizeParameters` method can be used to disable the optimization of the parameters.

# %%
algo.setOptimizeParameters(False)

# %%
# Then we run the algorithm and get the result. 

# %%
algo.run()
result = algo.getResult()

# %%
# We observe that the covariance model is unchanged.

# %%
result.getCovarianceModel()

# %%
# The trend, however, is still optimized, using linear least squares.

# %%
result.getTrendCoefficients()

# %%
# Reuse the parameters from a previous optimization
# -------------------------------------------------
#
# In this example, we show how to reuse the optimized parameters of a previous kriging and configure a new one. Furthermore, we disable the optimization so that the parameters of the covariance model are not updated. This make the process of adding a new point very fast: it improves the quality by adding a new point in the design of experiments without paying the price of the update of the covariance model.

# %%
# Step 1: Run a first kriging algorithm. 

# %%
dimension = myDistribution.getDimension()
basis = ot.ConstantBasisFactory(dimension).build()
covarianceModel = ot.SquaredExponential([1.]*dimension, [1.0])
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.run()
result = algo.getResult()
covarianceModel = result.getCovarianceModel()
covarianceModel

# %%
# Step 2: Create a new point and add it to the previous training design.

# %%
X_new = myDistribution.getSample(20)
Y_new = model(X_new)

# %%
X_train.add(X_new)
X_train.getSize()

# %%
Y_train.add(Y_new)
Y_train.getSize()

# %%
# Step 3: Create an updated kriging, using the new point with the old covariance parameters.

# %%
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.setOptimizeParameters(False)
algo.run()
result = algo.getResult()
notUpdatedCovarianceModel = result.getCovarianceModel()
notUpdatedCovarianceModel


# %%
def printCovarianceParameterChange(covarianceModel1,covarianceModel2):
    parameters1 = covarianceModel1.getFullParameter()
    parameters2 = covarianceModel2.getFullParameter()
    for i in range(parameters1.getDimension()):
        deltai = abs(parameters1[i] - parameters2[i])
        print("Change in the parameter #%d = %s" % (i,deltai))
    return


# %%
printCovarianceParameterChange(covarianceModel,notUpdatedCovarianceModel)

# %%
# We see that the parameters did not change *at all*: disabling the optimization allows to keep a constant covariance model. In a practical algorithm, we may, for example, add a block of 10 new points before updating the parameters of the covariance model. At this point, we may reuse the previous covariance model so that the optimization starts from a better point, compared to the parameters default values. This will reduce the cost of the optimization.

# %%
# Configure the optimization solver
# ---------------------------------

# %%
# The following example checks the robustness of the optimization of the kriging algorithm with respect to the optimization of the likelihood function in the covariance model parameters estimation. We use a `MultiStart` algorithm in order to avoid to be trapped by a local minimum. Furthermore, we generate the design of experiments using a `LHSExperiments`, which guarantees that the points will fill the space. 

# %%
sampleSize_train = 10
X_train = myDistribution.getSample(sampleSize_train)
Y_train = model(X_train)

# %%
# First, we create a multivariate distribution, based on independent `Uniform` marginals which have the bounds required by the covariance model.

# %%
distributions = ot.DistributionCollection()
for i in range(dim):
    distributions.add(ot.Uniform(lbounds[i], ubounds[i]))
boundedDistribution = ot.ComposedDistribution(distributions)

# %%
# We first generate a Latin Hypercube Sampling (LHS) design made of 25 points in the sample space. This LHS is optimized so as to fill the space.

# %%
K = 25 # design size
LHS = ot.LHSExperiment(boundedDistribution, K)
LHS.setAlwaysShuffle(True)
SA_profile = ot.GeometricProfile(10., 0.95, 20000)
LHS_optimization_algo = ot.SimulatedAnnealingLHS(LHS, SA_profile, ot.SpaceFillingC2())
LHS_optimization_algo.generate()
LHS_design = LHS_optimization_algo.getResult()
starting_points = LHS_design.getOptimalDesign()
starting_points.getSize()

# %%
# We can check that the minimum and maximum in the sample correspond to the bounds of the design of experiment.

# %%
lbounds, ubounds

# %%
starting_points.getMin(), starting_points.getMax()

# %%
# Then we create a `MultiStart` algorithm based on the LHS starting points.

# %%
solver.setMaximumIterationNumber(10000)
multiStartSolver = ot.MultiStart(solver, starting_points)

# %%
# Finally, we configure the optimization algorithm so as to use the `MultiStart` algorithm. 

# %%
algo = ot.KrigingAlgorithm(X_train, Y_train, covarianceModel, basis)
algo.setOptimizationAlgorithm(multiStartSolver)
algo.run()

# %%
finetune_covariance_model = result.getCovarianceModel()
finetune_covariance_model

# %%
printCovarianceParameterChange(finetune_covariance_model,basic_covariance_model)

# %%
# We see that there are no changes in the estimated parameters. This shows that the first optimization of the parameters worked fine.
