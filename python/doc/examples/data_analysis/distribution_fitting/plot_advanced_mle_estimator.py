"""
Fit a distribution with customized maximum likelihood
=====================================================
"""
# %%
# Introduction
# ------------
#
# When we perform distribution fitting using the :class:`~openturns.MaximumLikelihoodFactory`
# class, the default optimization solver sometimes fail to
# maximize the likelihood.
# This might because the optimization solver is not appropriate in
# this particular case, or because the bounds of the problem
# are not appropriate for the particular case.
# In this example, we configure the optimization solver by two different
# methods.
#
# - The first method sets a specific solver among the NLopt solvers.
#   Moreover, we customize the bounds of the parameters of the
#   distribution.
#   This helps the solver to avoid the part of the domain where
#   the distribution cannot be built.
# - The second method sets the :class:`~openturns.ResourceMap` key
#   that is used by the :class:`~openturns.MaximumLikelihoodFactory` as
#   the default solver.

import openturns as ot

# %%
# Simulate a sample
# -----------------

# %%
# In this example, we consider the :class:`~openturns.TruncatedNormal` distribution
# with default parameters and generate a large sample from it.
distribution = ot.TruncatedNormal()
print("Distribution=", distribution)
size = 10000
sample = distribution.getSample(size)

# %%
# We can use the :class:`TruncatedNormalFactory` class.
factory = ot.TruncatedNormalFactory()
fittedDistribution = factory.build(sample)
print("Fitted distribution=", fittedDistribution)

# %%
# But this method sometimes fails for specific distributions, for example
# when the sample size is very small or when the distribution has
# specific properties (e.g. some parameter is known beforehand).
# In these cases, general-purpose methods cannot be used, so that the
# next methods can be relevant.

# %%
# Define the estimation problem
# -----------------------------

# %%
# The truncated normal distribution has 4 parameters:
#
# - the mean and standard deviation parameters of the
#   underlying Normal distribution,
# - the minimum and maximum of the underlying truncated distribution.
#
# Estimating the parameters will be easier if two conditions are met:
#
# - the starting point (ie the initial value of the parameters)
#   of the optimization corresponds to the sample,
# - the bounds corresponds to the distribution.
#
# First, we define the starting point of the optimization,
# using statistics from the sample.

# %%
sample_min = sample.getMin()[0]
sample_max = sample.getMax()[0]
sample_mean = sample.computeMean()[0]
sigma_hat = sample.computeStandardDeviation()[0]
startingPoint = [sample_mean, sigma_hat, sample_min, sample_max]
print("startingPoint=", startingPoint)

# %%
# Secondly, we set the bounds of the parameters.
# We assume that the minimum and maximum cannot be lower than -1
# or greater than 1.
# The parameters have the following bounds:
#
# - :math:`\mu \in [-\infty, +\infty]` ;
# - :math:`\sigma \in [0, +\infty]` ;
# - :math:`a \in [-1, 1]` ;
# - :math:`b \in [-1, 1]`.
#
# But this may not give satisfactory results, because infinite parameters
# are impossible and a zero standard deviation is infeasible.
# Furthermore, we must have :math:`a < b`.
# We assume that we know that the mean parameter cannot be greater than 10
# and lower than -10.
# Similarly, the standard deviation cannot be greater than 10.
# Hence, we use the following bounds:
#
# - :math:`\mu \in [-10, +10]` ;
# - :math:`\sigma \in [\epsilon, +10]` ;
# - :math:`a \in [-1 - \epsilon, \bar{x} - \epsilon]` ;
# - :math:`b \in [\bar{x} + \epsilon, 1 + \epsilon]`
#
# where :math:`\bar{x}` is the sample mean and :math:`\epsilon`
# is a small parameter.

# %%
epsilon = 1.0e-3
bounds_lower = [-10.0, epsilon, -1.0 - epsilon, sample_mean + epsilon]
bounds_upper = [10.0, 10.0, sample_mean - epsilon, 1.0 + epsilon]
print("bounds_lower=", bounds_lower)
print("bounds_upper=", bounds_upper)

# %%
# The boolean `False` means unbounded and `True` means bounded.

# %%
finiteLowerBound = [True, True, True, True]
finiteUpperBound = [True, True, True, True]
interval = ot.Interval(bounds_lower, bounds_upper, finiteLowerBound, finiteUpperBound)

# %%
# Set the solver and bounds
# -------------------------

# %%
# The two methods that we suggest are based on the :class:`~openturns.MaximumLikelihoodFactory` class
# which uses the maximum likelihood estimator to estimate
# the parameters of the distribution depending on the sample.
# The first method is to use the :meth:`~openturns.MaximumLikelihoodFactory.setOptimizationAlgorithm`
# method.
# This sets the optimization algorithm as a replacement for the
# default optimization solver.


def printTruncatedNormalParameters(truncatedNormalDistribution):
    mu, sigma, a, b = truncatedNormalDistribution.getParameter()
    print(" ", truncatedNormalDistribution.getImplementation().getClassName())
    print("  mu= %.3f" % (mu))
    print("  sigma= %.4f" % (sigma))
    print("  a= %.2f" % (a))
    print("  b= %.2f" % (b))
    return None


# %%
factory = ot.MaximumLikelihoodFactory(distribution)
factory.setOptimizationBounds(interval)
distribution_MLE = factory.build(sample)
print("Fitted distribution with default solver:")
printTruncatedNormalParameters(distribution_MLE)
default_solver = factory.getOptimizationAlgorithm()
print("Default solver=", default_solver.getImplementation().getClassName())
solver = ot.NLopt("LN_COBYLA")
print("New solver=", solver.getClassName(), solver.getAlgorithmName())
solver.setStartingPoint(startingPoint)
factory.setOptimizationAlgorithm(solver)
print(
    "Updated solver=",
    factory.getOptimizationAlgorithm().getImplementation().getClassName(),
)
distribution_MLE = factory.build(sample)
print("Fitted distribution with new solver:")
printTruncatedNormalParameters(distribution_MLE)


# %%
# Use ResourceMap to set the solver
# ---------------------------------

# %%
# Another method is to use a special key of the :class:`ResourceMap`,
# which defines the default solver used by :class:`~openturns.MaximumLikelihoodFactory`.
# In this case, we do not define the starting point.

# %%
ot.ResourceMap.SetAsString(
    "MaximumLikelihoodFactory-DefaultOptimizationAlgorithm", "LN_COBYLA"
)
factory = ot.MaximumLikelihoodFactory(distribution)
factory.setOptimizationBounds(interval)
print(
    "Default solver=",
    factory.getOptimizationAlgorithm().getImplementation().getClassName(),
)
distribution_MLE = factory.build(sample)
print("Fitted distribution:")
printTruncatedNormalParameters(distribution_MLE)

# %%
# Fit a LogNormal with zero location parameter
# --------------------------------------------

# %%
# We now consider the :class:`~openturns.LogNormal` distribution
# and present two different topics.
#
# - We show how to estimate the parameters of a LogNormal distribution
#   which has a zero location parameter.
# - We compute the list of solvers which are compatible with a specific
#   MLE problem.

# %%
# We generate a large sample from the standard LogNormal distribution.

# %%
standardLogNormal = ot.LogNormal()
sample = standardLogNormal.getSample(1000)

# %%
# If we estimate the parameters using :class:`~openturns.LogNormalFactory`,
# the algorithm does not know that the true value of the location parameter
# :math:`\gamma` is zero.
# Hence, the estimator may not necessarily lead to an estimated parameter
# exactly equal to zero.
# In this example, we assume that we know that this parameter is zero.
# In this case, the simplest method is to use the
# :meth:`~openturns.MaximumLikelihoodFactory.setKnownParameter` method.
# Furthermore, we know that the absolute value of the mean of the underlying normal
# distribution cannot be greater than 5.
# Finally, we know that the standard deviation of the underlying
# normal distribution cannot be lower than, say, :math:`\epsilon = 10^{-4}`.

# %%
logNormalFactoryWithZeroLocation = ot.MaximumLikelihoodFactory(standardLogNormal)
logNormalFactoryWithZeroLocation.setKnownParameter([0.0], [2])
lowerBound = [-5.0, 1.0e-4]
upperBound = [5.0, -1.0]
finiteLowerBound = [True, True]
finiteUpperBound = [True, False]
bounds = ot.Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound)
logNormalFactoryWithZeroLocation.setOptimizationBounds(bounds)
fittedDistribution = logNormalFactoryWithZeroLocation.build(sample)
print("   fittedDistribution = ", fittedDistribution)

# %%
# See the list of solvers compatible with a given MLE problem
# -----------------------------------------------------------

# %%
# In the next script, we perform a loop over the algorithms
# from the :class:`NLopt` class and select the algorithms
# which can solve the problem.
# This is done using `try/except` statements.
# Moreover, some solvers pretend to solve the problem, but do
# not make any change to the parameters.
# In this case, we do not select them.

# %%
defaultParameter = standardLogNormal.getParameter()
defaultSolver = logNormalFactoryWithZeroLocation.getOptimizationAlgorithm()
startingPoint = defaultSolver.getStartingPoint()
print("Default solver = ", defaultSolver.getImplementation().getClassName())
availableSolverList = []
differenceThreshold = 1.0e-8
for algorithmName in ot.NLopt.GetAlgorithmNames():
    print("- ", algorithmName)
    solver = ot.NLopt(algorithmName)
    solver.setStartingPoint(startingPoint)
    logNormalFactoryWithZeroLocation.setOptimizationAlgorithm(solver)
    try:
        fittedDistribution = logNormalFactoryWithZeroLocation.build(sample)
        print("   fittedDistribution = ", fittedDistribution)
        parameter = fittedDistribution.getParameter()
        differenceOfParameters = parameter - defaultParameter
        if differenceOfParameters.norm() > differenceThreshold:
            # Consider only solvers which make a difference after optimization
            availableSolverList.append(algorithmName)
    except TypeError:
        print("   Fail : Impossible to build with ", algorithmName)

# %%
print("Available solvers = ", len(availableSolverList))
for name in availableSolverList:
    print("- ", name)

# %%
# Pick one and try it.

# %%
algorithmName = "LD_SLSQP"
solver = ot.NLopt(algorithmName)
print("   New solver = ", solver.getClassName(), solver.getAlgorithmName())
solver.setStartingPoint(startingPoint)
logNormalFactoryWithZeroLocation.setOptimizationAlgorithm(solver)
fittedDistribution = logNormalFactoryWithZeroLocation.build(sample)
print("   fittedDistribution = ", fittedDistribution)
